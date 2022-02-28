/*
Simple Linear Predictions
*/


#pragma once
#include "../game/PlayerState.h"
#include "../game/Message.h"


namespace Prediction {


	static uint32_t prediction_level = 0;
	static float predict_dx = -1.0;
	static float predict_dz = -1.0;

	void updateState(PlayerState prev_state[], Message* msg, PlayerState* myState) {

		if (msg) {
			prediction_level = 0;
			for (int i = 0; i < MAX_PLAYERS; i++) {
				prev_state[i] = msg->state[i];
			}
		}
		else {
			prediction_level++;
			assert(prediction_level != 0);
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (prev_state[i].last_input[KEY_W]) {
					prev_state[i].x += predict_dx / prediction_level;
				}
				if (prev_state[i].last_input[KEY_S]) {
					prev_state[i].x -= predict_dx / prediction_level;
				}
				if (prev_state[i].last_input[KEY_D]) {
					prev_state[i].z += predict_dz / prediction_level;
				}
				if (prev_state[i].last_input[KEY_A]) {
					prev_state[i].z -= predict_dz / prediction_level;
				}
			}
		}

		prev_state[myState->clientID] = *myState;



	}

}