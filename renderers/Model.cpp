#include "Model.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include "..\utils\MathUtil.h"
#include "..\utils\SimplexNoise.h"
#include "..\utils\ShaderUtil.h"
#include "..\utils\GlobalClock.h"
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\Branch.h"
#include "Mesh.h"

void Model::loadResources()
{
	m_solidShader.loadFromFile("solid-shader");
	m_leavesShader.loadFromFile("leaves-shader");
	m_skyboxShader.loadFromFile("skybox-shader");
	m_waterShader.loadFromFile("water-shader");

	m_solidShader.use();
	m_solidUniforms.projection = m_solidShader.getLocation("projection");
	m_solidUniforms.view = m_solidShader.getLocation("view");
	m_solidUniforms.cameraPos = m_solidShader.getLocation("cameraPos");
	m_solidUniforms.lightSource = m_solidShader.getLocation("lightSource");
	m_solidUniforms.lightMVP = m_solidShader.getLocation("lightMVP");
	m_solidUniforms.useShadows = m_solidShader.getLocation("useShadows");
	m_solidUniforms.useSSAO = m_solidShader.getLocation("useSSAO");
	m_solidUniforms.useLighting = m_solidShader.getLocation("useLighting");
	m_solidShader.setInt(m_solidShader.getLocation("shadowMap"), 0);
	m_solidShader.setInt(m_solidShader.getLocation("ssaoTexture"), 1);

	m_leavesShader.use();
	m_leavesUniforms.projection = m_leavesShader.getLocation("projection");
	m_leavesUniforms.view = m_leavesShader.getLocation("view");
	m_leavesUniforms.cameraPos = m_leavesShader.getLocation("cameraPos");
	m_leavesUniforms.lightSource = m_leavesShader.getLocation("lightSource");
	m_leavesUniforms.time = m_leavesShader.getLocation("time");
	m_leavesUniforms.leafSize = m_leavesShader.getLocation("leafSize");
	m_leavesUniforms.lightMVP = m_leavesShader.getLocation("lightMVP");
	m_leavesUniforms.useShadows = m_leavesShader.getLocation("useShadows");
	m_leavesUniforms.useSSAO = m_leavesShader.getLocation("useSSAO");
	m_leavesUniforms.useLighting = m_leavesShader.getLocation("useLighting");
	m_leavesShader.setInt(m_leavesShader.getLocation("leafTexture"), 0);
	m_leavesShader.setInt(m_leavesShader.getLocation("shadowMap"), 1);
	m_leavesShader.setInt(m_leavesShader.getLocation("ssaoTexture"), 2);

	m_skyboxShader.use();
	m_skyboxUniforms.projection = m_skyboxShader.getLocation("projection");
	m_skyboxUniforms.view = m_skyboxShader.getLocation("view");
	m_skyboxUniforms.sunRotate = m_skyboxShader.getLocation("sunRotate");

	m_waterShader.use();
	m_waterUniforms.projection = m_waterShader.getLocation("projection");
	m_waterUniforms.view = m_waterShader.getLocation("view");
	m_waterUniforms.lightMVP = m_waterShader.getLocation("lightMVP");
	m_waterShader.setInt(m_waterShader.getLocation("screenTexture"), 0);
	m_waterShader.setInt(m_waterShader.getLocation("shadowMap"), 1);

	ShaderUtil::loadTexture(m_leavesTexture, "leaves.png");

	TexParams params; 
	params.wrap = GL_CLAMP_TO_EDGE;
	params.internalFormat = GL_RGB;
	params.format = GL_RGBA;
	ShaderUtil::loadCubeTexture(m_skyboxTexture, { "right.bmp","left.bmp","top.bmp","bottom.bmp","front.bmp","back.bmp" }, params);
}

void Model::setReflectionPass(const bool & isReflectionPass)
{
	m_reflectionPass = isReflectionPass;
}

const bool Model::onReflectionPass() const
{
	return m_reflectionPass;
}

void Model::setShadowInfo(const unsigned int & shadowMap, const glm::mat4 & lightMatrix)
{
	m_shadowMapTexture = shadowMap;
	m_lightMVP = lightMatrix;
}

void Model::setSSAOInfo(const unsigned int & ssaoTex)
{
	m_ssaoTexture = ssaoTex;
}

void Model::setReflectionTextureInfo(const unsigned int & reflTex)
{
	m_reflTexture = reflTex;
}

