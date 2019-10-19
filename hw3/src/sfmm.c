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

int write_prologue(sf_prologue *ptr){
    void *start = sf_mem_start();
    if(start == (void *)-1 || start == NULL)
        return 0;
    // start = start + 8; // allocate 64 bits
    ptr = start;
    ptr->padding1 = 0;
    ptr->header = 32 + 3;
    ptr->unused1 = NULL;
    ptr->unused2 = NULL;
    ptr->footer = ptr->header ^ sf_magic();

    return 1;
}

int add_block(size_t size){
    return 111;
}

int write_epilogue(sf_epilogue *ptr){
    void *end = sf_mem_end();
    if(end == (void *)-1 || end == NULL)
        return 0;
    end = end - 9;
    ptr = end;
    ptr = 0;

    return 1;
}

void *sf_malloc(size_t size) {
    // void *sf_ptr;
    // sf_header head;
    // sf_footer foot;
    debug("%ld", size);
    if(size == 0)
        return NULL;
    if(!calledBefore){

        sf_mem_init();
        sf_mem_grow();

        if(write_prologue(pptr) == 0){
            // handle case
            return NULL;
        }
        // if(add_block(size) == 0){
        //     // handle case
        //     return NULL;
        // }
        if(write_epilogue(eptr) == 0){
            // handle case
            return NULL;
        }

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
