#include "jobber.h"

#ifndef JOBBER_H
#define JOBBER_H
#endif
#ifndef TASK_H
#define TASK_H
#endif

int enabled;

struct job{
    JOB_STATUS status;
    struct TASK *task;
    int job_id;
    int num_tasks;
    pid_t pid; /* if running */
};

struct job list_of_jobs[MAX_JOBS];

int parse(char *input);
char* replace_char_with_no_space(char *input, char c);
char* substring(const char *input, int begin, char end);
int search_free_slot();
char* get_pipeline_command(struct PIPELINE *p);

/* Debugging functions */
void print_jobs_table();
const char* map_status_to_str(JOB_STATUS j);