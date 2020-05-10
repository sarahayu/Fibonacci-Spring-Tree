#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <glm\glm.hpp>

struct TexParams
{
	TexParams();					// default values are GL_REPEAT, GL_LINEAR, GL_RGBA, GL_RGBA, and GL_UNSIGNED_BYTE

	unsigned int wrap;				// default GL_REPEAT
	unsigned int filter;			// default GL_LINEAR
	unsigned int internalFormat;	// default GL_RGBA
	unsigned int format;			// default GL_RGBA
	unsigned int type;				// default GL_UNSIGNED_BYTE
};

namespace ShaderUtil
{
	void linkShader(unsigned int &ID, const std::string &vertexFile, const std::string &fragmentFile = "");
	void loadTexture(unsigned int &texture, const std::string &file, const TexParams &params = TexParams());
	void createTexture(unsigned int &texture, const glm::vec2 &size, const TexParams &params = TexParams(), const void *pixels = nullptr);
};