#include "Map.h"


void Map::init()
{

	unsigned int wallIndices[] = {
		0, 1, 3,
		1, 2, 3,

		4, 5, 7,
		5, 6, 7,

		2,3, 7,
		2,6,7,

		1,0,5,
		1,4,5
	};

	GLfloat wallVertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-3.0f, -2.0f, -0.3f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			3.0f, -2.0f, -0.3f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			3.0f, -2.0f, 0.3f,		0.1f, 0.0f,		0.0f, 0.0f, 0.0f,
			-3.0f, -2.0f, 0.3f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

			-3.0f, 10.0f, -0.3f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			3.0f,  10.0f, -0.3f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			3.0f,  10.0f, 0.3f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
			-3.0f, 10.0f, 0.3f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

	};


	unsigned int floorIndices[] = {
	0, 2, 1,
	1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};



	wall = new Mesh();
	wall->CreateMesh(wallVertices, wallIndices, 64, 24);
	floor = new Mesh();
	floor->CreateMesh(floorVertices, floorIndices, 32, 6);

	brickTexture = Texture("graphics/Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("graphics/Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("graphics/Textures/plain.png");
	plainTexture.LoadTextureA();

}

void Map::renderMap(GLuint uniformModel, bool wallHackOn)
{

	glm::mat4 model;


	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	dirtTexture.UseTexture();
	floor->RenderMesh();


	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	if (!wallHackOn) {
		brickTexture.UseTexture();
		wall->RenderMesh();
	}






}
