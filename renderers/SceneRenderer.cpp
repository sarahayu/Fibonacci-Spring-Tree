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
#include "TreeComponentRenderer.h"
#include "ScreenQuad.h"
#include "Mesh.h"

void SceneRenderer::loadResources(const sf::Vector2i & screenDimensions)
{
	reloadFramebuffers(screenDimensions);
}

void SceneRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_screenDimensions = screenDimensions;

	m_accumBuffer.reinstantiate(screenDimensions);
	m_accumBuffer.attachDepthBuffer();
	m_accumBuffer.attachColorTexture();

	m_blurRenderer.reinstantiate(screenDimensions);

	ShaderUtil::linkShader(m_bgShader, "quad-shader", "background-shader");
	ShaderUtil::linkShader(m_waterShader, "quad-shader", "reflection-shader");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneRenderer::draw(TreeComponentRenderer & componentRenderer, const TreeMesh & treeMesh, const Camera & camera, const RenderSettings & settings)
{
	static sf::Clock clock;
	float elapsed = clock.getElapsedTime().asSeconds();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_bgShader);
	ScreenQuad::getQuad().draw(false);

	m_accumBuffer.bindAndClear();

	glm::vec3 focus(0.f, 10.f, 0.f);

	m_blurRenderer.setOptions(4, settings.depthOfField);

	m_blurRenderer.render(m_accumBuffer, camera, focus, [&](const Camera &jitterCam) {
		glCullFace(GL_FRONT);
		Camera reflJitter = jitterCam;
		reflJitter.pos.y = -reflJitter.pos.y;
		reflJitter.view = glm::lookAt(reflJitter.pos, { focus.x, -focus.y, focus.z }, { 0.f,1.f,0.f });
		reflJitter.view = glm::scale(glm::mat4(1.f), { 1.f,-1.f,1.f }) * reflJitter.view;
		componentRenderer.drawTree(treeMesh, reflJitter, settings);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_accumBuffer.bindTexture();
	glUseProgram(m_waterShader);
	ScreenQuad::getQuad().draw(false);

	m_accumBuffer.bindAndClear();

	m_blurRenderer.render(m_accumBuffer, camera, focus, [&](const Camera &jitterCam) {
		glCullFace(GL_BACK);
		componentRenderer.drawTree(treeMesh, jitterCam, settings);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_accumBuffer.bindTexture();
	ScreenQuad::getQuad().draw();

}
