#include <stdio.h>
#ifndef HELPER_H
#define HELPER_H
#endif
#ifndef SFMM_H
#define SFMM_H
#endif

int calledBefore = 0;

int write_prologue();
int write_epilogue();
int add_first_block();
int init_free_lists();