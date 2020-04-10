#include "BlurRenderer.h"
#include <glm\gtc\matrix_transform.hpp>
#include "FBO.h"
#include "..\Camera.h"
#include "ScreenQuad.h"

void BlurRenderer::reinstantiate(const sf::Vector2i & dimensions)
{
	m_buffer.rebuild(dimensions);
	m_buffer.attachDepthBuffer();
	m_buffer.attachColorTexture();
}

void BlurRenderer::setOptions(const int & lightrays, const float & aperture)
{
	m_lightRays = lightrays;
	m_aperture = aperture;
}

void BlurRenderer::render(FBO & finalBuffer, const Camera & camera, const std::function<void(const Camera&)>& renderFunc)
{
	glm::vec3 jitterCamRight = glm::normalize(glm::cross(camera.focus - camera.pos, { 0.f,1.f,0.f }));
	glm::vec3 jitterCamUp = glm::normalize(glm::cross(camera.focus - camera.pos, jitterCamRight)); jitterCamUp.y = -jitterCamUp.y;

	for (int i = 0; i < m_lightRays; i++)
	{
		glm::vec3 jitterOffset = m_aperture * (jitterCamRight * std::cos(i * glm::two_pi<float>() / m_lightRays) 
			+ jitterCamUp * std::sin(i * glm::two_pi<float>() / m_lightRays));;

		m_buffer.bindAndClear();

		Camera jitterCam = camera;
		jitterCam.pos += jitterOffset;
		jitterCam.view = glm::lookAt(jitterCam.pos, camera.focus, jitterCamUp);

		renderFunc(jitterCam);

		finalBuffer.bind();
		m_buffer.bindTexture();

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
		glBlendColor(0.f, 0.f, 0.f, 1.f / m_lightRays);

		ScreenQuad::getQuad().draw();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}
