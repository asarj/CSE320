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

    PIPELINE_LIST plist = *(t)->pipelines;
    while(plist.first != NULL){
        char *pipe = get_pipeline_command(plist.first);
        j.num_tasks += 1;

        if(j.num_tasks > MAX_RUNNERS){
            // handle it
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
