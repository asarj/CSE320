#include <stdio.h>
#include <stdlib.h>
#include "jobber.h"
#include "task.h"
#include <string.h>
#include <helper.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>

//int sf_suppress_chatter = 1;
int* enabled = 0;
int* jobs_queued = 0;
volatile sig_atomic_t flag = 0;
int* runners_queued = 0;

void handler(int sig){
//    sigset_t m, pr;
//    sigfillset(&m);
//    sigprocmask(SIG_SETMASK, &m, &pr);
    // insert stuff
//    sigprocmask(SIG_SETMASK, &pr, NULL);
    flag = sig;
    debug("%d", sig);
    if(sig == SIGINT){
        jobs_set_enabled(0);
        jobs_fini();
    }
    else{
        for(int i = 0; i < MAX_RUNNERS; i++){
            if(runners[i] == getpid()){
                debug("I found the runner of %d", getpid());
                for(int k = 0; k < MAX_JOBS; k++){
                    if(list_of_jobs[k].pid == getpid() && list_of_jobs[k].pgid == 1){
                        sigset_t m, pr;
                        sigfillset(&m);
                        sigprocmask(SIG_SETMASK, &m, &pr);
                        runners[i] = -1;
                        coalesce_runner_table();
                        (*runners_queued)--;
                        int status = 0;
                        sigprocmask(SIG_SETMASK, &pr, NULL);

                        waitpid(getpid(), &status, WNOHANG);

//                        sigset_t ma, pre;
                        sigfillset(&m);
                        sigprocmask(SIG_SETMASK, &m, &pr);
                        sf_job_end(k, getpid(), status);
                        int stat_enum = COMPLETED;
                        if(status < 0){
                            stat_enum = ABORTED;
                        }
                        sf_job_status_change(k, list_of_jobs[k].status, stat_enum);
                        list_of_jobs[k].status = stat_enum;
                        sigprocmask(SIG_SETMASK, &pr, NULL);
                        break;
                    }
                }

                break;
            }
        }
    }

}

int signal_hook_func(void){
//    if(flag){
//        debug("I got caught in signal hook func %d", flag);
////        flag = 0;
//
//    }
//    flag = 0;
    return 0;
}

