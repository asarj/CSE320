/*
 * Job manager for "jobber".
 */

#include <stdlib.h>
#include <string.h>

#include "jobber.h"
#include "task.h"
#include "helper.h"

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
    sf_job_create(j.job_id);
//    debug("Job created");
//    sf_job_status_change(j.job_id, NEW, NEW);

    PIPELINE_LIST *plist = (t)->pipelines;
    while(plist->first != NULL){
        PIPELINE *p = plist->first;
        debug("Start of pipeline");
        char* input_path = malloc(sizeof(char*));
        char* output_path = malloc(sizeof(char*));
        strcpy(input_path, "");
        strcpy(output_path, "");

        if(p->input_path != NULL){
//            strcpy(input_path, p->input_path);
            debug("%s", p->input_path);
        }
        if(p->output_path != NULL){
//            strcpy(output_path, p->output_path);
            debug("%s", p->output_path);
        }

        COMMAND_LIST *cmds = p->commands;

        while(cmds->first != NULL){
            debug("Start of command");
            WORD_LIST *wlist = cmds->first->words;
            while(wlist->first != NULL){
                char *temp = wlist->first;
//                strcat(cmd, temp);
                    debug("%s", temp);
                if(wlist->rest == NULL)
                    break;
//                strcat(cmd, " ");
                wlist = wlist->rest;
            }
            if(cmds->rest == NULL)
                break;
            cmds = cmds->rest;
        }
        if(plist->rest == NULL){
            free(input_path);
            free(output_path);
            break;
        }

        plist = plist->rest;

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
