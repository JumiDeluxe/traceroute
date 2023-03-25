/**
 * Julia Matuszewska 324093
 **/

#ifndef SEND_H
#define SEND_H

#include "config.h"

#include <stdlib.h>
#include <netinet/ip_icmp.h>

u_int16_t compute_icmp_checksum(const void *buff, int length);

/**
 * Create a ICMP packet for sending - ICMP echo request the header is enough.
 **/
struct icmp create_packet(uint16_t id, int seq);

/**
 * Sending packet through the socket.
 * Returning number of bytes send.
 * On error returning -1
 **/
int send_packet(int sockfd, struct sockaddr_in *recipient, uint16_t id, int ttl,
                int seq);

#endif /* SEND_H */