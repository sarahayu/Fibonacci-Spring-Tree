#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>

namespace ShaderUtil
{
	void linkShader(unsigned int &ID, const std::string &vertexFile, const std::string &fragmentFile = "");
	void loadTexture(unsigned int &texture, const std::string &file);
};