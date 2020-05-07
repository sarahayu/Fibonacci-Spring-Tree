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

namespace
{
	const Camera getReflectionCam(const Camera &camera)
	{
		Camera reflected = camera;
		reflected.setPos(reflected.getPos() * glm::vec3(1.f, -1.f, 1.f));
		reflected.setFocusPos(reflected.getFocusPos() * glm::vec3(1.f, -1.f, 1.f));
		reflected.setView(glm::scale(glm::mat4(1.f), { 1.f,-1.f,1.f }) * reflected.getView());
		return reflected;
		/*reflJitter.pos.y = -reflJitter.pos.y;
		reflJitter.view = glm::lookAt(reflJitter.pos, { camera.focus.x, -camera.focus.y,  camera.focus.z }, { 0.f,1.f,0.f });
		reflJitter.view = glm::scale(glm::mat4(1.f), { 1.f,-1.f,1.f }) * reflJitter.view;*/
	}
}


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

	m_branchShadowShader.loadFromFile("branch-shadow-shader", "shadow-shader");
	m_branchShadowShader.use();
	m_branchShadowUniforms.lightMVP = m_branchShadowShader.getLocation("lightMVP");

	m_leafShadowShader.loadFromFile("leaf-shadow-shader", "shadow-shader");
	m_leafShadowShader.use();
	m_leafShadowUniforms.lightMVP = m_leafShadowShader.getLocation("lightMVP");
	m_leafShadowUniforms.time = m_leafShadowShader.getLocation("time");

	m_depthBuffer.rebuild({ 1024,1024 });
	m_depthBuffer.attachDepthTexture();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	reloadFramebuffers(screenDimensions);
}

void SceneRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_screenDimensions = screenDimensions;

	m_interBuffer.rebuild(screenDimensions);
	m_interBuffer.attachDepthBuffer();
	m_interBuffer.attachColorTexture();

	m_blurRenderer.reinstantiate(screenDimensions);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneRenderer::draw(TreeRenderable & treeRenderable, const Camera & camera, const RenderSettings & settings)
{
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);


	static sf::Clock clock;
	float elapsed = clock.getElapsedTime().asSeconds();
	
	glViewport(0, 0, 1024, 1024);
	m_depthBuffer.bindAndClear();

	glm::mat4 lightOrtho = glm::ortho(-40.f, 40.f, -40.f, 40.f, 1.f, 200.f);
	glm::mat4 lightView = glm::lookAt(settings.sunPos * 40.f, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 lightMVP = lightOrtho * lightView;
	treeRenderable.setShadowInfo(m_depthBuffer.getTextureID(), lightMVP);

	m_branchShadowShader.use();
	m_branchShadowShader.setMat4(m_branchShadowUniforms.lightMVP, lightMVP);

	glBindTexture(GL_TEXTURE_2D, 0);
	treeRenderable.drawBranches();

	m_leafShadowShader.use();
	m_leafShadowShader.setMat4(m_leafShadowUniforms.lightMVP, lightMVP);
	m_leafShadowShader.setFloat(m_leafShadowUniforms.time, treeRenderable.getElapsedTime());

	treeRenderable.drawLeaves();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_screenDimensions.x, m_screenDimensions.y);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	m_bgShader.use();
	ScreenQuad::getQuad().draw(false);

	m_interBuffer.bindAndClear();

	m_blurRenderer.setOptions(3, settings);

	glCullFace(GL_FRONT);
	treeRenderable.drawTree(getReflectionCam(camera), settings);

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	m_depthBuffer.bindTexture();
	glActiveTexture(GL_TEXTURE1);
	m_interBuffer.bindTexture();

	m_waterShader.use();
	m_waterShader.setMat4(m_waterUniforms.projView, camera.getProjView());
	m_waterShader.setVec2(m_waterUniforms.windowSize, castSF2<glm::vec2>((sf::Vector2f)m_screenDimensions));
	m_waterShader.setMat4(m_waterUniforms.lightMVP, lightMVP);

	glBindVertexArray(m_waterPlane.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glActiveTexture(GL_TEXTURE0);
	m_interBuffer.bindAndClear();

	m_blurRenderer.render(m_interBuffer, camera, [&](const Camera &jitterCam) {
		treeRenderable.drawTree(jitterCam, settings);
	});

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_interBuffer.bindTexture();
	ScreenQuad::getQuad().draw();
}
