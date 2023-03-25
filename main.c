/**
 * Julia Matuszewska 324093
 **/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <arpa/inet.h>

#include "send.h"
#include "receive.h"
#include "config.h"

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Provide IP address\n");
        return EXIT_FAILURE;
    }
    const char *ip_addr = argv[1];

    /* Addressing */
    struct sockaddr_in recipent;
    bzero(&recipent, sizeof(recipent));
    recipent.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip_addr, &(recipent.sin_addr)) != 1) {
        fprintf(stderr, "ERROR: Provided incorrect IP address\n");
        return EXIT_FAILURE;
    }

    /* Creating raw socket */
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "ERROR: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

    /* Using PID to distinguish packets */
    uint16_t id = getpid();

    for (int curr_ttl = 1; curr_ttl <= MAX_TTL; curr_ttl++) {
        struct timeval start_time;
        gettimeofday(&start_time, NULL);

        for (int j = 0; j < PACKETS_PER_HOP; j++) {
            if (send_packet(sockfd, &recipent, id, curr_ttl, curr_ttl + j) < 0) {
                fprintf(stderr, "ERROR: Couldn't send packets: %s\n",
                        strerror(errno));
                return EXIT_FAILURE;
            }
        }

        /* Receive packets for this hop */
        int res = receive(id, sockfd, MAX_RESPONSE_TIME, curr_ttl,
                            PACKETS_PER_HOP, &start_time);
        if (res < 0) {
            fprintf(stderr, "ERROR: Couldn't receive packets: %s\n",
                    strerror(errno));
            return EXIT_FAILURE;
        }
        if (res == 1) break;
    }

    close(sockfd);
    return EXIT_SUCCESS;
}