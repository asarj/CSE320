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
    while(1){
        char* input = sf_readline("jobber> ");
        debug("%s", input);
        if(parse(input) == 1){
            debug("success");
        }


    }
    exit(EXIT_FAILURE);
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
