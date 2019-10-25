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

int roundTo16(int num, int mul){
    if (num % mul == 0)
        return num;

    return num + mul - (num % mul);
}

int getIndexFromSize(int size){
    int i;

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

    return i;
}

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

    // ftr_ptr = (sf_footer *) efp;
    // ftr_ptr = ftr_ptr - 1;
    // ftr = blk.header ^ sf_magic();
    // *ftr_ptr = ftr;
    return 1;
}

int add_first_block(size_t size){
    void* diff = (void*)sfp;
    if(diff == NULL)
        return 0;
    diff = diff - 8;
    blk_ptr = (sf_block*) diff;
    blk.prev_footer = pptr.footer;
    blk.header = 4048;
    *blk_ptr = blk;
    debug("%ld", sf_mem_start() - (void*)blk_ptr);
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
    int i = getIndexFromSize(size);
    if(i != -1){
        blk_ptr->body.links.next = sf_free_list_heads[i].body.links.next;
        blk_ptr->body.links.prev = &sf_free_list_heads[i];
        blk_ptr->body.links.next->body.links.prev = blk_ptr;
        sf_free_list_heads[i].body.links.next = blk_ptr;
        return 1;
    }
    return -1;
}

void *search_free_lists(size_t size){
    int i = getIndexFromSize(size);
    if(i != -1){
        while(sf_free_list_heads[i].body.links.next == &sf_free_list_heads[i]){
            i++;
        }
        if(i == 9)
            i = 8;
        debug("%ld", size);
        sf_block *blk = sf_free_list_heads[i].body.links.next;
        debug("The size of the block is: %ld", ((*blk).header));
        if((*blk).header >= size){
            debug("I AM INSIDE");
            sf_block blk2;
            blk2.prev_footer = (*blk).prev_footer;
            debug("%ld", blk2.prev_footer);
            blk2.header = size + 3;
            debug("%ld", size);
            void *add = (void *)blk;
            add = add + size;

            sf_block addPoint;
            addPoint.prev_footer = blk2.header ^ sf_magic();
            debug("%ld", addPoint.prev_footer);
            addPoint.header = ((*blk).header & BLOCK_SIZE_MASK) - size + 1;
            // debug("%ld", addPoint.header & BLOCK_SIZE_MASK);
            //debug("%ld", size);
            *blk = blk2;
            *((sf_block*)add) = addPoint;
            add = add + (addPoint.header & BLOCK_SIZE_MASK); // fix for 64
            *((sf_footer *)add) = addPoint.header ^ sf_magic();

            // blk->body.links.prev = &sf_free_list_heads[i];
            sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
            sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
            int size_to_add = addPoint.header & BLOCK_SIZE_MASK;
            add = add - (addPoint.header & BLOCK_SIZE_MASK);
            add_to_free_list(size_to_add, add);
            // sf_show_blocks();
        }

        return (*blk).body.payload;
    }
    return NULL;
}

void add_to_free_list(int size, void *add){
    int i = getIndexFromSize(size);
    if(i != -1){
        sf_free_list_heads[i].body.links.next = (sf_block*)add;
        sf_free_list_heads[i].body.links.prev = (sf_block*)add;
        (*(sf_block*)add).body.links.next = &sf_free_list_heads[i];
        // (*(sf_block*)add).header =
        (*(sf_block*)add).body.links.prev = &sf_free_list_heads[i];
    }
}

void *sf_malloc(size_t size) {
    if(size == 0)
        return NULL;
    else if(size > 4 * PAGE_SZ || usedSpace + size > 4 * PAGE_SZ){
        sf_errno = ENOMEM;
        return NULL;
    }
    if(!calledBefore){
        mem_grow = sf_mem_grow();
        if(mem_grow == NULL)
            return NULL;
        usedSpace += 4096;
        init_free_lists();
        debug("%ld", size);
        // size += 8;
        if(write_prologue() == 0)
            return NULL;
        if(add_first_block(size) == 0)
            return NULL;
        if(add_first_block_to_free_list(mem_grow/*, size*/) == 0)
            return NULL;
        if(write_epilogue() == 0)
            return NULL;
        calledBefore = 1;

        // return blk_ptr;
    }
    if(size != 4*PAGE_SZ){
        size = size + 16;
        size = roundTo16(size, 16);
    }

    // debug("%ld", size);
    while(size > sf_mem_end() - (void*)(blk_ptr) - 8){
        mem_grow = sf_mem_grow();
        if(mem_grow == NULL)
            return NULL;
        void* e = sf_mem_end();
        e -= 8;
        memcpy(e, efp, 8);
        sf_header blank = 0;
        (*efp).header = blank;
        blk_ptr->header += 4096;
        usedSpace += 4096;
        // if(usedSpace >= 4 * PAGE_SZ){
        //     debug("Error caught");
        //     sf_errno = ENOMEM;
        //     return NULL;
        // }
        if((blk_ptr->header ^ BLOCK_SIZE_MASK) == 4096){
            blk_ptr->body.links.next->body.links.prev = blk_ptr->body.links.prev;
            blk_ptr->body.links.prev->body.links.next = blk_ptr->body.links.next;

            sf_free_list_heads[7].body.links.prev->body.links.next = blk_ptr;
            blk_ptr->body.links.prev = sf_free_list_heads[7].body.links.prev;
            sf_free_list_heads[7].body.links.next = blk_ptr;
            blk_ptr->body.links.next = &sf_free_list_heads[7];
        }
        else{
            blk_ptr->body.links.next->body.links.prev = blk_ptr->body.links.prev;
            blk_ptr->body.links.prev->body.links.next = blk_ptr->body.links.next;

            sf_free_list_heads[8].body.links.prev->body.links.next = blk_ptr;
            blk_ptr->body.links.prev = sf_free_list_heads[8].body.links.prev;
            sf_free_list_heads[8].body.links.next = blk_ptr;
            blk_ptr->body.links.next = &sf_free_list_heads[8];
        }
        // sf_show_free_lists();
        void* add = (void*)efp;
        add = add + ((*efp).header & BLOCK_SIZE_MASK); // fix for 64
        *((sf_footer *)add) = (*efp).header ^ sf_magic();
        efp = (sf_epilogue*)e;
        efp->header = 2;
        debug("%ld", size);
    }

    // debug("The decimal is %ld",(void*)(sf_free_list_heads[7].body.links.next)-sf_mem_start());
    debug("%ld", size);
    return search_free_lists(size);
        // search free list
        // if enough ...
        // else {sf_mem_grow(), set new page, set free list}
    // }
    // return NULL;
}

