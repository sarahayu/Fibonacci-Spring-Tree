#include "ShaderUtil.h"

void ShaderUtil::linkShader(const std::string & vertexFile, const std::string & fragmentFile, unsigned int & ID)
{
	unsigned int vert, frag;
	int success;
	char infoLog[512];

	load(GL_VERTEX_SHADER, vertexFile, vert);
	load(GL_FRAGMENT_SHADER, fragmentFile, frag);

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
}

void ShaderUtil::load(const GLenum & shaderType, const std::string & file, unsigned int & shader)
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
}