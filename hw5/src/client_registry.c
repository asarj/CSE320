#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include "client_registry.h"
#include "exchange.h"
#include "trader.h"
#include "server.h"
#include "debug.h"
#include "csapp.h"
#include "semaphore.h"

struct client_registry{
    int *fds;       // buffer array
    int length;     // max slots
    int front;      // first item
    int rear;       // last item
    int used;
    sem_t mutex;    // protects access to buffer
    sem_t slots;    // available slots
    sem_t items;    // available items
} ;

CLIENT_REGISTRY *creg_init(){
    CLIENT_REGISTRY *creg = (CLIENT_REGISTRY *)malloc(sizeof(CLIENT_REGISTRY));
    debug("Initializing registry");
    creg->fds = (int *)malloc(sizeof(int) * FD_SETSIZE - 4);
    creg->length = FD_SETSIZE - 4;
    creg->front = 0;
    creg->rear = 0;
    creg->used = 0;
    Sem_init(&creg->mutex, 0, 1);
    Sem_init(&creg->slots, 0, FD_SETSIZE - 4);
    Sem_init(&creg->items, 0, 0);
    return creg;
}

void creg_fini(CLIENT_REGISTRY *cr){
    debug("Clearing registry");
    if(cr != NULL){
        if(cr->fds != NULL)
            Free(cr->fds);
        Free(cr);
    }
}

int creg_register(CLIENT_REGISTRY *cr, int fd){
    if(cr == NULL) {
        return -1;
    }
//    debug("One");
    if(cr->used == cr->length) {
        return -1;
    }
//    debug("Two");
//    for(int i = 0; i < cr->length; i++){
//        if((cr->fds)[i] == fd)
//            return -1;
//    }
//    P(&cr->items);
    P(&cr->mutex);
    cr->fds[(++cr->rear)%(cr->length)] = fd;
    cr->used = cr->used + 1;
    V(&cr->mutex);
    debug("Register client fd %d (total connected: %d)", fd, cr->used);
//    V(&cr->items);

    return 0;
}

int creg_unregister(CLIENT_REGISTRY *cr, int fd){
    if(fd < 0)
        return -1;
    if(cr->used == 0){
        return -1;
    }
    int item;
//    P(&cr->items);
    P(&cr->mutex);
    item = cr->fds[(++cr->front)%(cr->length)];
    cr->used = cr->used - 1;
    V(&cr->mutex);
//    V(&cr->slots);
    debug("Unregister client fd %d (total connected: %d)", fd, cr->used);
    return 0;
}

void creg_wait_for_empty(CLIENT_REGISTRY *cr){
//    debug("Waiting for service threads to terminate...")
    P(&cr->mutex);
    while(cr->used > 0);
    V(&cr->mutex);
}

void creg_shutdown_all(CLIENT_REGISTRY *cr){

    for(int i = 0; i < cr->length; i++){
        cr->used = cr->used - 1;
        if(cr->fds[i] != 0)
            debug("Shutting down client %d", cr->fds[i]);
        shutdown(cr->fds[i], SHUT_RD);
    }
}