int run_procs(){

    for(int i = 0; i < MAX_JOBS; i++){
        if(list_of_jobs[i].task == NULL || list_of_jobs[i].status == COMPLETED || list_of_jobs[i].status == ABORTED || list_of_jobs[i].status == PAUSED || list_of_jobs[i].status == CANCELED)
            continue;
//            sigset_t m, pr;
//            sigfillset(&m);
//            sigprocmask(SIG_SETMASK, &m, &pr);
        pid_t start_fork = fork();
        if(start_fork == 0){
            if(*runners_queued >= MAX_RUNNERS){
                sleep(10);
            }
            int ret = search_runner_slot();
            if(ret == -1)
                return -1;
            list_of_jobs[i].pid = getpid();
            setpgid(getpid(), getpid());

            sigset_t masks, prevs;
            sigfillset(&masks);
            sigprocmask(SIG_SETMASK, &masks, &prevs);
            sf_job_start(list_of_jobs[i].job_id, list_of_jobs[i].pgid);
            sf_job_status_change(list_of_jobs[i].job_id, list_of_jobs[i].status, RUNNING);
            list_of_jobs[i].status = RUNNING;
            runners[ret] = getpid();
            (*runners_queued)++;
            j = &list_of_jobs[i];
            sigprocmask(SIG_SETMASK, &prevs, NULL);

            PIPELINE_LIST *plist = (list_of_jobs[i].task)->pipelines;
            char* input_path = malloc(sizeof(char*));
            char* output_path = malloc(sizeof(char*));
            pid_t pid;

            while(plist->first != NULL){
                int pipe_arr[2];
                pipe(pipe_arr);
                int pipe_int = 0;
                PIPELINE *p = plist->first;
                int f_out = - 1;
                int f_in = -1;

                strcpy(input_path, "");
                strcpy(output_path, "");

                if(p->input_path != NULL){
                    strcpy(input_path, p->input_path);
//                    debug("%s", p->input_path);
                    f_in = open(p->input_path, O_WRONLY | O_TRUNC | O_CREAT, 0777);
                }
                if(p->output_path != NULL){
                    strcpy(output_path, p->output_path);
//                    debug("%s", p->output_path);
                    f_out = open(p->output_path, O_WRONLY | O_TRUNC | O_CREAT, 0777);
                }

                COMMAND_LIST *cmds = p->commands;

                char **cmd = NULL;// malloc(sizeof(char *));
//                int index  = 0;
                while(cmds->first != NULL){
                    WORD_LIST *wlist = cmds->first->words;
                    int index  = 0;
                    cmd = NULL;
                    while(wlist->first != NULL){
                        char *temp = wlist->first;
//                        debug("%s", temp);
                        cmd = (char**)realloc(cmd, (index + 1) * sizeof(char*));
                        cmd[index] = temp;
                        index++;
                        if(wlist->rest == NULL) {
                            break;
                        }
                        wlist = wlist->rest;
                    }
//                    debug("%s", cmd[0]);
                    cmd = realloc(cmd, (index + 1) * sizeof(char*));
                    cmd[index] = NULL;


                    if(cmd != NULL){
                        int filedesc[2];
                        if(pipe(filedesc) == -1){
                            exit(1);
                        }

                        pid = fork();
                        if(pid < 0){
                            abort();
                        }
                        else if(pid == 0){
                            // Entered child
//                            debug("%d", getpid());
                            if(cmds->rest == NULL){
                                if(f_out != -1){
                                    dup2(f_out, STDOUT_FILENO);

                                }
                            }
                            else{
                                dup2(filedesc[1], STDOUT_FILENO);
                            }
                            if(pipe_int){
                                dup2(pipe_arr[0], STDIN_FILENO);
                            }
                            else if(f_in != -1){
                                dup2(f_in, STDIN_FILENO);
                            }
                            if(f_in != -1)
                                close(f_in);
                            if(f_out != -1)
                                close(f_out);

                            close(filedesc[1]);
                            close(filedesc[0]);
                            close(pipe_arr[1]);
                            close(pipe_arr[0]);


                            execvp(cmd[0], cmd);
                        }
                        else{
                            // Entered parent
                            setpgid(pid, getpid());
//                            debug("%d", pid);
                            pipe_int = 1;
                            close(pipe_arr[1]);
                            close(pipe_arr[0]);
                            pipe_arr[0] = filedesc[0];
                            pipe_arr[1] = filedesc[1];

                        }
                        free(cmd);
                        if(cmds->rest == NULL)
                            break;
                        cmds = cmds->rest;
                    }
                }

                if(plist->rest == NULL){
                    free(input_path);
                    free(output_path);
                    break;
                }

                plist = plist->rest;

            }
            int status;
            list_of_jobs[i].pgid = 1;
            waitpid(pid, &status, 0);
            pid_t group_pid = getpgid(getpid());
            pid_t pid_exit = fork();

            if(pid_exit == 0){
                setpgid(getpid(), group_pid);
                exit(status);
            }
            waitpid(pid_exit, NULL, 0);
            exit(EXIT_SUCCESS);
        }

    }
    return 1;
}

