#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Map
{	
public:
	void spawnPlayer(glm::vec3 spawn_point);
	void init();
	void renderMap(GLuint uniformModel, bool wallHackOn);

private:
	Mesh* floor;
	Mesh* wall;
	Texture brickTexture;
	Texture dirtTexture;
	Texture plainTexture;
};

