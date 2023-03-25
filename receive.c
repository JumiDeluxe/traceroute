/**
 * Julia Matuszewska 324093
 **/

#include "receive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/socket.h>

double calc_avg_ms(struct timeval* times, int responses) {
	double sum = 0;
	for (int i = 0; i < responses; i++) {
        sum += 1000000 * times[i].tv_sec + times[i].tv_usec;
    }
	return sum / (1000 * responses);
}

void print_route(int ip_addr_len, char ip_addresses[][ip_addr_len],
                    struct timeval *times, int responses) {
    if (!responses) {
        printf("*\n");
        return;
    }
    /* Print addresess in the route, unless we printed them already */
    for (int i = 0; i < responses; i++) {
        bool not_printed = true;
        for (int j = 0; j < i; j++) {
            if (!strcmp(ip_addresses[i], ip_addresses[j])) {
                not_printed = false;
                break;
            }
        }
        if (not_printed) {
            printf("%s ", ip_addresses[i]);
        }
    }
    if (responses < 3) {
        printf("???\n");
    } else {
        printf("%.2fms\n", calc_avg_ms(times, responses));
    }
}

struct icmp *get_packet(int sockfd, char ip_addr[]) {
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];

    ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0,
        (struct sockaddr*) &sender, &sender_len);
    if (packet_len < 0) {
        fprintf(stderr, "ERROR: Couldn't receive packet\n");
        return NULL;
    }
    
    /* Convert address from binary to text form */
    if (inet_ntop(AF_INET, &(sender.sin_addr), ip_addr, IP_ADDR_LEN) == NULL) {
        fprintf(stderr, "ERROR: Couldn't convert address\n");
        return NULL;
    }
        
    struct ip* ip_header = (struct ip*) buffer;
    u_int8_t* icmp_packet = buffer + 4 * ip_header->ip_hl;
    return (struct icmp*)icmp_packet;
}

int receive(uint16_t id, int sockfd, int max_seconds, int ttl,
            int packets_per_hop, struct timeval* start_time) {
	bool success = false;
	int responses = 0;
	char ip_addresses[PACKETS_PER_HOP][IP_ADDR_LEN];
    
    struct timeval times[PACKETS_PER_HOP];
    struct timeval tv;
    tv.tv_sec = max_seconds;
    tv.tv_usec = 0;

    fd_set descriptors;
	

    /* Receive packets per hops until we got all or reached the destination */
	while (responses < PACKETS_PER_HOP) {
        FD_ZERO(&descriptors);
	    FD_SET(sockfd, &descriptors);
        /* Check if new packet appeared in the socket */
		int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
		if (ready < 0) return -1;   // error
		else if (ready == 0) break; // timeout

        /* Get packet from the socket */
		struct icmp* icmp_header = get_packet(sockfd,
                                                ip_addresses[responses]);
        uint8_t icmp_type = icmp_header->icmp_type;

        /* Check if we got a reply or time-exceeded message */
        if (icmp_type == ICMP_ECHOREPLY &&
                icmp_header->icmp_seq == ttl + responses &&
                icmp_header->icmp_id == id) {
            responses++;
            success = true;

        } else if (icmp_type == ICMP_TIME_EXCEEDED) {
            /* We have to shift the packet by 8 bytes */
            struct ip *ip_shifted = (struct ip*)((uint8_t *)icmp_header + 8);
            struct icmp *icmp_shifted = (struct icmp*)
                ((uint8_t *)ip_shifted + (*ip_shifted).ip_hl * 4);
            if (icmp_shifted->icmp_seq == ttl + responses &&
                    icmp_shifted->icmp_id == id) {
                responses++;
            }
        }
		
        /* Calculate delay */
        struct timeval t;
		gettimeofday(&t, NULL);
		timersub(&t, start_time, &times[responses-1]);
	}
	printf("%d. ", ttl, responses);
	print_route(IP_ADDR_LEN, ip_addresses, times, responses);

	return success;
}