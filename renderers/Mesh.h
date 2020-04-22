#pragma once
#include <glm\glm.hpp>

struct LeafVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
	float yCenter;
};

struct BranchVertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

template <class T>
struct Mesh
{
	std::vector<T> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::mat4> models;
	int instances;
	unsigned int VAO, VBO, modelVBO, EBO;
};

struct TreeMesh
{
	Mesh<LeafVertex> leaves;
	Mesh<BranchVertex> branches;
};