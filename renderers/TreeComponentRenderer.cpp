#include "TreeComponentRenderer.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "..\utils\MathUtil.h"
#include "..\RenderSettings.h"
#include "..\Branch.h"
#include "Mesh.h"

void TreeComponentRenderer::loadResources()
{
	m_shaders.loadResources();

	auto loadTexture = [&](const std::string &file) {

		sf::Image image;
		if (!image.loadFromFile(file)) throw std::runtime_error("Could not load file '" + file + "'!");

		glGenTextures(1, &m_leavesTexture);
		glBindTexture(GL_TEXTURE_2D, m_leavesTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	};

	loadTexture("resources/leaves.png");

}

void TreeComponentRenderer::drawTree(const TreeMesh & mesh, const Camera & camera, const RenderSettings & settings)
{
	sf::Vector3f lightSource = { std::cos(settings.sunAzimuth),0.7f,std::sin(settings.sunAzimuth) };
	m_shaders.prepareBranchDraw(camera, lightSource);
	m_shaders.setBranchModel(glm::mat4(1.f));

	glBindVertexArray(mesh.branches.VAO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glDrawElements(GL_TRIANGLES, mesh.branches.indices.size(), GL_UNSIGNED_INT, 0);

	static sf::Clock clock;
	m_shaders.prepareLeavesDraw(camera, settings.leafDensity);
	m_shaders.setLeavesTime(clock.getElapsedTime().asSeconds());
	m_shaders.setLeavesModel(glm::mat4(1.f));

	glBindTexture(GL_TEXTURE_2D, m_leavesTexture);
	glBindVertexArray(mesh.leaves.VAO);
	glDisable(GL_CULL_FACE);
	glDrawElements(GL_TRIANGLES, mesh.leaves.indices.size(), GL_UNSIGNED_INT, 0);
}