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

};

CLIENT_REGISTRY *creg_init(){

}

void creg_fini(CLIENT_REGISTRY *cr){

}

int creg_register(CLIENT_REGISTRY *cr, int fd){

}

int creg_unregister(CLIENT_REGISTRY *cr, int fd){

}

void creg_wait_for_empty(CLIENT_REGISTRY *cr){

}

void creg_shutdown_all(CLIENT_REGISTRY *cr){

}