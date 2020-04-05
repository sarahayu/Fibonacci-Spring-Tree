#pragma once
#include <glm\glm.hpp>

struct LeafVertex
{
	glm::vec3 position;
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
	unsigned int VAO, VBO, EBO;
};

struct TreeMesh
{
	Mesh<LeafVertex> leaves;
	Mesh<BranchVertex> branches;
};