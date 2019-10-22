#include <stdio.h>
#ifndef HELPER_H
#define HELPER_H
#endif
#ifndef SFMM_H
#define SFMM_H
#endif

#define M 32
int calledBefore = 0;

int write_prologue();
int write_epilogue();
int add_first_block();
int add_first_block_to_free_list(void *mem_grow_first_call/*, size_t size*/);
int init_free_lists();