void coalesce(void *pp, sf_footer foot, sf_header head){

    if(pp - sf_mem_start() == 48){ // do right code here
        sf_block* curr = (sf_block *)pp - 16;
        sf_block* prev = (sf_block *)pp - 16 - (foot & BLOCK_SIZE_MASK);
        sf_block* next = (sf_block *)pp - 16 + (head & BLOCK_SIZE_MASK);
        if(!(next->header & PREV_BLOCK_ALLOCATED)){

            next->body.links.prev->body.links.next = curr;
            next->body.links.prev->body.links.prev = curr;

            curr->header += (prev->header & BLOCK_SIZE_MASK);
            next->prev_footer = prev->header ^ sf_magic();
        }
    }
    else if(pp + (head & BLOCK_SIZE_MASK) == sf_mem_end() - 8){ // do left code here
        sf_block* curr = (sf_block *)pp - 16;
        sf_block* prev = (sf_block *)pp - 16 - (foot & BLOCK_SIZE_MASK);
        sf_block* next = (sf_block *)pp - 16 + (head & BLOCK_SIZE_MASK);

        next->body.links.prev->body.links.next = curr;
        next->body.links.prev->body.links.prev = curr;

        prev->header += (curr->header & BLOCK_SIZE_MASK);
        next->prev_footer = prev->header ^ sf_magic();
        next->header = next->header - 1;
    }



    // Middle
    if(0){

    }
    // Left
    if(!(foot & PREV_BLOCK_ALLOCATED)){
        sf_block* curr = (sf_block *)pp - 16;
        sf_block* prev = (sf_block *)pp - 16 - (foot & BLOCK_SIZE_MASK);
        sf_block* next = (sf_block *)pp - 16 + (head & BLOCK_SIZE_MASK);

        next->body.links.prev->body.links.next = curr;
        next->body.links.prev->body.links.prev = curr;

        prev->header += (curr->header & BLOCK_SIZE_MASK);
        next->prev_footer = prev->header ^ sf_magic();
        next->header = next->header - 1;

    }
    // Right
    if(!(head & PREV_BLOCK_ALLOCATED)){
        sf_block* curr = (sf_block *)pp - 16;
        sf_block* prev = (sf_block *)pp - 16 - (foot & BLOCK_SIZE_MASK);
        sf_block* next = (sf_block *)pp - 16 + (head & BLOCK_SIZE_MASK);
        if(!((next->prev_footer ^ sf_magic()) & PREV_BLOCK_ALLOCATED)){

            next->body.links.prev->body.links.next = curr;
            next->body.links.prev->body.links.prev = curr;

            curr->header += (prev->header & BLOCK_SIZE_MASK);
            next->prev_footer = prev->header ^ sf_magic();
        }

    }
}

void sf_free(void *pp) {
    sf_footer foot = ((((sf_footer)*(char *)pp - 16) ^ sf_magic()));
    sf_header head = ((sf_header)*(char *)pp - 24 - ((sf_header)*(char *)pp-8));
    if(pp == NULL ||
        ((size_t)*(char *)(pp - 8) & PREV_BLOCK_ALLOCATED) == 0 ||
        !(pp <= sf_mem_end()&& pp >= sf_mem_start()) ||
        (size_t)*(char *)(pp - 8) < 32 ||
        ((((size_t)*(char *)(pp - 8) & PREV_BLOCK_ALLOCATED) == 0) && foot != 0) ||
        (((((size_t)*(char *)(pp - 8) & BLOCK_SIZE_MASK) % 16 != 0))))
        abort();
    else if(head != foot)
        abort();
    // else if()
    coalesce(pp, foot, head);
    return;
}

void *sf_realloc(void *pp, size_t rsize) {
    return NULL;
}
