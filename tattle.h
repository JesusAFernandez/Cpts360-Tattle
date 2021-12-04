#include <stdbool.h>
typedef struct node{
  char user[1024],
  	   tty[20],
  	   host[20],  
       Time[20], 
       date[20],
       logOut[20];  
  long int comp;  
  
  struct node * next;   
}node; 
//void saveEntries(struct utmp *tmp, long int filePos); 
void tattle(int argc, char * argv[]); 
void readFile(void); 

void insertInOrder(node * ptr); 
void printing(void);
bool dateCheck(char * Date); 
bool timeCheck(char * Time, char * Date); 
bool userCheck(char * User); 
void destructor(void);