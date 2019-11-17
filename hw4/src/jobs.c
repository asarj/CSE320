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
    if(signal(SIGINT, handler) == SIG_ERR)
        abort();
    if(signal(SIGABRT, handler) == SIG_ERR)
        abort();
    if(signal(SIGSEGV , handler) == SIG_ERR)
        abort();

    list_of_jobs = (struct job *)mmap(NULL, (sizeof list_of_jobs) * MAX_JOBS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    runners = (pid_t*)mmap(NULL, (sizeof runners) * MAX_RUNNERS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    jobs_queued = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    runners_queued = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    enabled = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *jobs_queued = 0;
    *runners_queued = 0;
    *enabled = 0;

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
    return 0;
}

void jobs_fini(void) {
    // stop all jobs and free all tasks
    for(int i = 0; i < MAX_JOBS; i++){
        job_cancel(list_of_jobs[i].job_id);
    }
    for(int i = 0; i < MAX_JOBS; i++){
        job_expunge(list_of_jobs[i].job_id);
    }

    exit(EXIT_SUCCESS);
}

int jobs_set_enabled(int val) {
    int prev = *enabled;
    *enabled = val;
    return prev;
}

int jobs_get_enabled() {
    return *enabled;
}

int job_create(char *command) {
    if(command == NULL || strlen(command) == 0){
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
    if(j.cmd == NULL)
        return -1;
//    j.cmd = command
    strcpy(j.cmd, "");
    strcpy(j.cmd, command);
    j.pid = -1;
    TASK *t = parse_task(&command);
    if(t == NULL){
        /* handle it */
        return -1;
    }
    j.task = t;
    sf_job_create(j.job_id);
    sf_job_status_change(j.job_id, j.status, WAITING);
    j.status = WAITING;
    list_of_jobs[index] = j;
    jobs_queued++;
    if(jobs_get_enabled()) {
        run_procs();
    }
    return j.job_id;
}

int job_expunge(int jobid) {
    if(job_get_status(jobid) == ABORTED || job_get_status(jobid) == COMPLETED){
        sf_job_expunge(list_of_jobs[jobid].job_id);
        list_of_jobs[jobid].status = -1;
        list_of_jobs[jobid].job_id = -1;
        free(list_of_jobs[jobid].cmd);
        list_of_jobs[jobid].cmd = NULL;
        free_task(list_of_jobs[jobid].task);
        list_of_jobs[jobid].task = NULL;
        list_of_jobs[jobid].pgid = -1;
        list_of_jobs[jobid].pid = -1;
//        coalesce_job_table();
        (*jobs_queued) -= 1;
//        sf_job_expunge(jobid);
        coalesce_job_table();
        return 0;
    }
    printf("Error: expunge\n");
    return -1;
}

int job_cancel(int jobid) {
    // TO BE IMPLEMENTED
    if(list_of_jobs[jobid].status == WAITING || list_of_jobs[jobid].status == RUNNING || list_of_jobs[jobid].status == PAUSED){

        sigset_t m, pr;
        sigfillset(&m);
        sigprocmask(SIG_SETMASK, &m, &pr);
        sf_job_status_change(jobid, list_of_jobs[jobid].status, CANCELED);
        list_of_jobs[jobid].status = CANCELED;
        sigprocmask(SIG_SETMASK, &pr, NULL);

        if(list_of_jobs[jobid].status != WAITING){
            killpg(list_of_jobs[jobid].pid, SIGKILL);

            sigset_t ma, pre;
            sigfillset(&m);
            sigprocmask(SIG_SETMASK, &ma, &pre);
            sf_job_end(jobid, list_of_jobs[jobid].pgid, SIGKILL);
            sf_job_status_change(jobid, list_of_jobs[jobid].status, ABORTED);
            list_of_jobs[jobid].status = ABORTED;


            for(int i = 0; i < MAX_RUNNERS; i++){
                if(runners[i] == list_of_jobs[jobid].pid){
                    *(runners_queued) -= 1;
                    runners[i] = -1;
                    coalesce_runner_table();
                    break;
                }
            }
            sigprocmask(SIG_SETMASK, &pre, NULL);
        }
        return 0;
    }
    return -1;
}

int job_pause(int jobid) {
    if(list_of_jobs[jobid].status == RUNNING){
        killpg(list_of_jobs[jobid].pid, SIGSTOP);

        sigset_t m, pr;
        sigfillset(&m);
        sigprocmask(SIG_SETMASK, &m, &pr);
        sf_job_status_change(jobid, list_of_jobs[jobid].status, PAUSED);
        list_of_jobs[jobid].status = PAUSED;
        sigprocmask(SIG_SETMASK, &pr, NULL);

        return 0;
    }
    return -1;
}

int job_resume(int jobid) {
    if(list_of_jobs[jobid].status == PAUSED){
        killpg(list_of_jobs[jobid].pid, SIGCONT);

        sigset_t m, pr;
        sigfillset(&m);
        sigprocmask(SIG_SETMASK, &m, &pr);
        sf_job_status_change(jobid, list_of_jobs[jobid].status, RUNNING);
        list_of_jobs[jobid].status = RUNNING;
        sigprocmask(SIG_SETMASK, &pr, NULL);
        run_procs_from_index(list_of_jobs[jobid].job_id);
        return 0;
    }
    return -1;
}

int job_get_pgid(int jobid) {
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
    if(list_of_jobs[jobid].job_id != -1 && list_of_jobs[jobid].status == ABORTED)
        return 1;
    return 0;
}

char *job_get_taskspec(int jobid) {
    if(list_of_jobs[jobid].job_id != -1){
        return list_of_jobs[jobid].cmd;
    }
    return NULL;
}
