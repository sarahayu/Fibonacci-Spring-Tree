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
	m_waterPlane.vertices = {
		glm::vec3{ 500.f, 0.f, 500.f },
		{ -500.f, 0.f, -500.f },
		{ 500.f, 0.f, -500.f },
		{ 500.f, 0.f, 500.f },
		{ -500.f, 0.f, 500.f },
		{ -500.f, 0.f, -500.f },
	};

	glGenVertexArrays(1, &m_waterPlane.VAO);
	glGenBuffers(1, &m_waterPlane.VBO);
	glBindVertexArray(m_waterPlane.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_waterPlane.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_waterPlane.vertices.size(), m_waterPlane.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	m_depthBuffer.rebuild({ 1024,1024 });
	m_depthBuffer.attachDepthTexture();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	reloadFramebuffers(screenDimensions);
}

void SceneRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_screenDimensions = screenDimensions;

	m_accumBuffer.rebuild(screenDimensions);
	m_accumBuffer.attachDepthBuffer();
	m_accumBuffer.attachColorTexture();

	m_blurRenderer.reinstantiate(screenDimensions);

	ShaderUtil::linkShader(m_bgShader, "quad-shader", "background-shader");
	ShaderUtil::linkShader(m_waterShader.ID, "quad-shader", "reflection-shader");
	glUseProgram(m_waterShader.ID);
	m_waterShader.cameraPitch = glGetUniformLocation(m_waterShader.ID, "cameraPitch");
	m_waterShader.screenHeight = glGetUniformLocation(m_waterShader.ID, "screenHeight");
	ShaderUtil::linkShader(m_shadowShader.ID, "shadow-shader", "shadow-shader");
	glUseProgram(m_shadowShader.ID);
	m_shadowShader.lightMVP = glGetUniformLocation(m_shadowShader.ID, "lightMVP");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneRenderer::draw(TreeComponentRenderer & componentRenderer, const TreeMesh & treeMesh, const Camera & camera, const RenderSettings & settings)
{
	static sf::Clock clock;
	float elapsed = clock.getElapsedTime().asSeconds();
	
	glViewport(0, 0, 1024, 1024);
	m_depthBuffer.bindAndClear();

	glm::mat4 lightOrtho = glm::ortho(-40.f, 40.f, -40.f, 60.f, 1.f, 100.f);
	glm::mat4 lightView = glm::lookAt(castSF3<glm::vec3>(getSunPos(settings.sunAzimuth) * 40.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 lightMVP = lightOrtho * lightView;
	componentRenderer.setShadowInfo(m_depthBuffer.getTextureID(), lightMVP);
	glUseProgram(m_shadowShader.ID);
	glUniformMatrix4fv(m_shadowShader.lightMVP, 1, GL_FALSE, &lightMVP[0][0]);

	componentRenderer.drawTreeRaw(treeMesh);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_screenDimensions.x, m_screenDimensions.y);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_bgShader);
	ScreenQuad::getQuad().draw(false);

	m_accumBuffer.bindAndClear();

	m_blurRenderer.setOptions(4, settings.depthOfField);

	m_blurRenderer.render(m_accumBuffer, camera, [&](const Camera &jitterCam) {
		glCullFace(GL_FRONT);
		Camera reflJitter = jitterCam;
		reflJitter.pos.y = -reflJitter.pos.y;
		reflJitter.view = glm::lookAt(reflJitter.pos, { camera.focus.x, -camera.focus.y,  camera.focus.z }, { 0.f,1.f,0.f });
		reflJitter.view = glm::scale(glm::mat4(1.f), { 1.f,-1.f,1.f }) * reflJitter.view;
		componentRenderer.drawTree(treeMesh, reflJitter, settings);
	});

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_accumBuffer.bindTexture();
	glUseProgram(m_waterShader.ID);
	glUniform1f(m_waterShader.cameraPitch, std::atan2f(camera.focus.y, settings.sceneRotate.z));
	glUniform1f(m_waterShader.screenHeight, m_screenDimensions.y);
	ScreenQuad::getQuad().draw(false);

	m_accumBuffer.bindAndClear();

	m_blurRenderer.render(m_accumBuffer, camera, [&](const Camera &jitterCam) {
		componentRenderer.drawTree(treeMesh, jitterCam, settings);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_accumBuffer.bindTexture();
	ScreenQuad::getQuad().draw();
}
