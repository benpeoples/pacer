/*
Pacer transmission test
23 Apr 2021

WARNING: Do not take anything in this code as authoritative as to the spec.


*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>

// WARNING: This is not authoritative
#define PACER_PORT "8463"

#define TRUE 1
#define FALSE 0
typedef int bool;

// WARNING: THIS STRUCT IS MADE UP
struct pkt_pacer_time {
	uint8_t version;
	uint8_t message_type;
	uint8_t timeline_status;
	uint8_t timecode_hint;

	uint8_t sender_id[16];

	uint32_t timeline_id;
	uint32_t sequence;

	uint64_t ref_seconds;
	uint32_t ref_fractional;
	uint32_t ref_timescale;

	uint64_t offset_seconds;
	uint32_t offset_fractional;
	uint32_t offset_timescale;

	uint64_t time_seconds;
	uint32_t time_fractional;
	uint32_t time_timescale;

} __attribute__((packed));

// WARNING: These defs are not authoritative
#define STATUS_STOPPED 0
#define STATUS_STANDBY 1
#define STATUS_RUNNING 2
#define STATUS_PAUSED 3
#define STATUS_MAX 3

char *status_text[] = {
	"Stopped",
	"Standby",
	"Running",
	"Paused"
};

int pacer_socket;

static void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

void setup_socket(void)
{
	struct addrinfo hints, *res;
	struct sockaddr_in myaddr;
	struct timeval timeout;

	timeout.tv_sec = 3;
	timeout.tv_usec = 0;

	int yes = 1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	getaddrinfo("0.0.0.0", NULL, &hints, &res);

	pacer_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (pacer_socket == -1) {
		perror("socket");
		exit(1);
	}

	if (bind(pacer_socket, res->ai_addr, res->ai_addrlen) == -1) {
		perror("bind");
		exit(1);
	}

	if (setsockopt(pacer_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))
		== -1) {
	    perror("setsockopt");
	    exit(1);
	}

	if (setsockopt(pacer_socket,SOL_SOCKET,SO_BROADCAST,&yes,sizeof(int))
		== -1) {
	    perror("setsockopt");
	    exit(1);
	}

	if (setsockopt(pacer_socket,SOL_SOCKET,SO_RCVTIMEO,&timeout,
			sizeof(timeout)) == -1) {
	    perror("setsockopt");
	    exit(1);
	}
}

void send_packet(char *broadcast_address, uint8_t timeline_status, uint64_t timeline_seconds, uint32_t timeline_fractional, uint32_t timeline_timescale, uint32_t sequence, uint32_t timeline_id)
{
	struct pkt_pacer_time pkt;
	struct addrinfo hints, *dest;
	int i;
	int rv;
	int numbytes;
	struct timeval tv;

	memset(&pkt,0,sizeof(pkt));

	pkt.timeline_id = timeline_id;
	pkt.timeline_status = timeline_status;

	pkt.sequence = sequence;

	pkt.time_seconds = timeline_seconds;
	pkt.time_fractional = timeline_fractional;
	pkt.time_timescale = timeline_timescale;

	gettimeofday(&tv,NULL);

	pkt.ref_seconds = (uint64_t)tv.tv_sec;
	pkt.ref_fractional = tv.tv_usec;
	pkt.ref_timescale = 1000000;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(broadcast_address, PACER_PORT, &hints, &dest))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	

	if ((numbytes = sendto(pacer_socket, &pkt, sizeof(pkt), 0,
             dest->ai_addr, dest->ai_addrlen)) == -1) {
		perror("sendto");
		exit(1);
	}
	DumpHex(&pkt,sizeof(pkt));
}

int main(int argc, char *argv[])
{
	uint64_t timeline = 0;
	uint32_t seq = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: sendtest <IP address>\n");
		exit(1);
	}
	setup_socket();

	while(1) {
		send_packet(argv[1],STATUS_RUNNING,timeline/1000,timeline % 1000, 1000, seq++,1234);
		printf("Sent packet\n");
		timeline+= 1000;
		sleep(1);
	}

	return 0;
}
