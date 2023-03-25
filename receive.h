/**
 * Julia Matuszewska 324093
 **/

#ifndef RECEIVE_H
#define RECEIVE_H

#include "config.h"

#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

/**
 * Calculates average time in miliseconds.
 **/
double calc_avg_ms(struct timeval* times, int responses);

/**
 * Prints user-readable route representation containing IP address of router
 * (or many addresses) and average time of responses in miliseconds.
 * In case of no response "*" is printed.
 * If less than 3 packets were received in the specified time "???" is printed
 * instead of the average time.
 **/
void print_route(int ip_addr_len, char ip_str[][ip_addr_len],
                struct timeval *time, int responses);

/**
 * Receives packets from hop
 * Returns 1 if we reached destination, 0 if not
 * Returns -1 on error
 **/
int receive(uint16_t id, int sockfd, int max_seconds, int ttl,
            int packets_per_hop, struct timeval* start_time);

#endif /* RECEIVE_H */