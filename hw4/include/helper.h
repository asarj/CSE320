#include <stdio.h>
#ifndef JOBBER_H
#define JOBBER_H
#endif
#ifndef TASK_H
#define TASK_H
#endif

struct job{
    JOB_STATUS status;
    int job_number;
    // struct TASK t;
};

int check_misc_cmd(char *input);
int check_info_cmd(char *input);
int check_sys_cmd(char *input);
int check_spool_cmd(char *input);