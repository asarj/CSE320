#include "const.h"

/**
    Additional functions
*/
int compareStrings(char *first, char *second);
void int2Bin(int n);
int checkIfDir(char *dir);
int stringLength(char *str);
void manStrCpy(char *source, char *dest);
int path_push_directory(char *name);
int path_pop_directory();
void serialize_helper(int depth);
int insert_header(int type, int depth, off_t size, FILE *f, DIR *dir);