#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>

struct ShaderUtil
{
	static void linkShader(const std::string &vertexFile, const std::string &fragmentFile, unsigned int &ID);
private:
	static void load(const GLenum &shaderType, const std::string &file, unsigned int &shader);
};