int run_procs_from_index(int jobid){
    for(int i = jobid; i < MAX_JOBS; i++){
        if(list_of_jobs[i].task == NULL || list_of_jobs[i].status == COMPLETED || list_of_jobs[i].status == ABORTED || list_of_jobs[i].status == PAUSED || list_of_jobs[i].status == CANCELED)
            continue;
        pid_t start_fork = fork();
        if(start_fork == 0){
            if(*runners_queued >= MAX_RUNNERS){
                sleep(10);
            }
            int ret = search_runner_slot();
            if(ret == -1)
                return -1;
            list_of_jobs[i].pid = getpid();
            setpgid(getpid(), getpid());

            sigset_t masks, prevs;
            sigfillset(&masks);
            sigprocmask(SIG_SETMASK, &masks, &prevs);
            sf_job_start(list_of_jobs[i].job_id, list_of_jobs[i].pgid);
            sf_job_status_change(list_of_jobs[i].job_id, list_of_jobs[i].status, RUNNING);
            list_of_jobs[i].status = RUNNING;
            runners[ret] = getpid();
            (*runners_queued)++;
            sigprocmask(SIG_SETMASK, &prevs, NULL);

            j = &list_of_jobs[i];
            PIPELINE_LIST *plist = (list_of_jobs[i].task)->pipelines;
            char* input_path = malloc(sizeof(char*));
            char* output_path = malloc(sizeof(char*));
            pid_t pid;

            while(plist->first != NULL){
                int pipe_arr[2];
                pipe(pipe_arr);
                int pipe_int = 0;
                PIPELINE *p = plist->first;
                int f_out = - 1;
                int f_in = -1;

                strcpy(input_path, "");
                strcpy(output_path, "");

                if(p->input_path != NULL){
                    strcpy(input_path, p->input_path);
//                    debug("%s", p->input_path);
                    f_in = open(p->input_path, O_WRONLY | O_TRUNC | O_CREAT, 0777);
                }
                if(p->output_path != NULL){
                    strcpy(output_path, p->output_path);
//                    debug("%s", p->output_path);
                    f_out = open(p->output_path, O_WRONLY | O_TRUNC | O_CREAT, 0777);
                }

                COMMAND_LIST *cmds = p->commands;

                char **cmd = NULL;// malloc(sizeof(char *));
//                int index  = 0;
                while(cmds->first != NULL){
                    WORD_LIST *wlist = cmds->first->words;
                    int index  = 0;
                    cmd = NULL;
                    while(wlist->first != NULL){
                        char *temp = wlist->first;
                        debug("%s", temp);
                        cmd = (char**)realloc(cmd, (index + 1) * sizeof(char*));
                        cmd[index] = temp;
                        index++;
                        if(wlist->rest == NULL) {
                            break;
                        }
                        wlist = wlist->rest;
                    }
//                    debug("%s", cmd[0]);
                    cmd = realloc(cmd, (index + 1) * sizeof(char*));
                    cmd[index] = NULL;


                    if(cmd != NULL){
                        int filedesc[2];
                        if(pipe(filedesc) == -1){
                            exit(1);
                        }

                        pid = fork();
                        if(pid < 0){
                            abort();
                        }
                        else if(pid == 0){
                            // Entered child
//                            debug("%d", getpid());
                            if(cmds->rest == NULL){
                                if(f_out != -1){
                                    dup2(f_out, STDOUT_FILENO);

                                }
                            }
                            else{
                                dup2(filedesc[1], STDOUT_FILENO);
                            }
                            if(pipe_int){
                                dup2(pipe_arr[0], STDIN_FILENO);
                            }
                            else if(f_in != -1){
                                dup2(f_in, STDIN_FILENO);
                            }
                            if(f_in != -1)
                                close(f_in);
                            if(f_out != -1)
                                close(f_out);

                            close(filedesc[1]);
                            close(filedesc[0]);
                            close(pipe_arr[1]);
                            close(pipe_arr[0]);


                            execvp(cmd[0], cmd);
                        }
                        else{
                            // Entered parent
                            setpgid(pid, getpid());
//                            debug("%d", pid);
                            pipe_int = 1;
                            close(pipe_arr[1]);
                            close(pipe_arr[0]);
                            pipe_arr[0] = filedesc[0];
                            pipe_arr[1] = filedesc[1];

                        }
                        free(cmd);
                        if(cmds->rest == NULL)
                            break;
                        cmds = cmds->rest;
                    }
                }

                if(plist->rest == NULL){
                    free(input_path);
                    free(output_path);
                    break;
                }

                plist = plist->rest;

            }
            int status;
            list_of_jobs[i].pgid = 1;
            waitpid(pid, &status, 0);
            pid_t group_pid = getpgid(getpid());
            pid_t pid_exit = fork();

            if(pid_exit == 0){
                setpgid(getpid(), group_pid);
                exit(status);
            }
            waitpid(pid_exit, NULL, 0);
            exit(EXIT_SUCCESS);
        }

    }
    return 1;
}

