#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {
    sf_mem_init();

    // double* ptr = sf_malloc(sizeof(double));

    // *ptr = 320320320e-320;

    // printf("%f\n", *ptr);

    // sf_free(ptr);

    // sf_mem_fini();

    // return EXIT_SUCCESS;

    // sf_mem_init();

    int* ptr = sf_malloc(sizeof(int));
    double* pt2 = sf_malloc(sizeof(double));

    // *ptr = 320320320e-320;
    *ptr = 4;

    // printf("%f\n", ptr);
    // if(ptr == NULL){
    //     printf("\n");
    // }
    // else{
    //     printf("\n");
    // }
    if(ptr == NULL || pt2 == NULL){
        printf("\n");
    }
    else{
        printf("\n");
    }

    // sf_free(ptr);

    // Prints output
    // sf_show_blocks();
    // sf_show_free_lists();
    sf_show_heap();
    sf_mem_fini();

    return EXIT_SUCCESS;
}
