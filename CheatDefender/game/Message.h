#pragma once
#include "Keys.h"
#include "PlayerState.h"

#define CL_SV_MSG_JOIN 0
#define CL_SV_MSG_LOCAL_STATE 1
#define	CL_SV_MSG_QUIT 2

#define SV_CL_MSG_JOIN_OK 3
#define SV_CL_MSG_ALL_STATE 4



struct Message {
	uint8_t msgType;
	uint32_t clientId;
	PlayerState state[MAX_PLAYERS];
};