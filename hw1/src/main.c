#include <stdio.h>
#include <stdlib.h>

#include "const.h"
#include "debug.h"
#include "helper.h" // Delete after

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

// Original main
int main(int argc, char **argv)
{
    int ret;
    if(validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);
    debug("Options: 0x%x", global_options);
    if(global_options & 1)
        USAGE(*argv, EXIT_SUCCESS);

    return EXIT_SUCCESS;
}

// int main(int argc, char **argv)
// {
//     int ret;
//     printf("Global Options: %d\n", global_options);
//     printf("argc: %d\n", argc);
//     ret = -1;
//     if(argc < 2){
//         ret = -1;
//     }

//     char *ptr;
//     int hFound, sFound, dFound;

//     ptr = *argv;
//     argv++;
//     ptr = *argv;

//     hFound = compareStrings(ptr, "-h");
//     printf("hFound: %d\n", hFound);
//     if(hFound == 0){
//         global_options |= 0x1;
//         ret = 0;
//     }

//     sFound =  compareStrings(ptr, "-s");
//     printf("sFound: %d\n", sFound);
//     if(sFound == 0){
//         global_options |= 0x2;
//         ret = 0;
//     }

//     // ret = compareStrings("-s", "-s");
//     printf("Validargs return: %d\n", ret);
//     printf("Modified Global Options: %d\n", global_options);
//     if(validargs(argc, argv))
//         USAGE(*argv, EXIT_FAILURE);
//     debug("Options: 0x%x", global_options);
//     if(global_options & 1)
//         USAGE(*argv, EXIT_SUCCESS);

//     return EXIT_SUCCESS;
// }

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
