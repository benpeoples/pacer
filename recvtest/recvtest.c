/*
Pacer recieve test
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

	getaddrinfo("0.0.0.0", PACER_PORT, &hints, &res);

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

void print_replies(void)
{
	size_t bytes;
	unsigned char buf[512];
	struct pkt_pacer_time *pkt = (struct pkt_pacer_time *)&buf;
	int i;
	struct timeval tv;

	bytes = recv(pacer_socket, &buf, sizeof(buf), 0);
	if (bytes == -1) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			return;
		perror("recv");
		exit(1);
	}
	gettimeofday(&tv,NULL);
	if (bytes == 0)
		return;
	
	printf("====================== Received %d bytes\n", (int)bytes);

	DumpHex(&buf,bytes);

	printf("Version: %u  Message Type: %u  Timecode hint: %u\n",pkt->version,pkt->message_type,pkt->version);

	printf("Sender ID: ");
	for(int x = 0; x < 16; x++)
	{
		printf("%02X ", pkt->sender_id[x]);
	}
	printf("\n");

	printf("Timeline ID: %u  Sequence: %u\n",pkt->timeline_id,pkt->sequence);
	if(pkt->timeline_status <= STATUS_MAX) {
		printf("Timeline Status: %s (%u)\n",status_text[pkt->timeline_status],pkt->timeline_status);
	} else {
		printf("Timeline Status: Unknown (%u)\n",pkt->timeline_status);
	}

	printf("REC %ld:%u/%u\n",tv.tv_sec,tv.tv_usec,1000000); // Recieved time (local)
	printf("REF %llu:%u/%u (REC-REF: %lld:%d)\n",pkt->ref_seconds,pkt->ref_fractional,pkt->ref_timescale,(tv.tv_sec - pkt->ref_seconds),(tv.tv_usec - pkt->ref_fractional)); // Refernce clock
	printf("OFF %llu:%u/%u\n",pkt->offset_seconds,pkt->offset_fractional,pkt->offset_timescale); // Offset time
	printf("TIM %llu:%u/%u\n",pkt->time_seconds,pkt->time_fractional,pkt->time_timescale); // Timeline time
}


int main(int argc, char *argv[])
{
	setup_socket();
	while(1)
	{
		print_replies();
	}

	return 0;
}
