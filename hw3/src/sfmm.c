/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"
#include "helper.h"

void *sf_malloc(size_t size) {
    if(size == 0)
        return NULL;
    if(!calledBefore){
        sf_mem_grow();
        // set new page
        // set free list
        calledBefore = 1;
    }
    else{
        // search free list
        // if enough ...
        // else {sf_mem_grow(), set new page, set free list}
    }
    return NULL;
}

void sf_free(void *pp) {
    return;
}

void *sf_realloc(void *pp, size_t rsize) {
    return NULL;
}
