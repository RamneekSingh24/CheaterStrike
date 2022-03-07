#pragma once
#include "Keys.h"
#include "PlayerState.h"


enum {
	CL_SV_MSG_JOIN,
	CL_SV_MSG_LOCAL_STATE,
	CL_SV_MSG_QUIT,
	SV_CL_MSG_JOIN_OK,
	SV_CL_MSG_ALL_STATE,
	KEY_EXCHANGE
};

struct Message {
	uint8_t msgType;
	int32_t clientId;
	PlayerState state[MAX_PLAYERS];
	unsigned char ecc_pub_key[64];
};