#pragma once 

// WARNING: This is not authoritative
#define PACER_PORT "8463"

#define TRUE 1
#define FALSE 0

// STRUCT at of 29 Apr 2021
struct pkt_pacer_time {
	uint8_t version;
	uint8_t message_type;
	uint8_t reserved1;
	uint8_t reserved2;

	uint8_t sender_id[16];

	uint32_t sequence;
	uint32_t timeline_id;

	uint8_t timeline_status;
	uint8_t timecode_hint;
	uint8_t reserved3;
	uint8_t reserved4;

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

// STATUS as of 28 Apr 2021
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

/*
timecode_hint values 
Not a great way (without a 100 element array) to map these to names, so I didn't yet.
*/
#define TC_HINT_NONE 	0x00
#define TC_HINT_24 	 	0x01
#define TC_HINT_25		0x02
#define TC_HINT_30		0x03
#define TC_HINT_30df	0x13
#define TC_HINT_48		0x21
#define TC_HINT_50		0x22
#define TC_HINT_60		0x23
#define TC_HINT_23976	0x41
#define TC_HINT_24975	0x42
#define TC_HINT_2997	0x43
#define TC_HINT_2997df	0x53
#define TC_HINT_47952	0x61
#define TC_HINT_4995	0x62
#define TC_HINT_5994	0x63

#define TC_HINT_MASK_DF	0x10 // Drop frame
#define TC_HINT_MASK_DR	0x20 // Double rate
#define TC_HINT_MASK_VS	0x40 // Video speed

/*
timeline name packet

FIXME: Verify that casting the single char to an array is the right choice.

*/
struct pkt_pacer_name {
	uint8_t version;
	uint8_t message_type;
	uint8_t reserved1;
	uint8_t reserved2;

	uint8_t sender_id[16];

	uint32_t sequence;
	uint32_t timeline_id;
	uint32_t timeline_name_length;
	char timeline_name; // Caveat: I'm not 100% sure this is the right way to handle this in C
} __attribute__((packed));

/*
sender -> follower ping response
*/
struct pkt_pacer_pong {
	uint8_t version;
	uint8_t message_type;
	uint8_t reserved1;
	uint8_t reserved2;

	uint8_t sender_id[16];

	uint32_t sequence;
	uint8_t follower_id[16];

	uint32_t follower_sequence;

	uint64_t ref_seconds;
	uint32_t ref_fractional;
	uint32_t ref_timescale;
} __attribute__((packed));

/*
follower -> sender ping request
*/
struct pkt_pacer_ping {
	uint8_t version;
	uint8_t message_type;
	uint8_t reserved1;
	uint8_t reserved2;

	uint8_t follower_id[16];

	uint32_t follower_sequence;
} __attribute__((packed));


/*
This is a generic packet that we can use to figure out which kind of packet it is
*/
struct pkt_pacer_generic {
	uint8_t version;
	uint8_t message_type;
	uint8_t reserved1;
	uint8_t reserved2;

	uint8_t sender_id[16];

	uint32_t sequence;
} __attribute__((packed));