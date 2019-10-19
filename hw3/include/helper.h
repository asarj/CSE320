#include <stdio.h>
#ifndef HELPER_H
#define HELPER_H
#endif
#ifndef SFMM_H
#define SFMM_H
#endif

int calledBefore = 0;

int write_prologue(sf_prologue *ptr);
int write_epilogue(sf_epilogue *ptr);
int add_block(size_t size);