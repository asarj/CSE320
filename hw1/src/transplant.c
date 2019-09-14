#include "const.h"
#include "transplant.h"
#include "debug.h"
#include "helper.h"
#include "stdlib.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the pathname of the current file or directory
 * as well as other data have been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/*
 * To build the program and run tests:
 * make
 * mkdir -p bin build
 * gcc build/transplant.o build/main.o -o bin/transplant
 * gcc -Wall -Werror -std=gnu11 -g -DDEBUG -I include build/transplant.o tests/hw1_tests.c -lcriterion -o bin/transplant
 *
*/

/*
 * A function that returns printable names for the record types, for use in
 * generating debugging printout.
 */
static char *record_type_name(int i) {
    switch(i) {
        case START_OF_TRANSMISSION:
            return "START_OF_TRANSMISSION";
        case END_OF_TRANSMISSION:
            return "END_OF_TRANSMISSION";
        case START_OF_DIRECTORY:
            return "START_OF_DIRECTORY";
        case END_OF_DIRECTORY:
            return "END_OF_DIRECTORY";
        case DIRECTORY_ENTRY:
            return "DIRECTORY_ENTRY";
        case FILE_DATA:
            return "FILE_DATA";
        default:
            return "UNKNOWN";
    }
}

/*
 * @brief  Initialize path_buf to a specified base path.
 * @details  This function copies its null-terminated argument string into
 * path_buf, including its terminating null byte.
 * The function fails if the argument string, including the terminating
 * null byte, is longer than the size of path_buf.  The path_length variable
 * is set to the length of the string in path_buf, not including the terminating
 * null byte.
 *
 * @param  Pathname to be copied into path_buf.
 * @return 0 on success, -1 in case of error
 */
int path_init(char *name) {
    // To be implemented.
    // debug("%s", name);
    path_length = stringLength(name);
    // debug("%d", PATH_MAX);
    if(path_length > PATH_MAX){
        path_length = -1;
        return -1;
    }
    path_length = path_length + 1;
    char *ptr = name_buf;
    manStrCpy(name, (char *) &name_buf);
    // debug("%s", path_buf);
    return 0;
}

/*
 * @brief  Append an additional component to the end of the pathname in path_buf.
 * @details  This function assumes that path_buf has been initialized to a valid
 * string.  It appends to the existing string the path separator character '/',
 * followed by the string given as argument, including its terminating null byte.
 * The length of the new string, including the terminating null byte, must be
 * no more than the size of path_buf.  The variable path_length is updated to
 * remain consistent with the length of the string in path_buf.
 *
 * @param  The string to be appended to the path in path_buf.  The string must
 * not contain any occurrences of the path separator character '/'.
 * @return 0 in case of success, -1 otherwise.
 */
int path_push(char *name) {
    // To be implemented.
    // debug("%s", name_buf);
    if(*path_buf == '\0'){
        manStrCpy(name_buf, (char *)&path_buf);
    }
    char *namePtr = name;
    while (*namePtr != '\0'){
        if(*namePtr == '/')
            return -1;
        namePtr++;
    }

    int nameLen = stringLength(name);
    char *regPtr = path_buf;
    for(int i = 0; i < path_length; i++){
        if(*regPtr == '\0'){
            char *slash = "/";
            manStrCpy(slash, regPtr);
            regPtr++;
            manStrCpy(name, regPtr);
            path_length += nameLen;
            // printf("%s", path_buf);
            break;
        }
        regPtr++;
    }
    // debug("%s", path_buf);
    return 0;
}

/*
 * @brief  Remove the last component from the end of the pathname.
 * @details  This function assumes that path_buf contains a non-empty string.
 * It removes the suffix of this string that starts at the last occurrence
 * of the path separator character '/'.  If there is no such occurrence,
 * then the entire string is removed, leaving an empty string in path_buf.
 * The variable path_length is updated to remain consistent with the length
 * of the string in path_buf.  The function fails if path_buf is originally
 * empty, so that there is no path component to be removed.
 *
 * @return 0 in case of success, -1 otherwise.
 */
