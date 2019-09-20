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
    manStrCpy(name, (char *) &path_buf);
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
            if(path_length > PATH_MAX)
                return -1;
            // debug("%s", path_buf);
            break;
        }
        regPtr++;
    }
    // debug("%s", path_buf);
    return 0;
}

//int path_push_directory(char *name){
//    if(*path_buf == '\0') {
//        manStrCpy(name_buf, (char *) &path_buf);
//    }
//
//    int nameLen = stringLength(name);
//    char *regPtr = path_buf;
//    for(int i = 0; i < path_length; i++){
//        if(*regPtr == '\0'){
//            if(compareStrings("./", path_buf) != 0){}
//            char *slash = "/";
//            manStrCpy(slash, regPtr);
//            regPtr++;
//            manStrCpy(name, regPtr);
//            regPtr++;
//            manStrCpy(slash, regPtr);
//            path_length += (nameLen + 1);
//
//            // debug("%s", path_buf);
//            break;
//        }
//        regPtr++;
//    }
//    // debug("%s", path_buf);
//    return 0;
//
//}

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
        while(i < lastSlash){
            replPtr++;
            i++;
        }
//        debug("%s\n", path_buf);
        *replPtr = '\0';
//        debug("%s\n", path_buf);
    }
    return 0;
}

//int path_pop_directory(){
//    if(*path_buf == '\0'){
//        return -1;
//    }
//    int found = -1;
//    char *ptr = path_buf;
//    int lastSlash = 0;
//    int secLastSlash = 0;
//    int iter = 0;
//    while (*ptr != '\0'){
//        if(*ptr == '/'){
//            if(lastSlash != 0){
//                secLastSlash = lastSlash;
//            }
//            lastSlash = iter;
//            found = 0;
//        }
//        iter++;
//        ptr++;
//    }
//
//    if(found == -1){
//        *path_buf = '\0';
//    }
//    else{
//        char *replPtr = path_buf;
//        int i = 0;
//        while(i <= secLastSlash){
//            replPtr++;
//            i++;
//        }
////        debug("%s", path_buf);
//        *replPtr = '\0';
////        debug("%s", path_buf);
//    }
//    return 0;
//}

// insert deserialize functions back here

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
    int st = -1, ret, ins = -1;
    DIR *dir = opendir(path_buf);
    if (dir == NULL)
        return -1;

    struct dirent *de;
//    debug("Depth: %d", depth);
    ins = insert_header(DIRECTORY_ENTRY, depth, -1, NULL, NULL);
    if(ins == -1)
        return -1;
//    debug("%s\n", "Directory entry");
    ins = insert_header(START_OF_DIRECTORY, ++depth, -1, NULL, NULL);
    if(ins == -1)
        return -1;
//    debug("%s\n", "Start of directory");
    while ((de = readdir(dir)) != NULL){

        struct stat stat_buf;
        if(!(compareStrings(de->d_name, "..") == 0 || compareStrings(de->d_name, ".") == 0)){
            int pp = path_push(de->d_name);
            if(pp == -1)
                return -1;
            manStrCpy(de->d_name, name_buf);
            st = stat(path_buf, &stat_buf);
        }

        if(st == -1){
            continue;
        }
        else{
            if(compareStrings(de->d_name, "..") == 0 || compareStrings(de->d_name, ".") == 0){
                continue;
            }
//            debug("path_buf: %s\n", path_buf);
            int isFile = S_ISREG(stat_buf.st_mode);
            if(isFile){
//                debug("Serializing file: %s in %s\n", de->d_name, path_buf);
                ret = serialize_file(depth, stat_buf.st_size);
                if(ret == -1){
                    return -1;
                }
                int ppo = path_pop();
                if(ppo == -1)
                    return -1;
                manStrCpy("\0", name_buf);
            }
            else if(S_ISDIR(stat_buf.st_mode)){
//                debug("Serializing Directory: %s in %s\n", de->d_name, path_buf);
                ret = serialize_directory(depth);
                if(ret == -1)
                    return -1;
                int ppo = path_pop();
                if(ppo == -1)
                    return -1;
                manStrCpy("\0", name_buf);
            }
            else{
                continue;
            }
        }
    }
    ins = insert_header(END_OF_DIRECTORY, depth, -1, NULL, NULL);
    if(ins == -1)
        return -1;
