#include "TreeDrawable.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "InputData.h"
#include "Camera.h"
#include "MathUtil.h"

TreeDrawable::TreeDrawable(const Context & context)
{
	m_context = context;
}

void TreeDrawable::loadResources()
{
	m_shaders.loadResources();

	auto loadTexture = [&](const std::string &file) {

		sf::Image image;
		if (!image.loadFromFile(file)) throw std::runtime_error("Could not load file '" + file + "'!");

		glGenTextures(1, &m_leavesDrawable.texture);
		glBindTexture(GL_TEXTURE_2D, m_leavesDrawable.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	};

	loadTexture("leaves.png");

	m_leavesDrawable.vertices = {
		sf::Vector3f{ 0.5f,0.5f,0.f },
		{ 0.5f,-0.5f,0.f },
		{ -0.5f,-0.5f,0.f },
		{ -0.5f,0.5f,0.f } };
	m_leavesDrawable.indices = { 0,3,2,0,2,1 };

	if (m_leavesDrawable.VAO)
	{
		glDeleteVertexArrays(1, &m_leavesDrawable.VAO);
		glDeleteBuffers(1, &m_leavesDrawable.VBO);
		glDeleteBuffers(1, &m_leavesDrawable.EBO);
	}

	glGenVertexArrays(1, &m_leavesDrawable.VAO);
	glGenBuffers(1, &m_leavesDrawable.VBO);
	glGenBuffers(1, &m_leavesDrawable.EBO);
	glBindVertexArray(m_leavesDrawable.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_leavesDrawable.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sf::Vector3f) * m_leavesDrawable.vertices.size(), m_leavesDrawable.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_leavesDrawable.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_leavesDrawable.indices.size(), m_leavesDrawable.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TreeDrawable::prepareBranchDraw(Camera & camera, const sf::Vector3f & lightSource)
{
	static float lastBranchWidth = -1.f;

	if (lastBranchWidth != m_context.input->branchTaper)
	{
		const float PI2 = 3.14159265f * 2;
		int offset = 0;
		float branchTaper = m_context.input->branchTaper + 0.8f;
		float yNormalAngle = std::atanf(1 - branchTaper);

		for (int i = 0; i < CYLINDER_FACE_VERT_COUNT; i++)
		{
			int offset2 = offset++;
			float x = std::cos(PI2 / CYLINDER_FACE_VERT_COUNT * i),
				z = std::sin(PI2 / CYLINDER_FACE_VERT_COUNT * i);
			m_branchDrawable.vertices[offset2 * 2] = { x, 0.f, z };
			m_branchDrawable.vertices[offset2 * 2 + CYLINDER_FACE_VERT_COUNT * 2] = { x * branchTaper, 1.f, z * branchTaper };
			m_branchDrawable.vertices[offset2 * 2 + 1] = rotate({ x,0.f,z }, { 0.f, yNormalAngle });
			m_branchDrawable.vertices[offset2 * 2 + CYLINDER_FACE_VERT_COUNT * 2 + 1] = rotate({ x,0.f,z }, { 0.f, yNormalAngle });
		}

		offset = 0;
		for (int i = 0; i < CYLINDER_FACE_VERT_COUNT; i++)
		{
			bool last = i == CYLINDER_FACE_VERT_COUNT - 1;

			m_branchDrawable.indices[offset++] = i;
			m_branchDrawable.indices[offset++] = i + CYLINDER_FACE_VERT_COUNT;
			m_branchDrawable.indices[offset++] = last ? CYLINDER_FACE_VERT_COUNT : i + CYLINDER_FACE_VERT_COUNT + 1;

			m_branchDrawable.indices[offset++] = i;
			m_branchDrawable.indices[offset++] = last ? CYLINDER_FACE_VERT_COUNT : i + CYLINDER_FACE_VERT_COUNT + 1;
			m_branchDrawable.indices[offset++] = last ? 0 : i + 1;
		}

		if (m_branchDrawable.VAO)
		{
			glDeleteVertexArrays(1, &m_branchDrawable.VAO);
			glDeleteBuffers(1, &m_branchDrawable.VBO);
			glDeleteBuffers(1, &m_branchDrawable.EBO);
		}

		glGenVertexArrays(1, &m_branchDrawable.VAO);
		glGenBuffers(1, &m_branchDrawable.VBO);
		glGenBuffers(1, &m_branchDrawable.EBO);
		glBindVertexArray(m_branchDrawable.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_branchDrawable.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sf::Vector3f) * m_branchDrawable.vertices.size(), m_branchDrawable.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_branchDrawable.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_branchDrawable.indices.size(), m_branchDrawable.indices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(sf::Vector3f), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(sf::Vector3f), (void*)(sizeof(sf::Vector3f)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	m_shaders.prepareBranchDraw(camera, lightSource);
	
	glBindVertexArray(m_branchDrawable.VAO);
}

void TreeDrawable::prepareLeavesDraw(Camera & camera)
{
	m_shaders.prepareLeavesDraw(camera);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_leavesDrawable.texture);
	glBindVertexArray(m_leavesDrawable.VAO);
}

void TreeDrawable::drawBranch(const sf::Vector3f & position, const sf::Vector2f & rotation, const float & length, const int & generation)
{
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, { position.x, position.y, position.z });
	model = glm::rotate(model, -rotation.x, { 0.f,1.f,0.f });
	model = glm::rotate(model, rotation.y, { 0.f,0.f,1.f });
	float shrinkScale = std::pow(m_context.input->branchTaper + 0.8f, generation);
	model = glm::scale(model, { shrinkScale, length * 1.05f, shrinkScale });

	m_shaders.setBranchModel(model);

	glDrawElements(GL_TRIANGLES, m_branchDrawable.indices.size(), GL_UNSIGNED_INT, 0);
}

void TreeDrawable::drawLeaves(const std::vector<sf::Vector3f> & positions)
{
	float faceRotate = 0.f;
	for (const auto &position : positions)
	{
		//glm::mat4 model = glm::translate(glm::mat4(1.f), { position.x,position.y,position.z });
		float density = m_context.input->leafDensity;

		for (int i = 0; i < 3; i++)
		{
			glm::mat4 globalModel = glm::mat4(1.f);
			globalModel = glm::translate(globalModel, { position.x,position.y,position.z });

			glm::mat4 localModel = glm::mat4(1.f);
			localModel = glm::rotate(localModel, glm::radians(i * 60.f), { 0.f,1.f,0.f });
			localModel = glm::rotate(localModel, glm::radians(faceRotate), { 0.f,0.f,1.f });
			localModel = glm::scale(localModel, { density, density, density });
			m_shaders.setLeavesLocalModel(localModel);
			m_shaders.setLeavesGlobalModel(globalModel);
			glDrawElements(GL_TRIANGLES, m_leavesDrawable.indices.size(), GL_UNSIGNED_INT, 0);
			faceRotate += 135.f;
		}
	}
}
