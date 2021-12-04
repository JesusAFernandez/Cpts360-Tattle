#include <utmp.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <pwd.h>

#include "tattle.h"

void saveEntries(struct utmp *tmp, long int filePos);  

char *userNames,*fileName, *date, *Ttime; 
//if no input then will input everything  
node * ptr = NULL; 
bool pass, special; 
void tattle(int argc, char **argv){
  int opt; 
  while((opt = getopt(argc, argv, "d:f:t:u:")) != -1){
    switch(opt){
    case 'd':
      assert(strlen(optarg) == 8);
      date = optarg;
      break;
    case 'f':
      fileName = optarg; 
      break;
    case 't':
      assert(strlen(optarg) == 5); 
     Ttime = optarg; 
      break;
    case 'u':
      userNames = optarg; 
      break; 
    default: 
      fprintf(stderr, "Commands:  -d(ate) -f(ilename) -t(ime) -u(sername)\n"); 
      exit(EXIT_FAILURE); 
    }
  }
  readFile();
  destructor();  
}
void readFile(){ 
  struct utmp log;
  if(fileName == NULL){
    fileName = WTMP_FILE; 
  }
  FILE * read = fopen(fileName, "rb"); 
  if(read == NULL){
    perror(fileName); 
    exit(EXIT_FAILURE); 
  }
  while(fread(&log, sizeof(struct utmp), 1, read) > 0){
    if(log.ut_type == USER_PROCESS && strcmp(log.ut_user, "root") != 0){
      long int x = ftell(read); 
      saveEntries(&log, x); 
    }
  }
  printing();  
  fclose(read); 
}

long int getLogout(char * line, long int filePos){
  struct utmp log;
  FILE * read = fopen(fileName, "rb"); 
  if(read == NULL){
    perror(fileName); 
    exit(EXIT_FAILURE); 
  }
  fseek(read, filePos, SEEK_SET); 
    while(fread(&log, sizeof(struct utmp), 1, read) > 0){
    if(strcmp(line, log.ut_line) == 0 && (log.ut_type == DEAD_PROCESS || log.ut_type == BOOT_TIME)){
      fclose(read); 
      return log.ut_time; 
    }
    else if(strcmp(line, log.ut_line) == 0 ){
      int filePos = ftell(read); 
      saveEntries(&log, filePos); 
      fclose(read); 
      return log.ut_time; 
    }
}
  //Still logged on
  fclose(read);  
  return -1; 
}

void saveEntries(struct utmp *record, long int filePos){ 

  struct tm *dtmp, *tmp; 
  char Date[10] = {0},  
       Time[10] = {0}, 
     logOUT[10] = {0};
  time_t t  = record->ut_time, tt;
  tt = getLogout(record->ut_line, filePos); 
  tmp = localtime(&tt); 
  strftime(logOUT, sizeof(logOUT), "%H:%M", tmp);
  dtmp = localtime(&t);
  strftime(Date, sizeof(Date), "%D", dtmp); 
  strftime(Time, sizeof(Time), "%H:%M", dtmp);
  //strftime(logOUT, sizeof(logOUT), "%H:%M", tmp);
//** CHECK IF INPUTS MATCH UP IF ANY **//
  pass = true; // RESET TO TRUE EVERY LOOP
  if(date != NULL && Ttime != NULL){
    pass = dateCheck(Date); 
  }else if(date != NULL && Ttime == NULL){// TAKES CARE OF TIME OVERLAP 
    char tempLogout[10] = {0},
          tempLogin[10] = {0};
    strftime(tempLogout, sizeof(tempLogout), "%H%M", tmp); 
    strftime(tempLogin, sizeof(tempLogin), "%H%M", dtmp);
    int long logout = atoi(tempLogout);
    int long login = atoi(tempLogin); 
    if(logout > 0000 || login < 2400){
      pass = dateCheck(Date); 
    }

  }
  if(Ttime != NULL && pass != false)
    pass = timeCheck(Time, Date); 
  if(userNames != NULL && pass != false)
    pass = userCheck(record->ut_user); 
  /***** SINCE ALL TEST PASSED NOW WE CAN GET LOGOUT TIMES
         WITH THE FILE POSTION WE SAVED FROM FTELL() *****/  
  if(pass == true){ 
    node * newNode = malloc(sizeof(node)); 
    if(tt != -1){ //Gets return of logout
      //strftime(logOUT, sizeof(logOUT), "%H:%M", tmp);
      sprintf(newNode->logOut, "%s", logOUT); 
    }
    else{
      sprintf(newNode->logOut, "%s", "(still logged in)"); 
    }
    newNode->next = NULL;
    sprintf(newNode->user, "%s", record->ut_user); 
    sprintf(newNode->tty, "%s", record->ut_line); 
    sprintf(newNode->host, "%s",record->ut_host); 
    sprintf(newNode->Time,"%s",Time); 
    sprintf(newNode->date, "%s", Date); 
    newNode->comp = t;
    insertInOrder(newNode); 
  }
}

void insertInOrder(node * newNod){
  if(ptr == NULL){ 
    ptr = newNod; 
  }else if(newNod->comp < ptr->comp){
    newNod->next = ptr; 
    ptr = newNod; 
  }else{
    node * current = ptr; 
    while(current->next != NULL && newNod->comp > current->next->comp){
      current = current->next; 
    }
    newNod->next = current->next;
    current->next = newNod; 
  }  
}

void printing(void){
node * temp = ptr; 
  while(temp != NULL){
    printf("    login: %s\n", temp->user);
    printf("      tty: %s\n", temp->tty); 
    printf("   log on: %s %s\n", temp->date, temp->Time); 
    printf("  log off: %s\n",  temp->logOut); 
    printf("from host: %s\n", temp->host); 
    printf("\n"); 
    temp = temp->next; 
  }
}

/*** HELPER FUNCTIONS TO MAKE CODE MORE CLEAR ON TOP ***/
bool dateCheck(char * Date){
    if(strcmp(date, Date) == 0){
      return true; 
    }
    else
      return false; 
}

bool timeCheck(char * Time, char * Date){
  char Today[10] = {0}; 
  struct tm * cur;
  if(date == NULL){
      time_t currDay = time(NULL); 
      cur = localtime(&currDay); 
      strftime(Today, sizeof(Today), "%D", cur);
      if(strcmp(Today, Date) == 0){
        //** jump to see if time matches **//
      }
      else
        return false; 
    }
  if(strcmp(Time, Ttime) == 0)
    return  true; 
  else
   return false; 
 }

 bool userCheck(char * User){
    struct passwd* pw; 
    char copy[1024] = {0}; 
    sprintf(copy, "%s", userNames);   
    char * tmp = strtok(copy, ",");  
    while(tmp != NULL){
      if((pw = getpwnam(tmp)) == NULL){
        fprintf( stderr, "getpwnam: unknown %s\n",
        tmp );
        exit(EXIT_FAILURE); 
      }
      if(strcmp(tmp, User)== 0){ 
        return true;
      }
      tmp = strtok(NULL, ","); 
    } 
    return false; 
 }
/** DESTRUCTOR **/
 void destructor(void){
  node * temp;
  while(ptr != NULL){
    temp = ptr;
    ptr = ptr->next;
    free(temp); 
  }
 ptr = NULL; 
} 


