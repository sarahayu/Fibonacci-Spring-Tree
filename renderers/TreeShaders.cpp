#include "TreeShaders.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include "..\utils\ShaderUtil.h"
#include "..\Camera.h"

void TreeShaders::loadResources()
{
	ShaderUtil::linkShader(m_treeShader.ID, "tree-shader");
	ShaderUtil::linkShader(m_leafShader.ID, "leaf-shader");
	std::cout << "\nLeaf shader successfully loaded";

	glUseProgram(m_treeShader.ID);
	m_treeShader.projView = glGetUniformLocation(m_treeShader.ID, "projView");
	m_treeShader.cameraPos = glGetUniformLocation(m_treeShader.ID, "cameraPos");
	m_treeShader.lightSource = glGetUniformLocation(m_treeShader.ID, "lightSource");
	m_treeShader.lightMVP = glGetUniformLocation(m_treeShader.ID, "lightMVP");

	glUseProgram(m_leafShader.ID);
	m_leafShader.projView = glGetUniformLocation(m_leafShader.ID, "projView");
	m_leafShader.cameraPos = glGetUniformLocation(m_leafShader.ID, "cameraPos");
	m_leafShader.lightSource = glGetUniformLocation(m_leafShader.ID, "lightSource");
	m_leafShader.time = glGetUniformLocation(m_leafShader.ID, "time");
	m_leafShader.leafSize = glGetUniformLocation(m_leafShader.ID, "leafSize");
	m_leafShader.lightMVP = glGetUniformLocation(m_leafShader.ID, "lightMVP");
	glUniform1i(glGetUniformLocation(m_leafShader.ID, "leafTexture"), 0);
	glUniform1i(glGetUniformLocation(m_leafShader.ID, "shadowMap"), 1);
}

void TreeShaders::prepareBranchDraw(const Camera & camera, const sf::Vector3f & lightSource, const glm::mat4 & lightMVP)
{
	glUseProgram(m_treeShader.ID);
	glm::mat4 projView = camera.projection * camera.view;
	glUniformMatrix4fv(m_treeShader.projView, 1, GL_FALSE, &projView[0][0]);
	glUniform3f(m_treeShader.cameraPos, camera.pos.x, camera.pos.y, camera.pos.z);
	glUniform3f(m_treeShader.lightSource, lightSource.x, lightSource.y, lightSource.z);
	glUniformMatrix4fv(m_treeShader.lightMVP, 1, GL_FALSE, &lightMVP[0][0]);
}

void TreeShaders::prepareLeavesDraw(const Camera & camera, const sf::Vector3f & lightSource, const float & leafSize, const float & elapsed, const glm::mat4 & lightMVP)
{
	glUseProgram(m_leafShader.ID);
	glm::mat4 projView = camera.projection * camera.view;
	glUniformMatrix4fv(m_leafShader.projView, 1, GL_FALSE, &projView[0][0]);
	glUniform3f(m_leafShader.cameraPos, camera.pos.x, camera.pos.y, camera.pos.z);
	glUniform3f(m_leafShader.lightSource, lightSource.x, lightSource.y, lightSource.z);
	glUniform1f(m_leafShader.leafSize, leafSize);
	glUniform1f(m_leafShader.time, elapsed);
	glUniformMatrix4fv(m_leafShader.lightMVP, 1, GL_FALSE, &lightMVP[0][0]);
}