const char* map_status_to_str(JOB_STATUS x){
    switch(x){
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

void coalesce_job_table(){
    int i = 0;
    int k = 0;
    while(k < *jobs_queued){
        if(list_of_jobs[i].job_id != -1){
            if(i < k){
                struct job temp = list_of_jobs[i];
                list_of_jobs[i] = list_of_jobs[k];
                list_of_jobs[k] = temp;
            }
            i++;
        }
        k++;
    }
}

void coalesce_runner_table(){
    int i = 0;
    int k = 0;
    while(k < *runners_queued){
        if(runners[i] != -1){
            if(i < k){
                pid_t temp = runners[i];
                runners[i] = runners[k];
                runners[k] = temp;
            }
            i++;
        }
        k++;
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

int search_runner_slot(){
    for(int i = 0; i < MAX_RUNNERS; i++){
        if(runners[i] == -1)
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
    strcpy(newInput, "");
    int i = begin;
    int k = 0;
    while(input[i] != end){
        // debug("substring: %c", input[i]);
        // debug("substring: %c", newInput[k]);
        newInput[k++] = input[i++];
    }

    newInput[k] = '\0';
//    debug("substring: %s", newInput);
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
    int mul_args = 0;
    if(strcmp("", input) == 0){
        return 1;
    }
    if(strcmp("quit", input) == 0){
        // TODO expunge table
//        for(int i = 0; i < MAX_JOBS; i++){
//            if(list_of_jobs[i].job_id != -1){
//                sf_job_status_change(list_of_jobs[i].job_id, list_of_jobs[i].status, ABORTED);
//                list_of_jobs[i].status = ABORTED;
//
////                kill(list_of_jobs[i].pid, SIGKILL);
//                job_expunge(list_of_jobs[i].job_id);
//            }
//
//        }
        jobs_fini();
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

    if(strcmp("enable", input) == 0){
//        debug("I am being enabled");
        int res = jobs_get_enabled();
//        debug("Enabled flag: %d, res");
        if(res == 0){
            jobs_set_enabled(1);
            run_procs();
        }
        return 1;
    }
    else if(strcmp("disable", input) == 0){
        int res = jobs_get_enabled();
        if(res != 0){
            jobs_set_enabled(0);
        }
        return 1;
    }
    else if(strcmp("jobs", input) == 0){
        // debug("%s", pipe);
        for(int i = 0; i < MAX_JOBS; i++){
            if(list_of_jobs[i].job_id != -1)
                printf("job %d [%s]:%s\n", list_of_jobs[i].job_id, map_status_to_str(list_of_jobs[i].status), list_of_jobs[i].cmd);
        }
        return 1;
    }
    mul_args = 1;
    /*It's not a one word command, so we will need to break it up*/


    // debug("%s", first);
    if(mul_args == 1){
        char *first = substring(input, 0, ' ');
//        debug("%s", first);
        if(strcmp("status", first) == 0){
            int str_start = strlen(first) + 1;
            int second = (*substring(input, str_start, '\0') - '0');
            if(list_of_jobs[second].job_id != -1){
                printf("job %d [%s]:%s\n", list_of_jobs[second].job_id, map_status_to_str(list_of_jobs[second].status), list_of_jobs->cmd);
            }
//            debug("%d", second);
            free(first);
            return 1;
        }
        else if(strcmp("spool", first) == 0){
            char *pipe = replace_char_with_no_space(strstr(input, "\'"), '\'');
//            debug("%s", pipe);

            int res = job_create(pipe);
            if(res == -1){
                // handle it
            }
            strcpy(pipe, "");

            free(first);
            return 1;
        }
        else if(strcmp("pause", first) == 0){
            int str_start = strlen(first) + 1;
            int second = (*substring(input, str_start, '\0') - '0');
            if(list_of_jobs[second].job_id != -1){
                job_pause(list_of_jobs[second].job_id);
            }
            else{
                printf("Unrecognized command: pause\n");
            }
            free(first);
            return 1;
        }
        else if(strcmp("resume", first) == 0){
            int str_start = strlen(first) + 1;
            int second = (*substring(input, str_start, '\0') - '0');
            if(list_of_jobs[second].job_id != -1){
                job_resume(list_of_jobs[second].job_id);
            }
            else{
                printf("Unrecognized command: resume\n");
            }
            free(first);
            return 1;
        }
        else if(strcmp("cancel", first) == 0){
            int str_start = strlen(first) + 1;
            int second = (*substring(input, str_start, '\0') - '0');
            if(list_of_jobs[second].job_id != -1){
                job_cancel(list_of_jobs[second].job_id);
            }
            else{
                printf("Unrecognized command: cancel\n");
            }
            free(first);
            return 1;
        }
        else if(strcmp("expunge", first) == 0){
            int str_start = strlen(first) + 1;
            int second = (*substring(input, str_start, '\0') - '0');
//            debug("%d", second);
            if(list_of_jobs[second].job_id != -1){
                job_expunge(second);
            }
            free(first);
            return 1;
        }
        else{
            printf("Unrecognized command: %s\n", input);
        }
        free(first);
    }
    return 1;
}
