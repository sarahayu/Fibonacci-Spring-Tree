#include "TreeShaders.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Camera.h"

void TreeShaders::loadResources()
{
	unsigned int vert, frag;
	int success;
	char infoLog[512];

	auto load = [&](const GLenum &shaderType, const std::string &file, unsigned int &shader) {
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::stringstream shaderStream;

		try
		{
			shaderFile.open(file);
			shaderStream << shaderFile.rdbuf();
		}
		catch (const std::ifstream::failure &failed)
		{
			std::cout << "\nShader file could not be read!";
		}

		std::string shaderString = shaderStream.str();
		const char *shaderCode = shaderString.c_str();
		const bool isVertex = shaderType == GL_VERTEX_SHADER;
		shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "\n" << (isVertex ? "Vertex" : "Fragment") << " compilation failed! " << infoLog;

		}
	};

	auto linkShader = [&](unsigned int &ID) {
		ID = glCreateProgram();
		glAttachShader(ID, vert);
		glAttachShader(ID, frag);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			std::cout << "\nShader linking failed!";
		}
		glDeleteShader(vert);
		glDeleteShader(frag);
	};

	load(GL_VERTEX_SHADER, "shaders/tree-shader.vs", vert);
	load(GL_FRAGMENT_SHADER, "shaders/tree-shader.fs", frag);
	linkShader(m_treeShader.ID);
	std::cout << "\nTree shader successfully loaded";

	load(GL_VERTEX_SHADER, "shaders/leaf-shader.vs", vert);
	load(GL_FRAGMENT_SHADER, "shaders/leaf-shader.fs", frag);
	linkShader(m_leafShader.ID);
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
	int texture = glGetUniformLocation(m_leafShader.ID, "leafTexture");
	glUniform1i(texture, 0);
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
