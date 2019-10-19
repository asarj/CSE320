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

sf_prologue *pptr;
sf_epilogue *eptr;

void *sf_malloc(size_t size) {
    // void *sf_ptr;
    if(size == 0)
        return NULL;
    if(!calledBefore){
        sf_mem_grow();
        void *start = sf_mem_start();
        start = start + 8; // allocate 64 bits
        pptr = start;
        pptr->padding1 = 0;
        pptr->header = 32 + 3;
        pptr->unused1 = NULL;
        pptr->unused2 = NULL;
        pptr->footer = pptr->header ^ sf_magic();

        void *end = sf_mem_end();
        end = end - 8;
        eptr = end;
        eptr -> header = 2;

        // sf_block *block_to_add = start + sizeof(pptr);
        // sf_show_blocks();
        sf_show_heap();
        // sf_show_heap();

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
