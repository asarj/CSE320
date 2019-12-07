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
        BRS_PACKET_HEADER *send = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
        BRS_PACKET_HEADER *recv = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
        void *p;
        if(p == NULL){

        }
//        creg_register(creg, fd);
        if(proto_recv_packet(fd, recv, &p) < 0){
            free(send);
            free(recv);
            free(p);
            send = NULL;
            recv = NULL;
            p = NULL;
        }
        if(recv == NULL){
            trader_logout(t);
            break;
        }
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
            trader_send_packet(t, send, NULL);
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
                trader_send_packet(t, send, NULL);
            }

            // All trader functions
            if(recv->type == BRS_STATUS_PKT){
                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);

                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
                newHeader->type = BRS_ACK_PKT;
                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = htonl(sizeof(brs_info));
//                newHeader->size = 0;
//                debug("The size of the header is %d", newHeader->size);
                proto_send_packet(fd, newHeader, &brs_info);
                trader_send_ack(t, &brs_info);
                free(newHeader);
//                free(brs_info);
            }
            else if(recv->type == BRS_DEPOSIT_PKT){
                BRS_FUNDS_INFO *brs_funds = (BRS_FUNDS_INFO *)p;
                trader_increase_balance(t, ntohl(brs_funds->amount));

                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);

                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
                newHeader->type = BRS_ACK_PKT;
                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = htonl(sizeof(brs_info));
//                newHeader->size = 0;
//                debug("The size of the header is %d", newHeader->size);
//                proto_send_packet(fd, newHeader, &brs_info);
                trader_send_ack(t, &brs_info);
                free(newHeader);
                free(brs_funds);
//                debug("deposit ends");
            }
            else if(recv->type == BRS_WITHDRAW_PKT){
                // Check this later
//                debug("%d", *((int *)p));
                BRS_FUNDS_INFO *brs_funds = (BRS_FUNDS_INFO *)p;
                int ret = trader_decrease_balance(t, ntohl(brs_funds->amount));

                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);
                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);

                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = htonl(sizeof(brs_info));

                if(ret == -1){
                    newHeader->type = BRS_NACK_PKT;
                    newHeader->size = 0;
//                    proto_send_packet(fd, send, NULL);
                    trader_send_nack(t);
                }
                else{
                    newHeader->type = BRS_ACK_PKT;
                    newHeader->size = htonl(sizeof(brs_info));
//                    proto_send_packet(fd, send, &brs_info);
                    trader_send_ack(t, &brs_info);
                }
                free(newHeader);
                free(brs_funds);
            }
            else if(recv->type == BRS_ESCROW_PKT){
                BRS_ESCROW_INFO *brs_escrow = (BRS_ESCROW_INFO *)p;
                trader_increase_inventory(t, htonl(brs_escrow->quantity));

                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);

                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
                newHeader->type = BRS_ACK_PKT;
                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = htonl(sizeof(brs_info));
//                proto_send_packet(fd, send, &brs_info);
                trader_send_ack(t, &brs_info);
                free(newHeader);
                free(brs_escrow);
            }
            else if(recv->type == BRS_RELEASE_PKT){
                BRS_ESCROW_INFO *brs_escrow = (BRS_ESCROW_INFO *)p;
                int ret = trader_decrease_inventory(t, htonl(brs_escrow->quantity));

                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;
                exchange_get_status(exchange, &brs_info);
                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);

                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = htonl(sizeof(brs_info));

                if(ret == -1){
                    newHeader->type = BRS_NACK_PKT;
                    newHeader->size = 0;
//                    proto_send_packet(fd, newHeader, NULL);
                    trader_send_nack(t);
                }
                else{
                    newHeader->type = BRS_ACK_PKT;
//                    proto_send_packet(fd, newHeader, &brs_info);
                    trader_send_ack(t, &brs_info);
                }
                free(newHeader);
                free(brs_escrow);
            }
            else if(recv->type == BRS_BUY_PKT){
                BRS_ORDER_INFO *brs_order = (BRS_ORDER_INFO *)p;
                orderid_t id = exchange_post_buy(exchange, t, htonl(brs_order->quantity), htonl(brs_order->price));

                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;

                exchange_get_status(exchange, &brs_info);
//                brs_info.orderid = htonl(id);
//                brs_info.quantity = htonl(brs_info.quantity);
//                brs_info.bid = htonl(brs_info.bid);
//                brs_info.inventory = htonl(brs_info.inventory);
//                brs_info.ask = htonl(brs_info.ask);
//                brs_info.last = htonl(brs_info.last);
//                brs_info.balance = htonl(brs_info.balance);

                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);

                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = htonl(sizeof(brs_info));
