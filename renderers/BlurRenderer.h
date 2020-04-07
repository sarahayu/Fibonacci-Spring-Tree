#pragma once
#include <functional>
#include <glm\glm.hpp>
#include "FBO.h"

struct Camera;

class BlurRenderer
{
public:

	void reinstantiate(const sf::Vector2i &dimensions);

	void setOptions(const int &lightrays, const float &aperture);

	void render(FBO &finalBuffer, const Camera &camera, const glm::vec3 &focus, const std::function<void(const Camera&)> &renderFunc);

private:
	int m_lightRays;
	float m_aperture;
	FBO m_buffer;
};