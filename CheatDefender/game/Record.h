#pragma once
#include "Message.h"


// Only used in server -> client communication

enum {
	ENCRYPTED,
	NOT_ENCRYPTED
};

struct Record {
	uint32_t seq_no;
	uint8_t encrypted; // Can remove later (Flag to tell wheter msg is encrypted)
	Message msg;
};

// Our only requirement is confidentiality.

