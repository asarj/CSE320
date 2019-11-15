#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "jobber.h"
#include "helper.h"

/*
 * "Jobber" job spooler.
 */

int main(int argc, char *argv[])
{
    // TO BE IMPLEMENTED
    jobs_init();
    char* input;
    while(1){
        input = sf_readline("jobber> ");
        // debug("%s", input);
        if(parse(input) != 1){
//            debug("success");
//            printf("Unrecognized command: %s\n", input);
//            free(input);
        }
        free(input);
    }
    jobs_fini();

    exit(EXIT_SUCCESS);
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
