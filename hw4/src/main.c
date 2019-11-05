#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "jobber.h"

/*
 * "Jobber" job spooler.
 */

int main(int argc, char *argv[])
{
    // TO BE IMPLEMENTED
    while(1){
        char* input = sf_readline("jobber> ");
        if(strcmp("quit", input) == 0){
            // TODO
            exit(EXIT_SUCCESS);
        }
        else if(strcmp("help", input) == 0){
            fprintf(stdout, "%s", "Available commands:\n" \
                    "help (0 args) Print this help message\n" \
                    "quit (0 args) Quit the program\n" \
                    "enable (0 args) Allow jobs to start\n" \
                    "disable (0 args) Prevent jobs from starting\n" \
                    "spool (1 args) Spool a new job\n" \
                    "pause (1 args) Pause a running job\n" \
                    "resume (1 args) Resume a paused job\n" \
                    "cancel (1 args) Cancel an unfinished job\n" \
                    "expunge (1 args) Expunge a finished job\n" \
                    "status (1 args) Print the status of a job\n" \
                    "jobs (0 args) Print the status of all jobs\n"
                );
        }

        debug("%s", input);
    }
    exit(EXIT_FAILURE);
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
