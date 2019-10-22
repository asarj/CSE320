/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "sfmm.h"
#include "helper.h"

// Pointers in memory
sf_prologue pptr;
sf_prologue *sfp;

sf_epilogue eptr;
sf_epilogue *efp;

sf_block blk;
sf_block *blk_ptr;

sf_footer ftr;
sf_footer *ftr_ptr;

void *mem_grow;

int write_prologue(){
    sfp = (sf_prologue *)sf_mem_start();
    if(sfp == (void *)-1 || sfp == NULL)
        return 0;
    // start = start + 8; // allocate 64 bits
    (pptr).padding1 = 0;
    (pptr).header = 32 + 3;
    (pptr).footer = (pptr).header ^ sf_magic();
    *sfp = pptr;
    sfp = sfp + 1;
    return 1;
}

int write_epilogue(){
    efp = (sf_epilogue *)sf_mem_end();
    if(efp == (void *)-1 || efp == NULL)
        return 0;
    efp = efp - 1;
    eptr.header = 2;
    *efp = eptr;

    ftr_ptr = (sf_footer *) efp;
    ftr_ptr = ftr_ptr - 1;
    ftr = blk.header ^ sf_magic();
    *ftr_ptr = ftr;
    return 1;
}

int add_first_block(){
    void* diff = (void*)sfp;
    if(diff == NULL)
        return 0;
    diff = diff - 8;
    blk_ptr = (sf_block*) diff;
    blk.prev_footer = pptr.footer;
    blk.header = 4048;
    *blk_ptr = blk;
    return 1;
}

int init_free_lists(){
    for(int i = 0; i < NUM_FREE_LISTS; i++){
        sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
        sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
    }
    return 1;
}

int add_first_block_to_free_list(void *mem_grow/*, size_t size*/){
    int size = PAGE_SZ - sizeof(sfp) - sizeof(efp);
    // int size = sizeof(blk_ptr);
    int i;
    // every other call is just page_sz - epilogue
    if(size == M)
        i = 0;
    else if(size > M && (size <= 2 * M))
        i = 1;
    else if((size > 2 * M) && (size <= 4 * M))
        i = 2;
    else if ((size > 4 * M) && (size <= 8 * M))
        i = 3;
    else if ((size > 8 * M) && (size <= 16 * M))
        i = 4;
    else if ((size > 16 * M) && (size <= 32 * M))
        i = 5;
    else if ((size > 32 * M) && (size <= 64 * M))
        i = 6;
    else if ((size > 64 * M) && (size <= 128 * M))
        i = 7;
    else if ((size > 128 * M))
        i = 8;
    else
        i = -1;
    // do the rest
    if(i != -1){
        blk_ptr->body.links.next = sf_free_list_heads[i].body.links.next;
        blk_ptr->body.links.prev = &sf_free_list_heads[i];
        blk_ptr->body.links.next->body.links.prev = blk_ptr;
        sf_free_list_heads[i].body.links.next = blk_ptr;
        return 1;
    }
    return -1;
}

void *sf_malloc(size_t size) {
    // debug("%l)
    if(size == 0)
        return NULL;
    else if(size >= 4 * PAGE_SZ){
        sf_errno = ENOMEM;
        return NULL;
    }
    if(!calledBefore){

        // sf_mem_init();
        mem_grow = sf_mem_grow();
        init_free_lists();

        if(write_prologue() == 0)
            return NULL;
        if(add_first_block() == 0)
            return NULL;
        if(add_first_block_to_free_list(mem_grow/*, size*/) == 0)
            return NULL;
        if(write_epilogue() == 0)
            return NULL;

        calledBefore = 1;
        return blk_ptr;
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
