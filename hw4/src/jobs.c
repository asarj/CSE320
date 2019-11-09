/*
 * Job manager for "jobber".
 */

#include <stdlib.h>
#include <string.h>

#include "jobber.h"
#include "task.h"
#include "helper.h"

int sf_suppress_chatter = 1;
int enabled = 0; // disabled to begin;
const char* map_status_to_str(JOB_STATUS j){
    switch(j){
        case NEW: return "NEW"; break;
        case WAITING: return "WAITING"; break;
        case RUNNING: return "RUNNING"; break;
        case PAUSED: return "PAUSED"; break;
        case CANCELED: return "CANCELLED"; break;
        case COMPLETED: return "COMPLETED"; break;
        case ABORTED: return "ABORTED"; break;
        default: return "INVALID"; break;
    }
}

void print_jobs_table(){
    for(int i = 0; i < MAX_JOBS; i++){
        debug("JOB INDEX: %d", i);
        debug("Status: %s", map_status_to_str(list_of_jobs[i].status));
        debug("Job ID: %d", list_of_jobs[i].job_id);
        debug("pid: %d", list_of_jobs[i].pid);
    }
}

int search_free_slot(){
    for(int i = 0; i < MAX_JOBS; i++){
        if(list_of_jobs[i].status == -1)
            return i;
    }
    return -1;
}

char* replace_char_with_no_space(char *input, char c){
    char *current_pos = strchr(input, c);
    while (current_pos){
        *current_pos = ' ';
        current_pos = strchr(current_pos, c);
    }
    return input;
}

char* substring(const char *input, int begin, char end){
    char *newInput = malloc(sizeof(char *));
    int i = begin;
    int j = 0;
    while(input[i] != end){
        // debug("substring: %c", input[i]);
        // debug("substring: %c", newInput[j]);
        newInput[j++] = input[i++];
    }

    newInput[j] = '\0';
    debug("substring: %s", newInput);
    return newInput;
}

char* get_pipeline_command(struct PIPELINE *p){
    char* input_path = malloc(sizeof(char*));
    char* output_path = malloc(sizeof(char*));
    strcpy(input_path, "");
    strcpy(output_path, "");
    char* cmd = malloc(sizeof(char*));

    while(p->commands->first != NULL){
        if(p->input_path != NULL){
            strcpy(input_path, p->input_path);
        }
        if(p->output_path != NULL){
            strcpy(output_path, p->output_path);
        }
        WORD_LIST *wlist = p->commands->first->words;
        while(wlist->first != NULL){
            char *temp = wlist->first;
            strcat(cmd, temp);
            if(wlist->rest == NULL)
                break;
            strcat(cmd, " ");
            wlist = wlist->rest;
        }
        if(p->commands->rest == NULL)
            break;
        strcat(cmd, " | ");
        p->commands = p->commands->rest;
    }
    if(input_path != NULL && *input_path != '\0'){
        strcat(cmd, " < ");
        strcat(cmd, input_path);
    }
    if(output_path != NULL  && *output_path != '\0'){
        strcat(cmd, " > ");
        strcat(cmd, output_path);
    }
//    free(input_path);
//    free(output_path);
    return cmd;
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

    jobs_init();

    if(strcmp("enable", input) == 0){

        return 1;
    }
    else if(strcmp("disable", input) == 0){

        return 1;
    }
    /*It's not a one word command, so we will need to break it up*/
    char *first = substring(input, 0, ' ');
    char *pipe = replace_char_with_no_space(strstr(input, "\'"), '\'');
    debug("%s", pipe);

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
        debug("%s", pipe);

        int res = job_create(pipe);
        if(res == -1){
            // handle it
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
    for(int i = 0; i < MAX_JOBS; i++){
        list_of_jobs[i].status = -1;
        list_of_jobs[i].job_id = -1;
        list_of_jobs[i].task = NULL;
        list_of_jobs[i].num_tasks = -1;
        list_of_jobs[i].pid = -1;
    }
    return 1;
}

void jobs_fini(void) {
    // TO BE IMPLEMENTED
    // stop all jobs and free all tasks
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
    TASK *t = parse_task(&command);
    if(t == NULL){
        /* handle it */
    }
    int index = search_free_slot();
    if(index == -1){
        // handle it
    }

    struct job j;
    j.status = NEW;
    j.job_id = index;
    j.task = t;
    j.pid = -1;
    j.num_tasks = 0;
    list_of_jobs[j.job_id] = j;

    PIPELINE_LIST plist = *(t)->pipelines;
    while(plist.first != NULL){
        char *pipe = get_pipeline_command(plist.first);
        debug("%s", pipe);
        j.num_tasks += 1;
        if(pipe == NULL){

        }
        if(plist.rest == NULL)
            break;
        plist = *plist.rest;
    }

    // print_jobs_table();
    return 1;
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
