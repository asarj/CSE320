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
        if(check_misc_cmd(input) == 1){
            continue;
        }
        else if(check_info_cmd(input) == 1){
            continue;
        }
        else if(check_sys_cmd(input) == 1){
            continue;
        }
        else if(check_spool_cmd(input) == 1){
            continue;
        }


    }
    exit(EXIT_FAILURE);
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
