#include "Mesh.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>

namespace
{
	template <typename T>
	void prepare(Mesh<T> &mesh)
	{
		if (mesh.VAO)
		{
			glDeleteVertexArrays(1, &mesh.VAO);
			glDeleteBuffers(1, &mesh.VBO);
			glDeleteBuffers(1, &mesh.EBO);
		}

		glGenVertexArrays(1, &mesh.VAO);
		glGenBuffers(1, &mesh.VBO);
		glGenBuffers(1, &mesh.EBO);
		glBindVertexArray(mesh.VAO);
	}
}

void MeshHelper::GLsetup(Mesh<LeafVertex>& mesh)
{
	prepare(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LeafVertex) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)(offsetof(LeafVertex, normal)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)(offsetof(LeafVertex, texCoord)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)(offsetof(LeafVertex, yCenter)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MeshHelper::GLsetup(Mesh<BasicVertex>& mesh)
{
	prepare(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BasicVertex) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)(offsetof(BasicVertex, normal)));
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(BasicVertex), (void*)(offsetof(BasicVertex, color)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MeshHelper::GLsetup(Mesh<glm::vec3>& mesh)
{
	prepare(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}