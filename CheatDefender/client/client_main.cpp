#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <iostream>

#include <GL\glew.h>
#include <GLFW\glfw3.h>


#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>


#include "Window.h"
#include "../graphics/Mesh.h"
#include "../graphics/Shader.h"
#include "../graphics/Camera.h"
#include "../graphics/Texture.h"
#include "../graphics/Model.h"
#include "../graphics/Map.h"
#include "../net/Network.h"
#include "../game/Message.h"
#include "../game/PlayerState.h"
#include "Predictor.h"
#include "../EnclaveWrapper.h"  /* TODO: in final build move all enclave files to ./enclave folder */

#include <Windows.h>   /* Do not move this to the top */
/*
			y
			|
			|
			|
			|/__________ x
			/
		   /
		  /
		z
*/

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;
Map map;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "graphics/Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "graphics/Shaders/shader.frag";


std::vector<Model> players(MAX_PLAYERS);

PlayerState local_players_state[MAX_PLAYERS];

PlayerState local_state;

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{

	Network net;

	net.init("127.0.0.1", 8866);

	/* Send join request to server and get back client ID */
	Message msg_join;

	msg_join.msgType = CL_SV_MSG_JOIN;
	msg_join.clientId = UNKNOWN_ID;
	msg_join.clientId = 5;
	/* TODO: Error Check */
	net.send(msg_join);

	Message msg = { 0 };
	

	while (net.recv(&msg) <= 0) {
		
	}

	local_state.clientID = msg.clientId;
	local_state.x = -0.0f, local_state.y = 0.0f, local_state.z = 0.0f;



	/* Initilize Enclave and Derive Session Key */

	Message msgKey = { 0 };
	EnclaveWrapper enc_w;
	enc_w.init(msgKey.ecc_pub_key);

	msgKey.msgType = KEY_EXCHANGE;
	msgKey.clientId = local_state.clientID;

	net.send(msgKey);

	msgKey = { 0 };

	while (net.recv(&msgKey) <= 0) {

	}

	enc_w.deriveSharedKey(msgKey.ecc_pub_key);



	/* Start up game cleint window */



	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);


	map.init();

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);



	for (auto& model : players) {
		model.LoadModel("graphics/Models/characterlowpoly2.obj");
	}



	while (!mainWindow.getShouldClose())
	{

		// TODO: Loop according to TickRate

		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		bool* keys = mainWindow.getsKeys();

		camera.keyControl(keys, deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		/* Prepare Message to send to server */
		glm::vec3 myPos = camera.getCameraPosition();
		local_state.x = myPos.x + 0.1f, local_state.y = myPos.y + 0.1f, local_state.z = myPos.z + 0.1f;
		for (int i = 0; i < NUM_KEYS; i++) {
			local_state.last_input[i] = 0;
		}

		if (keys[GLFW_KEY_W])
		{
			local_state.last_input[KEY_W] = 1;
		}

		if (keys[GLFW_KEY_S])
		{
			local_state.last_input[KEY_S] = 1;
		}

		if (keys[GLFW_KEY_A])
		{
			local_state.last_input[KEY_A] = 1;
		}

		if (keys[GLFW_KEY_D])
		{
			local_state.last_input[KEY_D] = 1;
		}

		local_players_state[local_state.clientID] = local_state;

		Message msg;
		msg.msgType = CL_SV_MSG_LOCAL_STATE;
		msg.clientId = local_state.clientID;
		msg.state[msg.clientId] = local_state;

		/* TODO: error check */
		net.send(msg);

		int bytes_recv = net.recv(&msg);

		if (bytes_recv <= 0) {
			Prediction::updateState(local_players_state, nullptr, &local_state);
		}
		else {
			Prediction::updateState(local_players_state, &msg, &local_state);
		}

		//std::cout << "Received state from server";
		
		/*	player.keyControl(mainWindow.getsKeys(), deltaTime);*/

			// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();



		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));


		map.renderMap(uniformModel);

		float dl = 0.0;

		for (int i = 0; i < MAX_PLAYERS; i++) {
			PlayerState& state = local_players_state[i];
			glm::vec3 pos(state.x, state.y, state.z);
			players[i].updatePosition(pos);
		}


		for (int i = 0; i < MAX_PLAYERS; i++) {
			players[i].RenderModel(uniformModel);
		}



		glUseProgram(0);

		mainWindow.swapBuffers();

		Sleep(50);

	}



	return 0;
}

/*
			y
			|
			|
			|
			|/__________ x
			/
		   /
		  /
		z
*/
