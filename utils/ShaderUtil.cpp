#include "ShaderUtil.h"
#include <SFML\Graphics.hpp>

namespace
{
	void load(const GLenum & shaderType, const std::string & file, unsigned int & shader)
	{
		int success;
		char infoLog[512];

		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::stringstream shaderStream;

		try
		{
			shaderFile.open(file);
			shaderStream << shaderFile.rdbuf();
		}
		catch (...)
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
	}
}

void ShaderUtil::linkShader(unsigned int & ID, const std::string & vertexFile, const std::string & fragmentFile)
{
	unsigned int vert, frag;
	int success;
	char infoLog[512];

	load(GL_VERTEX_SHADER, "resources/shaders/" + vertexFile + ".vs", vert);
	load(GL_FRAGMENT_SHADER, "resources/shaders/" + (fragmentFile == "" ? vertexFile : fragmentFile) + ".fs", frag);

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
	else
	{
		std::cout << "\nShader '" << vertexFile << "' successfully loaded!";
	}
	glDeleteShader(vert);
	glDeleteShader(frag);

}

void ShaderUtil::loadTexture(unsigned int &texture, const std::string & file, const TexParams & params)
{
	sf::Image image;
	if (!image.loadFromFile("resources/" + file)) throw std::runtime_error("Could not load file '" + file + "'!");

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, params.internalFormat, image.getSize().x, image.getSize().y, 0, params.format, params.type, image.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.filter);
}

void ShaderUtil::loadCubeTexture(unsigned int & texture, const std::array<std::string, 6> &faces, const TexParams & params)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	unsigned int offset = 0;
	for (const auto &face : faces)
	{
		sf::Image image;
		if (!image.loadFromFile("resources/" + face)) throw std::runtime_error("Could not load file '" + face + "'!");

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + offset++, 0, params.internalFormat, image.getSize().x, image.getSize().y, 0, params.format, params.type, image.getPixelsPtr());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, params.filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, params.filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, params.wrap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, params.wrap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, params.wrap);
}

void ShaderUtil::createTexture(unsigned int & texture, const glm::vec2 & size, const TexParams & params, const void *pixels)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, params.internalFormat, size.x, size.y, 0, params.format, params.type, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.filter);
}

TexParams::TexParams()
	: wrap(GL_REPEAT), filter(GL_LINEAR), internalFormat(GL_RGBA), format(GL_RGBA), type(GL_UNSIGNED_BYTE)
{
}
