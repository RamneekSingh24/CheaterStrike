#pragma once

#include <vector>
#include <string>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"

#include "Texture.h"


class Model
{

public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel(GLuint uniformModel);
	void ClearModel();
	void updatePosition(glm::vec3 new_position);
	glm::vec3 getPosition();

	~Model();

private:
	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);

	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	std::vector<unsigned int> meshToTex;
	glm::vec3 position;
};