int path_pop() {
    // To be implemented.
    // debug("%s", path_buf);
    if(*path_buf == '\0'){
        return -1;
    }
    int found = -1;
    char *ptr = path_buf;
    int lastSlash = 0;
    int iter = 0;
    while (*ptr != '\0'){
        if(*ptr == '/'){
            lastSlash = iter;
            found = 0;
        }
        iter++;
        ptr++;
    }

    if(found == -1){
        *path_buf = '\0';
    }
    else{
        char *replPtr = path_buf;
        int i = 0;
        while(i <= lastSlash){
            replPtr++;
            i++;
        }
        debug("%s", path_buf);
        *replPtr = '\0';
        debug("%s", path_buf);
    }
    return 0;
}

/*
 * @brief Deserialize directory contents into an existing directory.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory.  It reads (from the standard input) a sequence of DIRECTORY_ENTRY
 * records bracketed by a START_OF_DIRECTORY and END_OF_DIRECTORY record at the
 * same depth and it recreates the entries, leaving the deserialized files and
 * directories within the directory named by path_buf.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * each of the records processed.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including depth fields in the records read that do not match the
 * expected value, the records to be processed to not being with START_OF_DIRECTORY
 * or end with END_OF_DIRECTORY, or an I/O error occurs either while reading
 * the records from the standard input or in creating deserialized files and
 * directories.
 */
int deserialize_directory(int depth) {
    // To be implemented.
    return -1;
}

/*
 * @brief Deserialize the contents of a single file.
 * @details  This function assumes that path_buf contains the name of a file
 * to be deserialized.  The file must not already exist, unless the ``clobber''
 * bit is set in the global_options variable.  It reads (from the standard input)
 * a single FILE_DATA record containing the file content and it recreates the file
 * from the content.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * the FILE_DATA record.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including a depth field in the FILE_DATA record that does not match
 * the expected value, the record read is not a FILE_DATA record, the file to
 * be created already exists, or an I/O error occurs either while reading
 * the FILE_DATA record from the standard input or while re-creating the
 * deserialized file.
 */
int deserialize_file(int depth);

/*
 * @brief  Serialize the contents of a directory as a sequence of records written
 * to the standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory to be serialized.  It serializes the contents of that directory as a
 * sequence of records that begins with a START_OF_DIRECTORY record, ends with an
 * END_OF_DIRECTORY record, and with the intervening records all of type DIRECTORY_ENTRY.
 *
 * @param depth  The value of the depth field that is expected to occur in the
 * START_OF_DIRECTORY, DIRECTORY_ENTRY, and END_OF_DIRECTORY records processed.
 * Note that this depth pertains only to the "top-level" records in the sequence:
 * DIRECTORY_ENTRY records may be recursively followed by similar sequence of
 * records describing sub-directories at a greater depth.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open files, failure to traverse directories, and I/O errors
 * that occur while reading file content and writing to standard output.
 */
int serialize_directory(int depth) {
    // To be implemented.
    return -1;
}

/*
 * @brief  Serialize the contents of a file as a single record written to the
 * standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * file to be serialized.  It serializes the contents of that file as a single
 * FILE_DATA record emitted to the standard output.
 *
 * @param depth  The value to be used in the depth field of the FILE_DATA record.
 * @param size  The number of bytes of data in the file to be serialized.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open the file, too many or not enough data bytes read
 * from the file, and I/O errors reading the file data or writing to standard output.
 */
int serialize_file(int depth, off_t size) {
    // To be implemented.
    return -1;
}

/**
 * @brief Serializes a tree of files and directories, writes
 * serialized data to standard output.
 * @details This function assumes path_buf has been initialized with the pathname
 * of a directory whose contents are to be serialized.  It traverses the tree of
 * files and directories contained in this directory (not including the directory
 * itself) and it emits on the standard output a sequence of bytes from which the
 * tree can be reconstructed.  Options that modify the behavior are obtained from
 * the global_options variable.
 *
 * @return 0 if serialization completes without error, -1 if an error occurs.
 */
