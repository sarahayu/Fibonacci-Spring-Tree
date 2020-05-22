#include "SSAORenderer.h"
#include "..\utils\ShaderUtil.h"
#include "..\utils\MathUtil.h"
#include "..\utils\GlobalClock.h"
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "Model.h"
#include "ScreenQuad.h"
#include <random>
#include <glm\glm.hpp>

namespace
{
	const std::vector<glm::vec3> getSamples()
	{
		std::uniform_real_distribution<float> random(0.f, 1.f);
		std::default_random_engine gen;
		std::vector<glm::vec3> ssaoKernel;
		for (int i = 0; i < 36; i++)
		{
			glm::vec3 sample(random(gen) * 2 - 1, random(gen) * 2 - 1, random(gen));
			sample = glm::normalize(sample);
			float scale = (float)i / 36;
			scale = MathUtil::lerp(0.1f, 1.f, scale * scale);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		return ssaoKernel;
	}

	const std::vector<glm::vec3> getNoise()
	{
		std::uniform_real_distribution<float> random(0.f, 1.f);
		std::default_random_engine gen;
		std::vector<glm::vec3> noise;

		for (int i = 0; i < 16; i++)
		{
			noise.push_back({ random(gen) * 2 - 1, random(gen) * 2 - 1, 0.f });
		}

		return noise;
	}
}

void SSAORenderer::loadResources()
{
	m_ssaoKernel = getSamples();

	const auto noise = getNoise();
	TexParams params;
	params.filter = GL_NEAREST;
	params.type = GL_FLOAT;
	params.internalFormat = GL_RGBA16F;
	params.format = GL_RGB;
	ShaderUtil::createTexture(m_noiseTex, { 4, 4 }, params, noise.data());

	m_solidGeomShader.loadFromFile("solid-geometry-shader", "geometry-shader");
	m_solidGeomShader.use();
	m_solidUniforms.projection = m_solidGeomShader.getLocation("projection");
	m_solidUniforms.view = m_solidGeomShader.getLocation("view");
	m_solidUniforms.invView = m_solidGeomShader.getLocation("invView");

	m_leavesGeomShader.loadFromFile("leaves-geometry-shader", "geometry-shader");
	m_leavesGeomShader.use();
	m_leavesUniforms.projection = m_leavesGeomShader.getLocation("projection");
	m_leavesUniforms.view = m_leavesGeomShader.getLocation("view");
	m_leavesUniforms.time = m_leavesGeomShader.getLocation("time");
	m_leavesUniforms.invView = m_leavesGeomShader.getLocation("invView");

	m_ssaoShader.loadFromFile("quad-shader", "ssao-shader");
	m_ssaoShader.use();
	m_ssaoShader.setInt(m_ssaoShader.getLocation("posTex"), 0);
	m_ssaoShader.setInt(m_ssaoShader.getLocation("normalTex"), 1);
	m_ssaoShader.setInt(m_ssaoShader.getLocation("noiseTex"), 2);

	for (int i = 0; i < m_ssaoKernel.size(); i++)
		m_ssaoShader.setVec3(m_ssaoShader.getLocation("samples[" + std::to_string(i) + "]"), m_ssaoKernel[i]);

	//m_ssaoUniforms.samples = m_ssaoShader.getLocation("samples");
	m_ssaoUniforms.projection = m_ssaoShader.getLocation("projection");
	m_ssaoUniforms.windowSize = m_ssaoShader.getLocation("windowSize");
	m_ssaoUniforms.radius = m_ssaoShader.getLocation("radius");

	m_blurShader.loadFromFile("quad-shader", "blur-shader");
}

void SSAORenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_screenDimensions = screenDimensions;

	TexParams params;
	params.filter = GL_NEAREST;
	params.type = GL_FLOAT;
	params.wrap = GL_CLAMP_TO_EDGE;
	params.internalFormat = GL_RGB16F;
	params.format = GL_RGB;

	m_geometryFBO.rebuild(screenDimensions);
	m_geometryFBO.attachColorTexture(params);
	m_geometryFBO.attachColorTexture(params);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	m_geometryFBO.attachDepthBuffer();

	params.internalFormat = GL_RED;
	params.format = GL_RGB;
	m_ssaoFBO.rebuild(screenDimensions);
	m_ssaoFBO.attachColorTexture(params);

	m_blurFBO.rebuild(screenDimensions);
	m_blurFBO.attachColorTexture(params);
}

void SSAORenderer::clear()
{
	m_blurFBO.bindAndClear();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAORenderer::bindSSAOTexture() const
{
	m_blurFBO.bindTexture(GL_COLOR_ATTACHMENT0);
}

void SSAORenderer::draw(Model & scene, const Camera & camera, const RenderSettings & settings)
{
	m_geometryFBO.bindAndClear();
	m_solidGeomShader.use();
	m_solidGeomShader.setMat4(m_solidUniforms.view, camera.getView());
	m_solidGeomShader.setMat3(m_solidUniforms.invView, camera.getInvView());
	m_solidGeomShader.setMat4(m_solidUniforms.projection, camera.getProjection());
	glEnable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (scene.onReflectionPass())
	{
		glEnable(GL_CLIP_DISTANCE0);
		glCullFace(GL_FRONT);
	}
	scene.renderBasic(Model::SOLID_MESH);

	// disable clipping to avoid z-fighting?
	glDisable(GL_CLIP_DISTANCE0);
	scene.renderBasic(Model::WATER_MESH);
	
	m_leavesGeomShader.use();
	m_leavesGeomShader.setMat4(m_leavesUniforms.view, camera.getView());
	m_leavesGeomShader.setMat3(m_leavesUniforms.invView, camera.getInvView());
	m_leavesGeomShader.setMat4(m_leavesUniforms.projection, camera.getProjection());
	m_leavesGeomShader.setFloat(m_leavesUniforms.time, GlobalClock::getClock() .getElapsedTime().asSeconds());

	glDisable(GL_CULL_FACE);
	if (scene.onReflectionPass()) glEnable(GL_CLIP_DISTANCE0);
	scene.renderBasic(Model::LEAVES_MESH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_CLIP_DISTANCE0);

	m_ssaoFBO.bindAndClear();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	m_geometryFBO.bindTexture(GL_COLOR_ATTACHMENT0);
	glActiveTexture(GL_TEXTURE1);
	m_geometryFBO.bindTexture(GL_COLOR_ATTACHMENT1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_noiseTex);

	m_ssaoShader.use();
	m_ssaoShader.setMat4(m_ssaoUniforms.projection, camera.getProjection());
	m_ssaoShader.setFloat(m_ssaoUniforms.radius, settings.ssaoRadius);
	m_ssaoShader.setVec2(m_ssaoUniforms.windowSize, MathUtil::toGLM2(m_screenDimensions));

	ScreenQuad::getQuad().draw(false);

	m_blurFBO.bindAndClear();

	glActiveTexture(GL_TEXTURE0);
	m_ssaoFBO.bindTexture(GL_COLOR_ATTACHMENT0);
	
	m_blurShader.use();
	ScreenQuad::getQuad().draw(false);

	scene.setSSAOInfo(m_blurFBO.getTextureID(GL_COLOR_ATTACHMENT0));
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
