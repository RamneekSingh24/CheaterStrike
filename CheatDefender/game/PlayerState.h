#pragma once
#include <glm/glm.hpp>
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
		x = 0.0f, y = 0.0f, z = 0.0f;
		clientID = UNKNOWN_ID;
	}

};