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
#include "time.h"
#define MAX_PAYLOAD_SIZE 10240
volatile int logged_in = 0;
TRADER *t;
CLIENT_REGISTRY *creg;

void *brs_client_service(void *arg){
    int fd = *((int *)arg);
    free(arg);
    Pthread_detach(Pthread_self());
    creg_register(creg, fd);
    while(1){
        debug("ln22");
        BRS_PACKET_HEADER *send = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
        BRS_PACKET_HEADER *recv = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
        void *p = (void *)malloc(sizeof(char) * MAX_PAYLOAD_SIZE);
        creg_register(creg, fd);
        debug("Before");
        if(proto_recv_packet(fd, recv, &p) < 0){
            free(send);
            free(recv);
            free(p);
        }
        debug("Something");
        if(recv->type == BRS_LOGIN_PKT){
            logged_in = 1;
            t = trader_login(fd, (char *)p);
            struct timespec time;
            clock_gettime(CLOCK_MONOTONIC, &time);
            send->timestamp_sec = htonl(time.tv_sec);
            send->timestamp_nsec = htonl(time.tv_nsec);
            if(t != NULL){
                send->type = BRS_ACK_PKT;
            }
            else{
                send->type = BRS_NACK_PKT;
            }
            proto_send_packet(fd, send, NULL);
            trader_send_packet(t, send, &p);
        }
        if(logged_in){
            if(t == NULL){
                t = trader_login(fd, p);
                if(t != NULL){
                    send->type = BRS_ACK_PKT;
                }
                else{
                    send->type = BRS_NACK_PKT;
                }
                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
                send->timestamp_sec = htonl(time.tv_sec);
                send->timestamp_nsec = htonl(time.tv_nsec);
                proto_send_packet(fd, send, NULL);
                trader_send_packet(t, send, p);
            }

            // All trader functions
            if(recv->type == BRS_STATUS_PKT){
//                debug("The fd is %d", fd);
                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);

                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
                newHeader->type = BRS_ACK_PKT;
                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = sizeof(brs_info);
                newHeader->size = 0;
                debug("The size of the header is %d", newHeader->size);
                proto_send_packet(fd, newHeader, &brs_info);
                trader_send_ack(t, &brs_info);
            }
            else if(recv->type == BRS_DEPOSIT_PKT){
                BRS_FUNDS_INFO *brs_funds;
                brs_funds = (BRS_FUNDS_INFO *)p;
                trader_increase_balance(t, brs_funds->amount);

                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);
                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
                send->type = BRS_ACK_PKT;
                send->timestamp_sec = htonl(time.tv_sec);
                send->timestamp_nsec = htonl(time.tv_nsec);
                send->size = sizeof(brs_info);
                proto_send_packet(fd, send, &brs_info);
                trader_send_ack(t, &brs_info);
                debug("deposit ends");
            }
            else if(recv->type == BRS_WITHDRAW_PKT){
                // Check this later
                debug("%d", *((int *)p));
                int ret = trader_decrease_balance(t, (funds_t)*((int *)p));

                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);
                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);

                send->timestamp_sec = htonl(time.tv_sec);
                send->timestamp_nsec = htonl(time.tv_nsec);
//                send->size = sizeof(brs_info);

                if(ret == -1){
                    send->type = BRS_NACK_PKT;
                    send->size = 0;
                    proto_send_packet(fd, send, NULL);
                    trader_send_nack(t);
                }
                else{
                    send->type = BRS_ACK_PKT;
                    send->size = sizeof(brs_info);
                    proto_send_packet(fd, send, &brs_info);
                    trader_send_ack(t, &brs_info);
                }

            }
            else if(recv->type == BRS_ESCROW_PKT){
                trader_increase_inventory(t, (funds_t)*((int *)p));

                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);
                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);

                send->timestamp_sec = htonl(time.tv_sec);
                send->timestamp_nsec = htonl(time.tv_nsec);
                send->size = sizeof(brs_info);
                proto_send_packet(fd, send, &brs_info);
                trader_send_ack(t, &brs_info);
            }
            else if(recv->type == BRS_RELEASE_PKT){
                int ret = trader_decrease_inventory(t, (funds_t)*((int *)p));

                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);
                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);

                send->timestamp_sec = htonl(time.tv_sec);
                send->timestamp_nsec = htonl(time.tv_nsec);
                send->size = sizeof(brs_info);

                if(ret == -1){
                    send->type = BRS_NACK_PKT;
                    proto_send_packet(fd, send, NULL);
                    trader_send_nack(t);
                }
                else{
                    send->type = BRS_ACK_PKT;
                    proto_send_packet(fd, send, &brs_info);
                    trader_send_ack(t, &brs_info);
                }
            }
//            else if(recv->type == BRS_BUY_PKT){
//                BRS_ORDER_INFO brs_info = (BRS_ORDER_INFO)p;
//                orderid_t id = exchange_post_buy(exhange, t, brs_info.quantity, brs_info.price);
//
//            }
//            else if(recv->type == BRS_SELL_PKT){
//                exchange_post_sell(t, )
//            }
//            else if(recv->type == BRS_CANCEL_PKT){
//                exchange_cancel(exchange, t, )
//            }
        }
        free(send);
        free(recv);
        free(p);
        debug("while ends");
    }
    return NULL;
}