#ifndef HELPER_H
#define HELPER_H

extern int* enabled;
extern int* jobs_queued;
extern int* runners_queued;
volatile __sig_atomic_t flag;

struct job{
    JOB_STATUS status;
    char *cmd;
    struct TASK *task;
    int job_id;
    pid_t pgid;
    pid_t pid; /* if running */
} *job;

struct job *list_of_jobs;
pid_t *runners;

int parse(char *input);
char* replace_char_with_no_space(char *input, char c);
char* substring(const char *input, int begin, char end);
int search_free_slot();
int search_runner_slot();
void coalesce_job_table();
void coalesce_runner_table();
struct PIPELINE *p;
struct job *j;
char* get_pipeline_command(struct PIPELINE *p);
int run_procs();
int run_procs_from_index(int jobid);
void handler(int sig);
int signal_hook_func(void);

/* Debugging functions */
void print_jobs_table();
const char* map_status_to_str(JOB_STATUS j);

#endif