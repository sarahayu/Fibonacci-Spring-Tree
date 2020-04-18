#pragma once
#include <glm\glm.hpp>
#include <string>

class Shader
{
public:
	void loadFromFile(const std::string &vertexFile);
	void loadFromFile(const std::string &vertexFile, const std::string &fragmentFile);

	void use();
	const int getLocation(const std::string &name);

	void setMat4(const unsigned int &location, const glm::mat4 &mat);
	void setVec3(const unsigned int &location, const glm::vec3 &vec);
	void setVec2(const unsigned int &location, const glm::vec2 &vec);
	void setFloat(const unsigned int &location, const float &value);
	void setInt(const unsigned int &location, const int &value);

private:
	unsigned int ID;
};