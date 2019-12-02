#include <stdlib.h>
#include <getopt.h>
//#include <bits/signum.h>
#include <signal.h>
#include "client_registry.h"
#include "exchange.h"
#include "trader.h"
#include "server.h"
#include "debug.h"
#include "csapp.h"

extern EXCHANGE *exchange;
extern CLIENT_REGISTRY *client_registry;
void signalHandler(int signnum);

static void terminate(int status);

/*
 * "Bourse" exchange server.
 *
 * Usage: bourse <port>
 */
void signalHandler(int signum){
    if(signum == SIGHUP){
        terminate(EXIT_SUCCESS);
    }
}
int main(int argc, char* argv[]){
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.
    char *options = ":ph";
    extern char *optarg;
    extern int optind, optopt, opterr;
    int c;
    int port = 0;
    char *host = malloc(sizeof(char)), *strport = malloc(sizeof(char));

    while((c = getopt(argc, argv, options)) != -1) {

        if (argc < 1)
            break;
        switch (c) {
            case 'p':
                if (port == 0) {
                    strcpy(strport, argv[optind]);
                    port = atoi(argv[optind]);

                }

//                if(port == NULL || port == '-' || port == 0){
//                    fprintf(stderr,"Illegal syntax using -p option\n");
//                }
                break;
            case 'h':
                strcpy(host, argv[optind]);
                break;
            case '?':
                debug("illegal option %d\n", c);
                break;
            case 1:
                debug("%s\n", optarg);
                // debug("%s\n", optarg);
                break;
            default:
                debug("Default option reached: %d", c);
                break;
        }

        if (strcmp(host, "") == 0 || host == NULL || strlen(host) == 0) {
            strcpy(host, "localhost");
        }

        // Perform required initializations of the client_registry,
        // maze, and player modules.
        client_registry = creg_init();
        exchange = exchange_init();
        trader_init();

        // TODO: Set up the server socket and enter a loop to accept connections
        // on this socket.  For each connection, a thread should be started to
        // run function brs_client_service().  In addition, you should install
        // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
        // shutdown of the server.
        Signal(SIGHUP, signalHandler);

        int listenfd, *connfdp;
        socklen_t clientlen;
        struct sockaddr_storage clientaddr; pthread_t tid;
        listenfd = Open_listenfd(strport);
        while (1) {
            clientlen=sizeof(struct sockaddr_storage);
            connfdp = Malloc(sizeof(int));
            *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
            Pthread_create(&tid, NULL, brs_client_service, connfdp);
        }
//
//        while (1) {
//        listen(sockfd, 50);
//            int* fd = malloc(sizeof(int*));
//            struct sockaddr *my_addr;
//            socklen_t * length;
//            my_addr->sa_family = AF_UNIX;
//            strcpy(my_addr->sa_data , strport);
//            *length = strlen(strport);
//            *fd = Accept(clientfd, my_addr, length);
//            if(length){}
//            if(*fd > 0){
//                pthread_t tid;
//                Pthread_create(&tid, NULL, brs_client_service, fd);
////                debug("%d", x);
//                //Pthread_join(tid, NULL);
//
//            }
//            else{
//                free(fd);
//            }

//        }
    }

    fprintf(stderr, "You have to finish implementing main() "
	    "before the Bourse server will function.\n");

    terminate(EXIT_FAILURE);
}

/*
 * Function called to cleanly shut down the server.
 */
static void terminate(int status) {
    // Shutdown all client connections.
    // This will trigger the eventual termination of service threads.
    creg_shutdown_all(client_registry);
    
    debug("Waiting for service threads to terminate...");
    creg_wait_for_empty(client_registry);
    debug("All service threads terminated.");

    // Finalize modules.
    creg_fini(client_registry);
    exchange_fini(exchange);
    trader_fini();

    debug("Bourse server terminating");
    exit(status);
}
