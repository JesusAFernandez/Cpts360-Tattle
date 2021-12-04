#include <stdio.h>
#include "tattle.h"
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {
 tattle(argc, argv);
 return 0;
}