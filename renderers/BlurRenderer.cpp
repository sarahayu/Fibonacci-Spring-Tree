#include "BlurRenderer.h"
#include <glm\gtc\matrix_transform.hpp>
#include "FBO.h"
#include "..\Camera.h"
#include "..\utils\MathUtil.h"
#include "ScreenQuad.h"
#include "..\RenderSettings.h"

void BlurRenderer::reinstantiate(const sf::Vector2i & dimensions)
{
	m_buffer.rebuild(dimensions);
	m_buffer.attachDepthBuffer();
	m_buffer.attachColorTexture();
	m_buffer2.rebuild(dimensions);
	m_buffer2.attachDepthBuffer();
	m_buffer2.attachColorTexture();
}

void BlurRenderer::setOptions(const int & lightrays, const RenderSettings &settings)
{
	m_lightRays = lightrays;
	m_aperture = settings.depthOfField;
	m_ms = settings.multisampling;
}

void BlurRenderer::render(FBO & finalBuffer, const Camera & camera, const std::function<void(const Camera&)>& renderFunc)
{
	glm::vec3 jitterCamRight = glm::normalize(glm::cross(camera.getFocusPos() - camera.getPos(), { 0.f,1.f,0.f }));
	glm::vec3 jitterCamUp = glm::normalize(glm::cross(camera.getFocusPos() - camera.getPos(), jitterCamRight)); jitterCamUp.y = -jitterCamUp.y;

	for (int i = 0; i < m_lightRays; i++)
	{
		glm::vec3 jitterOffset = m_aperture * (jitterCamRight * std::cos(i * TWO_PI / m_lightRays) 
			+ jitterCamUp * std::sin(i * TWO_PI / m_lightRays));;

		if (m_ms) m_buffer.bindAndClear();
		else m_buffer2.bindAndClear();

		Camera jitterCam = camera;
		jitterCam.setPos(jitterCam.getPos() + jitterOffset);

		renderFunc(jitterCam);

		if (m_ms)
		{
			m_buffer.blitTexture();
			finalBuffer.bind();
			m_buffer.bindTexture();
		}
		else
		{
			finalBuffer.bind();
			m_buffer2.bindTexture();
		}

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