//    debug("%s\n", "End of directory");
    int cl = closedir(dir);
    if(cl == -1)
        return -1;
    return 0;
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
    int ins = -1;
    FILE *f = fopen(path_buf, "r");
    if(f == NULL){
        fclose(f);
        return -1;
    }
    ins = insert_header(DIRECTORY_ENTRY, depth, -1, f, NULL);
    if(ins == -1)
        return -1;
    ins = insert_header(FILE_DATA, depth, size, f, NULL);
    if(ins == -1)
        return -1;
    return 0;
}

int insert_header(int type, int depth, off_t size, FILE *f, DIR *dir){
    putchar(MAGIC0);
    putchar(MAGIC1);
    putchar(MAGIC2);
    putchar(type);

    struct stat stat_buf;
    int st = stat(path_buf, &stat_buf);

    if(sizeof(depth) == 4){
        for(int i = sizeof(depth) - 1; i >= 0; i--){
//            debug("%04x ", *((char *) &depth + i));
            char bit = *((char *) &depth + i);
//            debug("%o ", bit);
            putchar(bit);
        }
    }
    else {
        return -1;
    }
    if(type == DIRECTORY_ENTRY){
        long t = 16 + 12 + stringLength(name_buf);
        for(int i = sizeof(t) - 1; i >= 0; i--){
//            debug("%04x ", *((char *) &x + i));
            char bit = *((char *) &t + i);
//            debug("%o ", bit);
            putchar(bit);
        }
//        debug("\n");
        if(st != -1){
//            putchar(stat_buf.st_mode);
            for(int i = sizeof(stat_buf.st_mode) - 1; i >= 0; i--){
//            debug("%04x ", *((char *) &x + i));
                char bit = *((char *) &stat_buf.st_mode + i);
//                debug("%o ", bit);
                putchar(bit);
            }
//            putchar(stat_buf.st_size);
            for(int i = sizeof(stat_buf.st_size) - 1; i >= 0; i--){
//            debug("%04x ", *((char *) &x + i));
                char bit = *((char *) &stat_buf.st_size + i);
//                debug("%o ", bit);
                putchar(bit);
            }
//            for(int i = stringLength(name_buf) - 1; i >= 0; i--){
////            debug("%04x ", *((char *) &x + i));
//                char bit = *((char *) &name_buf + i);
//                debug("%o ", bit);
//                putchar(bit);
//            }
            for(int i = 0; i < stringLength(name_buf); i++){
//            debug("%04x ", *((char *) &x + i));
                char bit = *((char *) &name_buf + i);
//                debug("%o ", bit);
                putchar(bit);
            }
        }
        else{
//            return -1;
        }
    }
    else if(type == FILE_DATA){
        long t = 16 + stat_buf.st_size;
        for(int i = sizeof(t) - 1; i >= 0; i--){
//            debug("%04x ", *((char *) &x + i));
            char bit = *((char *) &t + i);
//            debug("%o ", bit);
            putchar(bit);
        }
//        debug("\n");
        int counter = 0;
        char c = fgetc(f);
        while (c != EOF){
            putchar(c);
            counter++;
            c = fgetc(f);
        }
        fclose(f);
        if(counter != stat_buf.st_size)
            return -1;
    }
    else{
        if(size != -1){
            if(sizeof(size) == 8){
                for(int i = sizeof(size) - 1; i >= 0; i--){
//                debug("%04x ", *((char *) &size + i));
                    char bit = *((char *) &size + i);
                    debug("%o ", bit);
                    putchar(bit);
                }
//                debug("\n");
            }
            else {
                return -1;
            }
        }
        else{
            off_t x = 16;
            for(int i = sizeof(x) - 1; i >= 0; i--){
//            debug("%04x ", *((char *) &x + i));
                char bit = *((char *) &x + i);
//                debug("%d ", bit);
                putchar(bit);
            }
//            debug("\n");
        }
    }
    return 0;

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
    int ret, st = -1, ins = -1;
    DIR *dir = opendir(path_buf);
    int depth = 0;
    struct dirent *de;
    if(dir == NULL)
        return -1;
    ins = insert_header(START_OF_TRANSMISSION, depth, -1, NULL, NULL);
    if(ins == -1)
        return -1;
//    debug("%s\n", "Start of transmission");
    ins = insert_header(START_OF_DIRECTORY, ++depth, -1, NULL, NULL);
    if(ins == -1)
        return -1;
//    debug("%s\n", "Start of directory");
//    debug("Depth: %d", depth);
    while ((de = readdir(dir)) != NULL){

        struct stat stat_buf;
        if(!(compareStrings(de->d_name, "..") == 0 || compareStrings(de->d_name, ".") == 0)){
            int pp = path_push(de->d_name);
            if(pp == -1)
                return -1;
            manStrCpy(de->d_name, name_buf);
            st = stat(path_buf, &stat_buf); // gives mode and size_t
        }

        if(st == -1){
            continue;
        }
        else{
            if(compareStrings(de->d_name, "..") == 0 || compareStrings(de->d_name, ".") == 0){
                continue;
            }
//            debug("path_buf: %s\n", path_buf);
            int isFile = S_ISREG(stat_buf.st_mode);
            if(isFile){
//                debug("Serializing file: %s in %s\n", de->d_name, path_buf);
                ret = serialize_file(depth, stat_buf.st_size);

                if(ret == -1){
                    return -1;
                }
                int ppo = path_pop();
                if(ppo == -1)
                    return -1;
            }
            else if(S_ISDIR(stat_buf.st_mode)){
//                debug("Serializing directory: %s in %s\n", de->d_name, path_buf);
                ret = serialize_directory(depth);
                if(ret == -1) {
                    return -1;
                }
                int ppo = path_pop();
                if(ppo == -1)
                    return -1;
            }
            else{
                continue;
            }
        }
    }
    ins = insert_header(END_OF_DIRECTORY, depth, -1, NULL, NULL);
    if(ins == -1)
        return -1;
//    debug("%s\n", "End of directory");
    ins = insert_header(END_OF_TRANSMISSION, 0, -1, NULL, NULL);
    if(ins == -1)
        return -1;
//    debug("%s\n", "End of transmission");
    int cl = closedir(dir);
    if(cl == -1)
        return -1;
    return 0;
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
//    info("Deserializing...");

    int ret = -1;

    ret = check_header_data(START_OF_TRANSMISSION);
    if(ret == -1)
        return -1;
    // info("Found magic");
    int t = get_type();
    if(t < 0 || t > FILE_DATA)
        return -1;
    // debug("%d", t);
    int d = get_depth();
    // debug("%d", d);
    long s = get_size();
    if(s != 16)
        return -1;
        // debug("%ld", s);
    else{
        DIR *dir = opendir(path_buf);
        if(dir == NULL){
            mkdir(path_buf, 0700);
        }
        ret = deserialize_directory(0);
        if(ret == -1)
            return -1;
        // else {
        //     ret = check_header_data(END_OF_TRANSMISSION);
        //     if (ret == -1)
        //         return -1;
        // }
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
    info("Starting deserialize directory");
    int ret = check_header_data();
    debug("%s\n", path_buf);
    if(ret == -1)
        // info("Invalid header data");
        return -1;
    int type = get_type();
    debug("%d\n", type);
    int d = get_depth();
    debug("%d\n", d);
    long size = get_size();
    debug("%ld\n", size);

    if(type == DIRECTORY_ENTRY){
        info("Dir entry");
        mode_t perm = get_st_mode();
        off_t st_size = get_st_size();
        int filename_len = size - 16 - 12;
        debug("%d\n", filename_len);
        char *filename = get_filename(filename_len);
        debug("%s\n", filename);
        int ppu = path_push(filename);
        debug("%s", path_buf);
        if(ppu == -1)
            return -1;
        if(S_ISDIR(perm)){
            info("Deserializing directory");
            // DIR *dir = opendir(path_buf);
            // if(dir == NULL){
            //     if(global_options & 8){

            //     }
            //     else{
            //         return -1;
            //     }
            // }
            int mk = mkdir(path_buf, 0700);
            debug("%d", mk);
            if(mk == -1)
                return -1;
            info("Made dir");
            ret = deserialize_directory(depth);
            if(ret == -1){
                // info("Something wrong happened");
                return 0;
            }

            info("Successful dir");
            chmod(path_buf, perm & 0777);
            // ret = deserialize_directory(depth);
            // if(ret == -1)
            //     return -1;
            int ppo = path_pop();
            if(ppo == -1)
                return -1;
            // ret = deserialize_directory(depth);
            // if(ret == -1)
            //     return -1;

        }
        else if(S_ISREG(perm)){
            info("Serializing File");
            ret = deserialize_file(depth);
            if(ret == -1)
                return -1;
            ret = deserialize_directory(depth);
            if(ret == -1)
                return -1;
            }
            int ppo = path_pop();
            if(ppo == -1)
                return -1;
        else{
            return -1;
        }
    }
    else if(type == START_OF_DIRECTORY){

        ret = deserialize_directory(++depth);
        if(ret == -1)
            return -1;
    }
    else if(type == FILE_DATA){

        ret = deserialize_file(depth);
        if(ret == -1)
            return -1;
        // ret = deserialize_directory(depth);
        // if(ret == -1)
        //     return -1;
    }
    else if(type == END_OF_DIRECTORY){
        ret = deserialize_directory(depth);
        if(ret == -1)
            return -1;
        // ret = path_pop();
        // if(ret == -1)
        //     return -1;
    }
    else if(type == END_OF_TRANSMISSION){
        return 0;
    }

    return 0;
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
int deserialize_file(int depth){
    int ret = check_header_data();
    if(ret == -1)
        return -1;
    info("Checked header data");
    int type = get_type();
    debug("%d", type);
    int d = get_depth();
    if(depth != d)
        return -1;
    debug("%d", depth);
    long size = get_size();
    debug("%ld", size);

    // FILE *file;
    // if((file = fopen(path_buf, "r"))){
    //     if(!(global_options & 8)){
    //         fclose(file);
    //         return -1;
    //     }
    //     else
    //         fclose(file);
    // }

    FILE *f = fopen(path_buf, "w");
    if(f == NULL){
        info("File could not be created");
        return -1;
    }

    info("Opened file");
    int num_bytes = size - 16;
    debug("%d", num_bytes);
    int i = 0;
    while(i < num_bytes){
        fputc(getchar(), f);
        i++;
    }
    fclose(f);
    int pp = path_pop();
    if(pp == -1)
        return -1;
    return 0;
}

int check_header_data(){
    int depth = 0, ret = -1;
    long size = 0;
    int c = (int)getchar();
    // info("Current char: %d", c);
    int c2 = (int)getchar();
    // info("Current char: %d", c2);
    int c3 = (int)getchar();
    // info("Current char: %d", c3);
    if(c == MAGIC0 && c2 == MAGIC1 && c3 == MAGIC2){
        return 0;
    }
    return -1;
}

mode_t get_st_mode(){
    mode_t m = (((mode_t)getchar() << 24) |
                ((mode_t)getchar() << 16) |
                ((mode_t)getchar() << 8) |
                ((mode_t)getchar() << 0));
    // info("%d", m);
    return m;
}

off_t get_st_size(){
    off_t m = (((off_t)getchar() << 56) |
               ((off_t)getchar() << 48) |
               ((off_t)getchar() << 40) |
               ((off_t)getchar() << 32) |
               ((off_t)getchar() << 24) |
               ((off_t)getchar() << 16) |
               ((off_t)getchar() << 8) |
               ((off_t)getchar() << 0));
    // info("%ld", m);
    return m;
}

int get_depth(){
    int m = (((int)getchar() << 24) |
             ((int)getchar() << 16) |
             ((int)getchar() << 8) |
             ((int)getchar() << 0));
    // info("%d", m);
    return m;
}

long get_size(){
    long m = (((long)getchar() << 56) |
              ((long)getchar() << 48) |
              ((long)getchar() << 40) |
              ((long)getchar() << 32) |
              ((long)getchar() << 24) |
              ((long)getchar() << 16) |
              ((long)getchar() << 8) |
              ((long)getchar() << 0));
    // info("%ld", m);
    return m;
}

int get_type(){
    int m = (int)getchar();
    // info("%d", m);
    return m;
}

char * get_filename(int len){
    // info("len = %d", len);
    int i = 0;
    char *ptr = name_buf;
    while(i < len){
        *(ptr + i) = getchar();
        i++;
    }
//    for(int i = 0; i < stringLength(name_buf); i++){
////            debug("%04x ", *((char *) &x + i));
//        char bit = *((char *) &name_buf + i);
////                debug("%o ", bit);
//        putchar(bit);
//    }
    // info("%s", name_buf);
    // fprintf(stderr, "%s\n", name_buf);
    *(ptr + i) = '\0';
    return name_buf;
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
    char* dirValue = ".";
    for (i = 1; i < argc; i++){
        ptr = *argv;
        if(hFlag != 0){
            hFlag = compareStrings(ptr, "-h");
            if(hFlag == 0){
                global_options |= 0x1;
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

                    int argFound = checkIfArg(ptr);

                    if(argFound == -1){
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
    // if(global_options & 2)
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

int checkIfArg(char *arg){
    if(*arg == '-')
        return -1;
    else
        return 0;
}