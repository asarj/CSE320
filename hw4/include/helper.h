#ifndef HELPER_H
#define HELPER_H

extern int enabled;
extern int jobs_queued;

struct job{
    JOB_STATUS status;
    char *cmd;
    struct TASK *task;
    int job_id;
    pid_t pgid;
    pid_t pid; /* if running */
};

struct job list_of_jobs[MAX_JOBS];
int runners[MAX_RUNNERS];

int parse(char *input);
char* replace_char_with_no_space(char *input, char c);
char* substring(const char *input, int begin, char end);
int search_free_slot();
void coalesce_job_table();
struct PIPELINE *p;
struct job *j;
char* get_pipeline_command(struct PIPELINE *p);
int run_procs();
void sig_child_handler(int sig);
void sig_segv_handler(int sig);
void sig_abrt_handler(int sig);

/* Debugging functions */
void print_jobs_table();
const char* map_status_to_str(JOB_STATUS j);

#endif