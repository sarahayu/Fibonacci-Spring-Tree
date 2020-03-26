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
	ShaderUtil::linkShader("resources/shaders/tree-shader.vs", "resources/shaders/tree-shader.fs", m_treeShader.ID);
	std::cout << "\nTree shader successfully loaded";
	ShaderUtil::linkShader("resources/shaders/leaf-shader.vs", "resources/shaders/leaf-shader.fs", m_leafShader.ID);
	std::cout << "\nLeaf shader successfully loaded";

	glUseProgram(m_treeShader.ID);
	m_treeShader.u_model = glGetUniformLocation(m_treeShader.ID, "model");
	m_treeShader.u_view = glGetUniformLocation(m_treeShader.ID, "view");
	m_treeShader.u_projection = glGetUniformLocation(m_treeShader.ID, "projection");
	m_treeShader.u_cameraPos = glGetUniformLocation(m_treeShader.ID, "cameraPos");
	m_treeShader.u_lightSource = glGetUniformLocation(m_treeShader.ID, "lightSource");

	glUseProgram(m_leafShader.ID);
	m_leafShader.u_localModel = glGetUniformLocation(m_leafShader.ID, "localModel");
	m_leafShader.u_globalModel = glGetUniformLocation(m_leafShader.ID, "globalModel");
	m_leafShader.u_view = glGetUniformLocation(m_leafShader.ID, "view");
	m_leafShader.u_projection = glGetUniformLocation(m_leafShader.ID, "projection");
	m_leafShader.u_time = glGetUniformLocation(m_leafShader.ID, "time");
}

void TreeShaders::prepareBranchDraw(Camera & camera, const sf::Vector3f & lightSource)
{
	glUseProgram(m_treeShader.ID);
	glUniformMatrix4fv(m_treeShader.u_view, 1, GL_FALSE, &camera.view[0][0]);
	glUniformMatrix4fv(m_treeShader.u_projection, 1, GL_FALSE, &camera.projection[0][0]);
	glUniform3f(m_treeShader.u_cameraPos, camera.pos.x, camera.pos.y, camera.pos.z);
	glUniform3f(m_treeShader.u_lightSource, lightSource.x, lightSource.y, lightSource.z);
}

void TreeShaders::prepareLeavesDraw(Camera & camera)
{
	glUseProgram(m_leafShader.ID);
	glUniformMatrix4fv(m_leafShader.u_view, 1, GL_FALSE, &camera.view[0][0]);
	glUniformMatrix4fv(m_leafShader.u_projection, 1, GL_FALSE, &camera.projection[0][0]);
}

void TreeShaders::setBranchModel(const glm::mat4 & model)
{
	glUniformMatrix4fv(m_treeShader.u_model, 1, GL_FALSE, &model[0][0]);
}

void TreeShaders::setLeavesLocalModel(const glm::mat4 & local)
{
	glUniformMatrix4fv(m_leafShader.u_localModel, 1, GL_FALSE, &local[0][0]);
}

void TreeShaders::setLeavesGlobalModel(const glm::mat4 & global)
{
	glUniformMatrix4fv(m_leafShader.u_globalModel, 1, GL_FALSE, &global[0][0]);
}

void TreeShaders::setLeavesTime(const float & elapsed)
{
	glUniform1f(m_leafShader.u_time, elapsed);
}
