/*
 * Job manager for "jobber".
 */

#include <stdlib.h>
#include <string.h>

#include "jobber.h"
#include "task.h"
#include "helper.h"

char* replace_char_with_no_space(char *input, char c){
    char *newInput = malloc(sizeof(input));
    if(input == NULL || strlen(input) == 0)
        return NULL;
    int i = 0;
    int j = 0;
    while(input[i] != '\0'){
        if(input[i] == c){
            i++;
        }
        else{
            newInput[j++] = input[i++];
        }
    }
    newInput[j] = '\0';
    return newInput;
}

char* substring(char *input, int begin, char end){
    char *newInput = malloc(sizeof(input));
    int i = begin;
    int j = 0;
    while(input[i] != end){
        newInput[j++] = input[i++];
    }
    newInput[j] = '\0';
    return newInput;
}

int parse(char *input){
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
    else if(strcmp("enable", input) == 0){

        return 1;
    }
    else if(strcmp("disable", input) == 0){

        return 1;
    }
    /*It's not a one word command, so we will need to break it up*/
    char *first = substring(input, 0, ' ');
    char *pipe = replace_char_with_no_space(strstr(input, "\'"), '\'');
    // debug("%s", pipe);

    // debug("%s", first);

    if(strcmp("status", first) == 0){
        // debug("%s", pipe);
        return 1;
    }
    else if(strcmp("jobs", first) == 0){
        // debug("%s", pipe);
        return 1;
    }
    else if(strcmp("spool", first) == 0){
        // debug("%s", pipe);
        TASK *t = parse_task(&pipe);
        if(t == NULL){
            /* handle it */
        }

        return 1;
    }
    else if(strcmp("pause", first) == 0){
        // debug("%s", pipe);
        return 1;
    }
    else if(strcmp("resume", first) == 0){
        // debug("%s", pipe);
        return 1;
    }
    else if(strcmp("cancel", first) == 0){
        // debug("%s", pipe);
        return 1;
    }
    else if(strcmp("expunge", first) == 0){
        // debug("%s", pipe);
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