int serialize() {
    // To be implemented.
    return -1;
}

/**
 * @brief Reads serialized data from the standard input and reconstructs from it
 * a tree of files and directories.
 * @details  This function assumes path_buf has been initialized with the pathname
 * of a directory into which a tree of files and directories is to be placed.
 * If the directory does not already exist, it is created.  The function then reads
 * from from the standard input a sequence of bytes that represent a serialized tree
 * of files and directories in the format written by serialize() and it reconstructs
 * the tree within the specified directory.  Options that modify the behavior are
 * obtained from the global_options variable.
 *
 * @return 0 if deserialization completes without error, -1 if an error occurs.
 */
int deserialize() {
    // To be implemented.
    return -1;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv) {
    // To be implemented.
    global_options = 0;
    if(argc < 2 || *argv == NULL){
        return -1;
    }

    char *ptr;
    int i;
    argv++;

    // Check initial args, either -h or -s|-d
    int validated = -1, hFlag = -1, sFlag = -1, dFlag = -1, pFlag = -1, cFlag = -1;
    char* dirValue = "./";
    // path_init(dirValue);
    for (i = 1; i < argc; i++){
        ptr = *argv;
        if(hFlag != 0){
            hFlag = compareStrings(ptr, "-h");
            if(hFlag == 0){
                global_options |= 0x1;
                USAGE(*(--argv), 0);
                return 0;
            }
        }

        if(sFlag != 0){
            sFlag = compareStrings(ptr, "-s");
            // Serialize the directory
            if (sFlag == 0){
                global_options |= 0x2;
                break;
            }
        }

        if(dFlag != 0){
            dFlag = compareStrings(ptr, "-d");
            // De-serialize the directory
            if (dFlag == 0){
                global_options |= 0x4;
                break;
            }
        }
    }

    if (sFlag == 0 || dFlag == 0){
        validated = 0;
    }
    else{
        global_options = 0;
        return -1;
    }
    argv++;
    ptr = *argv;


    if(i == argc || ptr == NULL){
        path_init(dirValue);
        return validated;
    }
    if(validated == 0){
        while(ptr != NULL){
            if(cFlag != 0){
                cFlag = compareStrings(ptr, "-c");
                if(cFlag == 0){
                    if(sFlag == 0){
                        global_options = 0;
                        return -1;
                    }
                    else{
                        global_options |= 0x8;
                    }
                }
            }

            if(pFlag != 0){
                pFlag = compareStrings(ptr, "-p");
                if (pFlag == 0){
                    argv++;
                    ptr = *argv;
                    if(ptr == NULL){
                        return -1;
                    }

                    int dirFound = checkIfDir(ptr);

                    if(dirFound == -1){
                        global_options = 0;
                        return -1;
                    }
                    else{
                        dirValue = ptr;

                    }
                }
            }
            argv++;
            ptr = *argv;
        }
    }
    path_init(dirValue);
    return validated;
}

int compareStrings(char *first, char *second){
    char ptr1;
    char ptr2;
    ptr1 = *first;
    ptr2 = *second;

    while(ptr1 != 0 && ptr2 != 0){
        if((ptr1 != ptr2)){
            return -1;
        }
        ptr1 = *(first++);
        ptr2 = *(second++);

    }
    return 0;
}

int checkIfDir(char *dir){
    char ptr;
    ptr = *dir;
    if(ptr == 46 || ptr == 47){
        return 0;
    }
    return -1;
}

void int2Bin(int n){
    while(n){
        if (n & 1){
            printf("1");
        }
        else{
            printf("0");
        }
        n >>= 1;
    }
    printf("\n");
}

int stringLength(char *name){
    int count = 0;

    while(*(name) != '\0'){
        count++;
        name++;
    }
    return count;
}
void manStrCpy(char *source, char *dest){
    while(*source != '\0'){
        *dest++ = *source++;
//        source++; dest++;
    }
    *dest = '\0';
}
