#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {
    sf_mem_init();

    // double* ptr = sf_malloc(sizeof(double));
    // double* ptr2 = sf_malloc(sizeof(double));
    // *ptr = 320320320e-320;

    // printf("%f\n%f\n", *ptr, *ptr2);

    // sf_free(ptr);
    // sf_show_heap();
    // sf_mem_fini();

    // return EXIT_SUCCESS;

    // sf_mem_init();

    // int* ptr = sf_malloc(sizeof(int));
    // double* pt2 = sf_malloc(sizeof(double));
    // void *ptr = sf_malloc(PAGE_SZ << 2);

    // // *ptr = 320320320e-320;
    // // *ptr = 4;

    // // printf("%ls\n", ptr);
    // if(ptr == NULL){
    //     printf("\n");
    // }
    // else{
    //     printf("\n");
    // }
    // if(ptr == NULL || pt2 == NULL){
    //     printf("\n");
    // }
    // else{
    //     printf("\n");
    // }

    // sf_free(ptr);

    // Prints output
    // sf_show_blocks();
    // sf_show_free_lists();

    void *x = sf_malloc(sizeof(int));
    /* void *y = */ sf_malloc(10);
    x = sf_realloc(x, sizeof(int) * 10);
    sf_show_heap();
    sf_mem_fini();

    return EXIT_SUCCESS;
}