//                proto_send_packet(fd, send, &brs_info);
                if(id == 0){
                    newHeader->type = BRS_NACK_PKT;
                    newHeader->size = 0;
                    trader_send_nack(t);
                }
                else{
                    newHeader->type = BRS_ACK_PKT;
                    newHeader->size = htonl(sizeof(brs_info));
                    trader_send_ack(t, &brs_info);
                }
                trader_send_ack(t, &brs_info);
                free(newHeader);
                free(brs_order);
            }
            else if(recv->type == BRS_SELL_PKT){
                BRS_ORDER_INFO *brs_order = (BRS_ORDER_INFO *)p;
                orderid_t id = exchange_post_sell(exchange, t, htonl(brs_order->quantity), htonl(brs_order->price));

                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;

                exchange_get_status(exchange, &brs_info);
//                brs_info.orderid = htonl(id);
//                brs_info.quantity = htonl(brs_info.quantity);
//                brs_info.bid = htonl(brs_info.bid);
//                brs_info.inventory = htonl(brs_info.inventory);
//                brs_info.ask = htonl(brs_info.ask);
//                brs_info.last = htonl(brs_info.last);
//                brs_info.balance = htonl(brs_info.balance);

                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
//                newHeader->type = BRS_ACK_PKT;
                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = htonl(sizeof(brs_info));
//                proto_send_packet(fd, send, &brs_info);
//                trader_send_ack(t, &brs_info);
                if(id == 0){
                    newHeader->type = BRS_NACK_PKT;
                    newHeader->size = 0;
                    trader_send_nack(t);
                }
                else{
                    newHeader->type = BRS_ACK_PKT;
                    newHeader->size = htonl(sizeof(brs_info));
                    trader_send_ack(t, &brs_info);
                }
                free(newHeader);
                free(brs_order);
            }
            else if(recv->type == BRS_CANCEL_PKT){
                quantity_t quantity;
                BRS_CANCEL_INFO *brs_order = (BRS_CANCEL_INFO *)p;
                int id = exchange_cancel(exchange, t, htonl(brs_order->order), &quantity);

                BRS_PACKET_HEADER *newHeader = (BRS_PACKET_HEADER *)malloc(sizeof(BRS_PACKET_HEADER));
                BRS_STATUS_INFO brs_info;

                exchange_get_status(exchange, &brs_info);
//                brs_info.orderid = htonl(id);
                brs_info.quantity = htonl(brs_info.quantity);
//                brs_info.bid = htonl(brs_info.bid);
//                brs_info.inventory = htonl(brs_info.inventory);
//                brs_info.ask = htonl(brs_info.ask);
//                brs_info.last = htonl(brs_info.last);
//                brs_info.balance = htonl(brs_info.balance);

                struct timespec time;
                clock_gettime(CLOCK_MONOTONIC, &time);
//                newHeader->type = BRS_ACK_PKT;
                newHeader->timestamp_sec = htonl(time.tv_sec);
                newHeader->timestamp_nsec = htonl(time.tv_nsec);
                newHeader->size = htonl(sizeof(brs_info));
//                proto_send_packet(fd, send, &brs_info);
//                trader_send_ack(t, &brs_info);
                if(id == -1){
                    newHeader->type = BRS_NACK_PKT;
                    newHeader->size = 0;
                    trader_send_nack(t);
                }
                else{
                    newHeader->type = BRS_ACK_PKT;
                    newHeader->size = htonl(sizeof(brs_info));
                    trader_send_ack(t, &brs_info);
                }
                free(newHeader);
                free(brs_order);
            }
        }
        free(send);
        free(recv);
        free(p);
        send = NULL;
        recv = NULL;
        p = NULL;

//        debug("while ends");
    }


    return NULL;
}