void Model::pushRender(const Type & meshType, const unsigned int & vao, const unsigned int & indexCount, const bool & cullOnShadow)
{
	switch (meshType)
	{
	case SOLID_MESH:
		m_solidMeshes.push_back({ vao, indexCount, cullOnShadow });
		break;
	case LEAVES_MESH:
		m_leavesMesh = { vao, indexCount, cullOnShadow };
		break;
	case WATER_MESH:
		m_waterMesh = { vao, indexCount, cullOnShadow };
		break;
	case SKYBOX_MESH:
		m_skyboxMesh = { vao, indexCount, cullOnShadow };
		break;
	}
}

void Model::renderBasic(const Type & meshType, const bool &shadowPass)
{
	switch (meshType)
	{
	case SOLID_MESH:
		for (const auto &mesh : m_solidMeshes)
		{
			if (shadowPass && mesh.cullOnShadow)
			{
				glCullFace(GL_FRONT);
				render(mesh);
				glCullFace(GL_BACK);
			}
			else
				render(mesh);
		}
		break;
	case LEAVES_MESH:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_leavesTexture);
		render(m_leavesMesh);
		break;
	case WATER_MESH:
		render(m_waterMesh);
		break;
	case SKYBOX_MESH:
		render(m_skyboxMesh);
		break;
	}
}

void Model::finishRender(const Camera & camera, const RenderSettings & settings)
{
	m_solidShader.use();
	m_solidShader.setMat4(m_solidUniforms.projection, camera.getProjection());
	m_solidShader.setMat4(m_solidUniforms.view, camera.getView());
	m_solidShader.setVec3(m_solidUniforms.cameraPos, camera.getPos());
	m_solidShader.setVec3(m_solidUniforms.lightSource, settings.sunPos);
	m_solidShader.setMat4(m_solidUniforms.lightMVP, m_lightMVP);
	m_solidShader.setInt(m_solidUniforms.useShadows, (int)settings.useShadows);
	m_solidShader.setInt(m_solidUniforms.useSSAO, (int)settings.useSSAO);
	m_solidShader.setInt(m_solidUniforms.useLighting, (int)settings.useLighting);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);

	if (m_reflectionPass)
	{
		glEnable(GL_CLIP_DISTANCE0);
		glCullFace(GL_FRONT);
	}

	renderBasic(SOLID_MESH);

	m_leavesShader.use();
	m_leavesShader.setMat4(m_leavesUniforms.projection, camera.getProjection());
	m_leavesShader.setMat4(m_leavesUniforms.view, camera.getView());
	m_leavesShader.setVec3(m_leavesUniforms.cameraPos, camera.getPos());
	m_leavesShader.setVec3(m_leavesUniforms.lightSource, settings.sunPos);
	m_leavesShader.setFloat(m_leavesUniforms.leafSize, settings.leafDensity);
	m_leavesShader.setFloat(m_leavesUniforms.time, GlobalClock::getClock().getElapsedTime().asSeconds());
	m_leavesShader.setMat4(m_leavesUniforms.lightMVP, m_lightMVP);
	m_leavesShader.setInt(m_leavesUniforms.useShadows, (int)settings.useShadows);
	m_leavesShader.setInt(m_leavesUniforms.useSSAO, (int)settings.useSSAO);
	m_leavesShader.setInt(m_leavesUniforms.useLighting, (int)settings.useLighting);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
	glDisable(GL_CULL_FACE);

	renderBasic(LEAVES_MESH);

	glEnable(GL_CULL_FACE);

	m_skyboxShader.use();
	m_skyboxShader.setMat4(m_skyboxUniforms.projection, camera.getProjection());
	m_skyboxShader.setMat4(m_skyboxUniforms.view, glm::mat3(camera.getView()));
	m_skyboxShader.setMat3(m_skyboxUniforms.sunRotate, glm::rotate(glm::mat4(1.f), settings.trueSunAzimuth + glm::radians(45.f), { 0.f,1.f,0.f }));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CLIP_DISTANCE0);

	renderBasic(SKYBOX_MESH);

	glDepthFunc(GL_LESS);

	// if we're rendering reflection, there's no need to render water plane
	if (!m_reflectionPass)
	{
		m_waterShader.use();
		m_waterShader.setMat4(m_waterUniforms.projection, camera.getProjection());
		m_waterShader.setMat4(m_waterUniforms.view, camera.getView());
		m_waterShader.setMat4(m_waterUniforms.lightMVP, m_lightMVP);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_reflTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
		renderBasic(WATER_MESH);
	}

	glCullFace(GL_BACK);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Model::clear()
{
	m_solidMeshes.clear();
	m_leavesMesh = {};
	m_waterMesh = {};
	m_skyboxMesh = {};
}

void Model::render(const MeshInfo & mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
}