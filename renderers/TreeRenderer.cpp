#include "TreeRenderer.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "..\utils\MathUtil.h"
#include "..\RenderSettings.h"
#include "..\Branch.h"
#include "Mesh.h"


void TreeRenderer::loadResources()
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

void TreeRenderer::setShadowInfo(const unsigned int & shadowMap, const glm::mat4 & lightMatrix)
{
	m_shadowMapTexture = shadowMap;
	m_lightMVP = lightMatrix;
}

void TreeRenderer::drawTreeRaw(const TreeMesh & mesh)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	drawBranches(mesh);
	drawLeaves(mesh);
}

void TreeRenderer::drawTree(const TreeMesh & mesh, const Camera & camera, const RenderSettings & settings)
{
	sf::Vector3f lightSource = getSunPos(settings.sunAzimuth);
	m_shaders.prepareBranchDraw(camera, lightSource, m_lightMVP);

	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	drawBranches(mesh);

	static sf::Clock clock;
	m_shaders.prepareLeavesDraw(camera, lightSource, settings.leafDensity, clock.getElapsedTime().asSeconds(), m_lightMVP);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glDisable(GL_CULL_FACE);
	drawLeaves(mesh);

	glEnable(GL_CULL_FACE);
}

void TreeRenderer::drawBranches(const TreeMesh & mesh)
{
	glBindVertexArray(mesh.branches.VAO);
	glDrawElements(GL_TRIANGLES, mesh.branches.indices.size(), GL_UNSIGNED_INT, 0);
}

void TreeRenderer::drawLeaves(const TreeMesh & mesh)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_leavesTexture);
	glBindVertexArray(mesh.leaves.VAO);
	glDrawElements(GL_TRIANGLES, mesh.leaves.indices.size(), GL_UNSIGNED_INT, 0);
}
