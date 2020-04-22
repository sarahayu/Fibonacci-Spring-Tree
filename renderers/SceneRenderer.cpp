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
#include "TreeRenderable.h"
#include "ScreenQuad.h"
#include "Mesh.h"


void SceneRenderer::loadResources(const sf::Vector2i & screenDimensions)
{
	m_waterPlane.vertices = {
		glm::vec3{ -1000.f, 0.f, 1000.f },
		{ 1000.f, 0.f, -1000.f },
		{ -1000.f, 0.f, -1000.f },
		{ -1000.f, 0.f, 1000.f },
		{ 1000.f, 0.f, 1000.f },
		{ 1000.f, 0.f, -1000.f },
	};

	glGenVertexArrays(1, &m_waterPlane.VAO);
	glGenBuffers(1, &m_waterPlane.VBO);
	glBindVertexArray(m_waterPlane.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_waterPlane.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_waterPlane.vertices.size(), m_waterPlane.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	m_bgShader.loadFromFile("quad-shader", "background-shader");
	m_waterShader.loadFromFile("water-shader");
	m_waterShader.use();
	m_waterUniforms.projView = m_waterShader.getLocation("projView");
	m_waterUniforms.windowSize = m_waterShader.getLocation("windowSize");
	m_waterUniforms.lightMVP = m_waterShader.getLocation("lightMVP");
	m_waterShader.setInt(m_waterShader.getLocation("shadowMap"), 0);
	m_waterShader.setInt(m_waterShader.getLocation("screenTexture"), 1);

	m_shadowShader.loadFromFile("shadow-shader");
	m_shadowShader.use();
	m_shadowUniforms.lightMVP = m_shadowShader.getLocation("lightMVP");

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneRenderer::draw(TreeRenderable & treeRenderable, const Camera & camera, const RenderSettings & settings)
{
	static sf::Clock clock;
	float elapsed = clock.getElapsedTime().asSeconds();
	
	glViewport(0, 0, 1024, 1024);
	m_depthBuffer.bindAndClear();

	glm::mat4 lightOrtho = glm::ortho(-40.f, 40.f, -40.f, 60.f, 1.f, 100.f);
	glm::mat4 lightView = glm::lookAt(castSF3<glm::vec3>(getSunPos(settings.sunAzimuth) * 40.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 lightMVP = lightOrtho * lightView;
	treeRenderable.setShadowInfo(m_depthBuffer.getTextureID(), lightMVP);

	m_shadowShader.use();
	m_shadowShader.setMat4(m_shadowUniforms.lightMVP, lightMVP);

	treeRenderable.drawTreeRaw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_screenDimensions.x, m_screenDimensions.y);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	m_bgShader.use();
	ScreenQuad::getQuad().draw(false);

	m_accumBuffer.bindAndClear();

	m_blurRenderer.setOptions(3, settings.depthOfField, settings.multisampling);

	//m_blurRenderer.render(m_accumBuffer, camera, [&](const Camera &jitterCam) {
	//	glCullFace(GL_FRONT);
	//	Camera reflJitter = jitterCam;
	//	reflJitter.setPos(reflJitter.getPos() * glm::vec3(1.f, -1.f, 1.f));
	//	reflJitter.setFocusPos(reflJitter.getFocusPos() * glm::vec3(1.f, -1.f, 1.f));
	//	reflJitter.setView(glm::scale(glm::mat4(1.f), { 1.f,-1.f,1.f }) * reflJitter.getView());
	//	/*reflJitter.pos.y = -reflJitter.pos.y;
	//	reflJitter.view = glm::lookAt(reflJitter.pos, { camera.focus.x, -camera.focus.y,  camera.focus.z }, { 0.f,1.f,0.f });
	//	reflJitter.view = glm::scale(glm::mat4(1.f), { 1.f,-1.f,1.f }) * reflJitter.view;*/
	//	treeRenderable.drawTree(reflJitter, settings);
	//});

	//glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	m_depthBuffer.bindTexture();
	glActiveTexture(GL_TEXTURE1);
	m_accumBuffer.bindTexture();

	m_waterShader.use();
	m_waterShader.setMat4(m_waterUniforms.projView, camera.getProjView());
	m_waterShader.setVec2(m_waterUniforms.windowSize, castSF2<glm::vec2>((sf::Vector2f)m_screenDimensions));
	m_waterShader.setMat4(m_waterUniforms.lightMVP, lightMVP);

	glBindVertexArray(m_waterPlane.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glActiveTexture(GL_TEXTURE0);
	m_accumBuffer.bindAndClear();

	m_blurRenderer.render(m_accumBuffer, camera, [&](const Camera &jitterCam) {
		treeRenderable.drawTree(jitterCam, settings);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_accumBuffer.bindTexture();
	ScreenQuad::getQuad().draw();
}
