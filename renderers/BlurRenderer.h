#pragma once
#include <functional>
#include <glm\glm.hpp>
#include "MSFBO.h"

class Camera;
struct RenderSettings;

class BlurRenderer
{
public:
	
	void reinstantiate(const sf::Vector2i &dimensions);

	void setOptions(const int &lightrays, const RenderSettings &settings);

	void render(FBO &finalBuffer, const Camera &camera, const std::function<void(const Camera&)> &renderFunc);

private:
	int m_lightRays;
	float m_aperture;
	bool m_ms;
	MSFBO m_buffer;
	FBO m_buffer2;
};