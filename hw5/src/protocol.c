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

int proto_send_packet(int fd, BRS_PACKET_HEADER *hdr, void *payload){
//    debug("--- Packet Header Information (WRITE)---");
//    debug("Packet Type: %i", hdr->type);
//    debug("Payload Length: %i", hdr->size);
//    debug("Timestamp Sec: %i", hdr->timestamp_sec);
//    debug("Timestamp Nsec: %i", hdr->timestamp_nsec);

    uint16_t size = ntohs(hdr->size);
//    hdr->type = htonl(hdr->type);
//    hdr->size = htonl(hdr->size);
//    hdr->timestamp_sec = htonl(hdr->timestamp_sec);
//    hdr->timestamp_nsec = htonl(hdr->timestamp_nsec);

    int write_res = write(fd, hdr, sizeof(BRS_PACKET_HEADER));
    if(write_res == -1 /*< sizeof(BRS_PACKET_HEADER)*/) {
//        debug("Writing to packet returned -1");
        return -1;
    }
    if(payload != NULL && size != 0){
        int w = write(fd, payload, size);
        if(w == -1/*< size*/){
//            debug("Writing to payload returned -1");
            return -1;
        }
    }

//    if (rio_writen(fd, hdr, sizeof(BRS_PACKET_HEADER)) < 0)
//        return -1;
//    if(size > 0){
//        if(rio_writen(fd, payload, size) < 0)
//            return -1;
//    }
    return 0;
}

int proto_recv_packet(int fd, BRS_PACKET_HEADER *hdr, void **payloadp){
//    debug("--- Packet Header Information (READ)---");

    int read_res = read(fd, hdr, sizeof(BRS_PACKET_HEADER));
//    debug("I finished reading the first time");
    if(read_res <= 0/*< sizeof(BRS_PACKET_HEADER)*/) {
        debug("EOF on fd: %d", fd);
        return -1;
    }

    uint16_t size = htons(hdr->size);

    if(payloadp != NULL && size != 0){
        void *p = (void *)malloc(size);
        if(p == NULL)
            return -1;

        int r = read(fd, p, size);
        if(r <= 0/*< hdr->size*/){
            free(p);
            return -1;
        }

        *payloadp = p;
    }

    return 0;
}