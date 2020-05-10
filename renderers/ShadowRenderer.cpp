#include "ShadowRenderer.h"
#include <glm\gtc\matrix_transform.hpp>
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\utils\GlobalClock.h"
#include "TreeRenderable.h"
#include "Mesh.h"

void ShadowRenderer::loadResources()
{
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
}

void ShadowRenderer::bindShadowTexture() const
{
	m_depthBuffer.bindTexture(GL_DEPTH_ATTACHMENT);
}

const glm::mat4 ShadowRenderer::getLightMVP() const
{
	return m_lightMVP;
}

void ShadowRenderer::draw(TreeRenderable & treeRenderable, const Camera & camera, const RenderSettings & settings)
{
	m_lightMVP = calculateLightMVP(settings.sunPos);

	glViewport(0, 0, 1024, 1024);
	m_depthBuffer.bindAndClear();

	treeRenderable.setShadowInfo(m_depthBuffer.getTextureID(GL_DEPTH_ATTACHMENT), m_lightMVP);

	m_branchShadowShader.use();
	m_branchShadowShader.setMat4(m_branchShadowUniforms.lightMVP, m_lightMVP);

	treeRenderable.drawBranches();

	m_leafShadowShader.use();
	m_leafShadowShader.setMat4(m_leafShadowUniforms.lightMVP, m_lightMVP);
	m_leafShadowShader.setFloat(m_leafShadowUniforms.time, GlobalClock::getClock().getElapsedTime().asSeconds());

	treeRenderable.drawLeaves();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const glm::mat4 ShadowRenderer::calculateLightMVP(const glm::vec3 sunPosition)
{
	glm::mat4 lightOrtho = glm::ortho(-40.f, 40.f, -40.f, 40.f, 1.f, 200.f);
	glm::mat4 lightView = glm::lookAt(sunPosition * 40.f, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

	return lightOrtho * lightView;
}
