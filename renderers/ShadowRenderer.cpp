#include "ShadowRenderer.h"
#include <glm\gtc\matrix_transform.hpp>
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\utils\GlobalClock.h"
#include "Model.h"
#include "Mesh.h"

void ShadowRenderer::loadResources()
{
	m_solidShadowShader.loadFromFile("solid-shadow-shader", "shadow-shader");
	m_solidShadowShader.use();
	m_solidShadowUniforms.lightMVP = m_solidShadowShader.getLocation("lightMVP");

	m_leavesShadowShader.loadFromFile("leaves-shadow-shader", "shadow-shader");
	m_leavesShadowShader.use();
	m_leavesShadowUniforms.lightMVP = m_leavesShadowShader.getLocation("lightMVP");
	m_leavesShadowUniforms.time = m_leavesShadowShader.getLocation("time");

	m_depthBuffer.rebuild({ 2048,2048 });
	m_depthBuffer.attachDepthTexture();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void ShadowRenderer::bindShadowTexture() const
{
	m_depthBuffer.bindTexture(GL_DEPTH_ATTACHMENT);
}

void ShadowRenderer::clear()
{
	m_depthBuffer.bindAndClear();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const glm::mat4 ShadowRenderer::getLightMVP() const
{
	return m_lightMVP;
}

void ShadowRenderer::draw(Model & scene, const Camera & camera, const RenderSettings & settings)
{
	m_lightMVP = calculateLightMVP(settings.sunPos);

	glViewport(0, 0, 2048, 2048);
	m_depthBuffer.bindAndClear();

	scene.setShadowInfo(m_depthBuffer.getTextureID(GL_DEPTH_ATTACHMENT), m_lightMVP);

	m_solidShadowShader.use();
	m_solidShadowShader.setMat4(m_solidShadowUniforms.lightMVP, m_lightMVP);

	glDisable(GL_CULL_FACE);
	scene.renderBasic(Model::SOLID_MESH, true);

	m_leavesShadowShader.use();
	m_leavesShadowShader.setMat4(m_leavesShadowUniforms.lightMVP, m_lightMVP);
	m_leavesShadowShader.setFloat(m_leavesShadowUniforms.time, GlobalClock::getClock().getElapsedTime().asSeconds());

	scene.renderBasic(Model::LEAVES_MESH, true);

	glEnable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const glm::mat4 ShadowRenderer::calculateLightMVP(const glm::vec3 sunPosition)
{
	const float size = 150.f;
	glm::mat4 lightOrtho = glm::ortho(-size, size, -size, size, 1.f, 300.f);
	glm::mat4 lightView = glm::lookAt(sunPosition * 100.f, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

	return lightOrtho * lightView;
}
