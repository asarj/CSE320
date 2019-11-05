#include <stdio.h>
#ifndef JOBBER_H
#define JOBBER_H
#endif

struct task{
    char* cmd;
    pid_t pid;
};

struct job{
    JOB_STATUS status;
    int job_number;
    struct task tsk;
};

