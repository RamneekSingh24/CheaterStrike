#pragma once
#include <stdint.h>
#include "Keys.h"
#define MAX_PLAYERS 10
#define UNKNOWN_ID -1

struct PlayerState {

	float x, y, z;
	uint8_t last_input[NUM_KEYS];
	uint32_t clientID;

	PlayerState() {
		for (int i = 0; i < NUM_KEYS; i++) {
			last_input[i] = false;
		}
		x = -100.0f, y = -100.0f, z = -100.0f;
		clientID = UNKNOWN_ID;
	}

};