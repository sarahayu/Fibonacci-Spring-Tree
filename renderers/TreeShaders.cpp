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
	m_treeShader.u_model = glGetUniformLocation(m_treeShader.ID, "model");
	m_treeShader.u_projView = glGetUniformLocation(m_treeShader.ID, "projView");
	m_treeShader.u_cameraPos = glGetUniformLocation(m_treeShader.ID, "cameraPos");
	m_treeShader.u_lightSource = glGetUniformLocation(m_treeShader.ID, "lightSource");

	glUseProgram(m_leafShader.ID);
	m_leafShader.u_model = glGetUniformLocation(m_leafShader.ID, "model");
	m_leafShader.u_projView = glGetUniformLocation(m_leafShader.ID, "projView");
	m_leafShader.u_cameraPos = glGetUniformLocation(m_leafShader.ID, "cameraPos");
	m_leafShader.u_lightSource = glGetUniformLocation(m_leafShader.ID, "lightSource");
	m_leafShader.u_time = glGetUniformLocation(m_leafShader.ID, "time");
	m_leafShader.u_leafSize = glGetUniformLocation(m_leafShader.ID, "leafSize");
}

void TreeShaders::prepareBranchDraw(const Camera & camera, const sf::Vector3f & lightSource)
{
	glUseProgram(m_treeShader.ID);
	glm::mat4 projView = camera.projection * camera.view;
	glUniformMatrix4fv(m_treeShader.u_projView, 1, GL_FALSE, &projView[0][0]);
	glUniform3f(m_treeShader.u_cameraPos, camera.pos.x, camera.pos.y, camera.pos.z);
	glUniform3f(m_treeShader.u_lightSource, lightSource.x, lightSource.y, lightSource.z);
}

void TreeShaders::prepareLeavesDraw(const Camera & camera, const sf::Vector3f & lightSource, const float & leafSize)
{
	glUseProgram(m_leafShader.ID);
	glm::mat4 projView = camera.projection * camera.view;
	glUniformMatrix4fv(m_leafShader.u_projView, 1, GL_FALSE, &projView[0][0]);
	glUniform3f(m_leafShader.u_cameraPos, camera.pos.x, camera.pos.y, camera.pos.z);
	glUniform3f(m_leafShader.u_lightSource, lightSource.x, lightSource.y, lightSource.z);
	glUniform1f(m_leafShader.u_leafSize, leafSize);
}

void TreeShaders::setBranchModel(const glm::mat4 & model)
{
	glUniformMatrix4fv(m_treeShader.u_model, 1, GL_FALSE, &model[0][0]);
}

void TreeShaders::setLeavesModel(const glm::mat4 & model)
{
	glUniformMatrix4fv(m_leafShader.u_model, 1, GL_FALSE, &model[0][0]);
}

void TreeShaders::setLeavesTime(const float & elapsed)
{
	glUniform1f(m_leafShader.u_time, elapsed);
}
