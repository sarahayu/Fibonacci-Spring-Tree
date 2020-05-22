#pragma once
#include <glm\glm.hpp>
#include <vector>

template <class T>
struct Mesh
{
	std::vector<T> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO, VBO, EBO;
};

struct LeafVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
	float yCenter;
};

struct BasicVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	unsigned int color;		// color offset in shader
};

namespace MeshHelper
{
	void GLsetup(Mesh<LeafVertex> &mesh);
	void GLsetup(Mesh<BasicVertex> &mesh);
	void GLsetup(Mesh<glm::vec3> &mesh);
}