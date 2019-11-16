/*
 * Job manager for "jobber".
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <wait.h>


#include "jobber.h"
#include "task.h"
#include "helper.h"

int jobs_init(void) {
    sf_set_readline_signal_hook(signal_hook_func);

    if(signal(SIGCHLD, handler) == SIG_ERR)
        abort();

    list_of_jobs = (struct job *)mmap(NULL, (sizeof list_of_jobs) * MAX_JOBS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    runners = mmap(NULL, (sizeof runners) * MAX_RUNNERS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for(int i = 0; i < MAX_JOBS; i++){
        list_of_jobs[i].status = -1;
        list_of_jobs[i].job_id = -1;
//        list_of_jobs[i].cmd = malloc(sizeof(char *));
        list_of_jobs[i].task = NULL;
        list_of_jobs[i].pgid = -1;
        list_of_jobs[i].pid = -1;
    }
    for(int i = 0; i < MAX_RUNNERS; i++){
        runners[i] = -1;
    }

    char* input;
//    sf_set_readline_signal_hook();
    while(1){
        input = sf_readline("jobber> ");
//        int l = 0;
//        while (input[l] != NULL){
            debug("%s", input);
//        }
        if(input == NULL){
            exit(EXIT_SUCCESS);
        }
        // debug("%s", input);
        if(parse(input) != 1){
//            debug("success");
//            printf("Unrecognized command: %s\n", input);
//            free(input);
        }
        free(input);
    }
//    jobs_fini();
    return 1;
}

void jobs_fini(void) {
    // TO BE IMPLEMENTED
    // stop all jobs and free all tasks
    for(int i = 0; i < MAX_JOBS; i++){
        list_of_jobs[i].status = -1;
        list_of_jobs[i].job_id = -1;
        free_task(list_of_jobs[i].task);
        free(list_of_jobs[i].cmd);
        list_of_jobs[i].cmd = NULL;
        list_of_jobs[i].task = NULL;
        list_of_jobs[i].pgid = -1;
        list_of_jobs[i].pid = -1;
    }
    exit(EXIT_SUCCESS);
}

int jobs_set_enabled(int val) {
    int prev = enabled;
    enabled = val;
    return prev;
}

int jobs_get_enabled() {
    return enabled;
}

int job_create(char *command) {
    if(command == NULL || strlen(command) == 0){
        // kill later
        exit(EXIT_SUCCESS);
    }
    int index = search_free_slot();
    if(index == -1){
        return -1;
    }
    struct job j;
    j.status = NEW;
    j.job_id = index;
    j.cmd = malloc(sizeof(char *) * strlen(command) + 1);
//    j.cmd = command
    strcpy(j.cmd, "");
    strcpy(j.cmd, command);
    j.pid = -1;
    j.pgid = 0;
    TASK *t = parse_task(&command);
    if(t == NULL){
        /* handle it */
        return -1;
    }
    j.task = t;
    sf_job_create(j.job_id);
//    debug("Job created");

    sf_job_status_change(j.job_id, j.status, WAITING);
    j.status = WAITING;
    list_of_jobs[index] = j;
    jobs_queued++;
    if(jobs_get_enabled() == 0){
        return 1;
    }
    else{
        run_procs();
        return 1;
    }

    // print_jobs_table();
    return 1;
}

int job_expunge(int jobid) {
    if(job_get_status(jobid) == ABORTED || job_get_status(jobid) == COMPLETED){
        list_of_jobs[jobid].status = -1;
        list_of_jobs[jobid].job_id = -1;
        free(list_of_jobs[jobid].cmd);
        list_of_jobs[jobid].cmd = NULL;
        free_task(list_of_jobs[jobid].task);
        list_of_jobs[jobid].task = NULL;
        list_of_jobs[jobid].pgid = -1;
        list_of_jobs[jobid].pid = -1;
        coalesce_job_table();
        jobs_queued--;
        sf_job_expunge(jobid);
        coalesce_job_table();
        return 0;
    }
    printf("Error: expunge\n");
    return -1;
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
    if(job_get_status(jobid) == RUNNING || job_get_status(jobid) == PAUSED || job_get_status(CANCELED)){
        return list_of_jobs[jobid].pgid;
    }
    return -1;
}

JOB_STATUS job_get_status(int jobid) {
    if(list_of_jobs[jobid].job_id != -1){
        return list_of_jobs[jobid].status;
    }
    return -1;
}

int job_get_result(int jobid) {
    if(job_get_status(jobid) == COMPLETED){
        int stat;
        if(waitpid(list_of_jobs[jobid].pid, &stat, WNOHANG) == -1)
            return -1;
        return stat;
    }
    return -1;
}

int job_was_canceled(int jobid) {
    // TO BE IMPLEMENTED
    abort();
}

char *job_get_taskspec(int jobid) {
    if(list_of_jobs[jobid].job_id != -1){
        return list_of_jobs[jobid].cmd;
    }
    return NULL;
}
