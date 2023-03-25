/**
 * Julia Matuszewska 324093
 **/

#include "send.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include <sys/time.h>
#include <assert.h>

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

u_int16_t compute_icmp_checksum(const void *buff, int length) {
    assert (length % 2 == 0);
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	
	for (sum = 0; length > 0; length -= 2) {
        sum += *ptr++;
    }
	
	sum = (sum >> 16) + (sum & 0xffff);

	return (u_int16_t)(~(sum + (sum >> 16)));
}

struct icmp create_packet(uint16_t id, int seq) {
    struct icmp icmp_header;
    icmp_header.icmp_type = ICMP_ECHO;
    icmp_header.icmp_code = 0;
    icmp_header.icmp_hun.ih_idseq.icd_seq = seq;
    icmp_header.icmp_hun.ih_idseq.icd_id = id;
    icmp_header.icmp_cksum = 0;
    icmp_header.icmp_cksum = compute_icmp_checksum((u_int16_t*) &icmp_header,
                                sizeof(icmp_header));
    return icmp_header;
}

int send_packet(int sockfd, struct sockaddr_in *recipient, uint16_t id, int ttl,
                int seq) {
    /* Setting socket options */
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0)
        return -1;

    struct icmp icmp_header = create_packet(id, seq);
    /* Sending the packet */
    return sendto(
        sockfd,
        &icmp_header,
        sizeof(icmp_header),
        0,
        (struct sockaddr *)recipient,
        sizeof(*recipient)
    );
}