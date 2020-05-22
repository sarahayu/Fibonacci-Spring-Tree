#include "SceneRenderer.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "..\utils\ShaderUtil.h"
#include "..\utils\MathUtil.h"
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\Tree.h"
#include "objects\TreeRenderable.h"
#include "objects\LandRenderable.h"
#include "ScreenQuad.h"
#include "Mesh.h"

namespace
{
	const glm::vec3 WATER_PLANE(0.f, 3.f, 0.f);
	const Camera getReflectionCam(const Camera &camera)
	{
		Camera reflected = camera;
		reflected.setPos((reflected.getPos() - WATER_PLANE) * glm::vec3(1.f, -1.f, 1.f) + WATER_PLANE);
		reflected.setFocusPos((reflected.getFocusPos() - WATER_PLANE) * glm::vec3(1.f, -1.f, 1.f) + WATER_PLANE);
		reflected.setView(glm::scale(glm::mat4(1.f), { 1.f,-1.f,1.f }) * reflected.getView());
		return reflected;
	}
}


void SceneRenderer::loadResources(const sf::Vector2i & screenDimensions)
{
	m_shadowRenderer.loadResources();
	m_ssaoRenderer.loadResources();
	m_sceneModel.loadResources();

	reloadFramebuffers(screenDimensions);
}

void SceneRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_screenDimensions = screenDimensions;

	m_interBuffer.rebuild(screenDimensions);
	m_interBuffer.attachDepthBuffer();
	m_interBuffer.attachColorTexture();
	m_reflectionBuffer.rebuild(screenDimensions);
	m_reflectionBuffer.attachDepthBuffer();
	m_reflectionBuffer.attachColorTexture();

	m_blurRenderer.reloadFramebuffers(screenDimensions);
	m_ssaoRenderer.reloadFramebuffers(screenDimensions);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneRenderer::draw(const RenderableCollection & objects, const Camera & camera, const RenderSettings & settings)
{
	m_sceneModel.clear();
	objects.pushRenders(m_sceneModel);

	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	m_sceneModel.setReflectionPass(true);
	m_blurRenderer.setOptions(3, settings);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderInOrientation(objects, camera, settings, false);

	m_interBuffer.bind(GL_READ_FRAMEBUFFER);
	m_reflectionBuffer.bind(GL_DRAW_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int x = m_screenDimensions.x, y = m_screenDimensions.y;
	glBlitFramebuffer(0, 0, x, y, 0, 0, x, y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	m_sceneModel.setReflectionTextureInfo(m_reflectionBuffer.getTextureID(GL_COLOR_ATTACHMENT0));
	m_sceneModel.setReflectionPass(false);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderInOrientation(objects, camera, settings, true);

	m_interBuffer.bindTexture(GL_COLOR_ATTACHMENT0);
	ScreenQuad::getQuad().draw();
}

void SceneRenderer::renderInOrientation(const RenderableCollection & objects, const Camera & rightSideUpCamera, const RenderSettings & settings, const bool & rightSideUp)
{
	const Camera newCam = rightSideUp ? rightSideUpCamera : getReflectionCam(rightSideUpCamera);

	if (settings.useShadows) m_shadowRenderer.draw(m_sceneModel, newCam, settings);
	else m_shadowRenderer.clear();

	glViewport(0, 0, m_screenDimensions.x, m_screenDimensions.y);

	if (settings.useSSAO) m_ssaoRenderer.draw(m_sceneModel, newCam, settings);
	else m_ssaoRenderer.clear();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	m_interBuffer.bindAndClear();

	m_blurRenderer.render(m_interBuffer, rightSideUpCamera, [&](const Camera &jitterCam) {

		if (settings.lines) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// for reflections, we can't just pass in a reflected camera as a parameter into the blur render function, 
		// so we have to flip it here instead
		m_sceneModel.finishRender(rightSideUp ? jitterCam : getReflectionCam(jitterCam), settings);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	});

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
