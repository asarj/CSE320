#include "const.h"

/**
    Additional functions
*/
int compareStrings(char *first, char *second);
//void int2Bin(int n);
int checkIfArg(char *arg);
int stringLength(char *str);
void manStrCpy(char *source, char *dest);
//int path_push_directory(char *name);
//int path_pop_directory();
//void serialize_helper(int depth);
int insert_header(int type, int depth, off_t size, FILE *f, DIR *dir);
int check_header_data();
int get_type();
int get_depth();
long get_size();
mode_t get_st_mode();
off_t get_st_size();
char * get_filename(int len);