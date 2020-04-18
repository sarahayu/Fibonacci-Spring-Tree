#include "Shader.h"
#include "..\utils\ShaderUtil.h"

void Shader::loadFromFile(const std::string & vertexFile)
{
	loadFromFile(vertexFile, vertexFile);
}

void Shader::loadFromFile(const std::string & vertexFile, const std::string & fragmentFile)
{
	ShaderUtil::linkShader(ID, vertexFile, fragmentFile);
}

void Shader::use()
{
	glUseProgram(ID);
}

const int Shader::getLocation(const std::string & name)
{
	return glGetUniformLocation(ID, name.c_str());
}

void Shader::setMat4(const unsigned int & location, const glm::mat4 & mat)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec3(const unsigned int & location, const glm::vec3 & vec)
{
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::setVec2(const unsigned int & location, const glm::vec2 & vec)
{
	glUniform2f(location, vec.x, vec.y);
}

void Shader::setFloat(const unsigned int & location, const float & value)
{
	glUniform1f(location, value);
}

void Shader::setInt(const unsigned int & location, const int & value)
{
	glUniform1i(location, value);
}
