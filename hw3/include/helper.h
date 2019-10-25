#include <stdio.h>
#ifndef HELPER_H
#define HELPER_H

#define M 32
int calledBefore = 0;
int usedSpace = 0;
int write_prologue();
int write_epilogue();
int add_first_block(size_t size);
int add_first_block_to_free_list(void *mem_grow_first_call/*, size_t size*/);
int init_free_lists();
void* search_free_lists(size_t size);
void add_to_free_list(int size, void* add);
int roundTo16(int num, int mul);
int getIndexFromSize(int size);
void coalesce(void *pp, sf_footer foot, sf_header head, int flag);

#endif
#ifndef SFMM_H
#define SFMM_H
#endif

