#pragma once

struct Camera
{
	glm::vec3 pos;
	glm::vec3 focus;
	glm::mat4 view;
	glm::mat4 projection;
};