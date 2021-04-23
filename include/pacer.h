#pragma once 

// WARNING: This is not authoritative
#define PACER_PORT "8463"

#define TRUE 1
#define FALSE 0

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
