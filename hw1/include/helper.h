#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <utime.h>
#include <dirent.h>
#include <sys/stat.h>

#include "const.h"

/**
    Additional functions
*/
int compareStrings(char *first, char *second);
void int2Bin(int n);
int checkIfDir(char *dir);
int stringLength(char *str);
void manStrCpy(char *source, char *dest);