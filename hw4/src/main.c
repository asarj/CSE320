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
    sf_set_readline_signal_hook(signal_hook_func);
    jobs_init();


    exit(EXIT_SUCCESS);
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
