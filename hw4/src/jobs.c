/*
 * Job manager for "jobber".
 */

#include <stdlib.h>
#include <string.h>

#include "jobber.h"
#include "task.h"
#include "helper.h"

int check_misc_cmd(char *input){
    if(strcmp("quit", input) == 0){
        // TODO expunge table
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
        return 1;
    }
    return 0;
}

int check_info_cmd(char *input){
    if(strcmp("status", input) == 0){
        // TODO
        return 1;
    }
    else if(strcmp("jobs", input) == 0){
        // TODO
        return 1;
    }
    return 0;
}
int check_sys_cmd(char *input){
    if(strcmp("enable", input) == 0){
        // TODO
        return 1;
    }
    else if(strcmp("disable", input) == 0){
        // TODO
        return 1;
    }
    return 0;
}
int check_spool_cmd(char *input){
    if(strcmp("spool", input) == 0){
        // TODO
        return 1;
    }
    else if(strcmp("pause", input) == 0){
        // TODO
        return 1;
    }
    else if(strcmp("resume", input) == 0){
        // TODO
        return 1;
    }
    else if(strcmp("cancel", input) == 0){
        // TODO
        return 1;
    }
    else if(strcmp("expunge", input) == 0){
        // TODO
        return 1;
    }
    return 0;
}

int jobs_init(void) {
    // TO BE IMPLEMENTED
    abort();
}

void jobs_fini(void) {
    // TO BE IMPLEMENTED
    abort();
}

int jobs_set_enabled(int val) {
    // TO BE IMPLEMENTED
    abort();
}

int jobs_get_enabled() {
    // TO BE IMPLEMENTED
    abort();
}

int job_create(char *command) {
    // TO BE IMPLEMENTED
    abort();
}

int job_expunge(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_cancel(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_pause(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_resume(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_get_pgid(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

JOB_STATUS job_get_status(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_get_result(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

int job_was_canceled(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

char *job_get_taskspec(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}
