#include "TreeRenderable.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "..\utils\MathUtil.h"
#include "..\utils\ShaderUtil.h"
#include "..\utils\GlobalClock.h"
#include "..\RenderSettings.h"
#include "..\Branch.h"
#include "TreeMeshMaker.h"
#include "Mesh.h"


void TreeRenderable::loadResources()
{
	m_branchesShader.loadFromFile("branches-shader");
	m_leavesShader.loadFromFile("leaves-shader");

	m_branchesShader.use();
	m_branchesUniforms.projView = m_branchesShader.getLocation("projView");
	m_branchesUniforms.cameraPos = m_branchesShader.getLocation("cameraPos");
	m_branchesUniforms.lightSource = m_branchesShader.getLocation("lightSource");
	m_branchesUniforms.lightMVP = m_branchesShader.getLocation("lightMVP");
	m_branchesUniforms.useShadows = m_branchesShader.getLocation("useShadows");
	m_branchesUniforms.useSSAO = m_branchesShader.getLocation("useSSAO");
	m_branchesUniforms.useLighting = m_branchesShader.getLocation("useLighting");
	m_branchesShader.setInt(m_branchesShader.getLocation("shadowMap"), 0);
	m_branchesShader.setInt(m_branchesShader.getLocation("ssaoTexture"), 1);

	m_leavesShader.use();
	m_leavesUniforms.projView = m_leavesShader.getLocation("projView");
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

	ShaderUtil::loadTexture(m_leavesTexture, "resources/leaves.png");

}

void TreeRenderable::createDrawable(const Tree & tree, const RenderSettings & settings)
{
	updateLeavesDrawable(tree, settings);
	updateBranchesDrawable(tree, settings);
}

void TreeRenderable::updateLeavesDrawable(const Tree & tree, const RenderSettings & settings)
{
	TreeMeshMaker::createLeavesMesh(tree, m_mesh, settings);

	auto &leaves = m_mesh.leaves;

	if (leaves.VAO)
	{
		glDeleteVertexArrays(1, &leaves.VAO);
		glDeleteBuffers(1, &leaves.VBO);
		glDeleteBuffers(1, &leaves.EBO);
	}

	glGenVertexArrays(1, &leaves.VAO);
	glGenBuffers(1, &leaves.VBO);
	glGenBuffers(1, &leaves.EBO);
	glBindVertexArray(leaves.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, leaves.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LeafVertex) * leaves.vertices.size(), leaves.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leaves.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * leaves.indices.size(), leaves.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)(offsetof(LeafVertex, normal)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)(offsetof(LeafVertex, texCoord)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)(offsetof(LeafVertex, yCenter)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TreeRenderable::updateBranchesDrawable(const Tree & tree, const RenderSettings & settings)
{
	TreeMeshMaker::createBranchesMesh(tree, m_mesh, settings);

	auto &branches = m_mesh.branches;

	if (branches.VAO)
	{
		glDeleteVertexArrays(1, &branches.VAO);
		glDeleteBuffers(1, &branches.VBO);
		glDeleteBuffers(1, &branches.EBO);
	}

	glGenVertexArrays(1, &branches.VAO);
	glGenBuffers(1, &branches.VBO);
	glGenBuffers(1, &branches.EBO);
	glBindVertexArray(branches.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, branches.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BranchVertex) * branches.vertices.size(), branches.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, branches.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * branches.indices.size(), branches.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BranchVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BranchVertex), (void*)(offsetof(BranchVertex, normal)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TreeRenderable::setShadowInfo(const unsigned int & shadowMap, const glm::mat4 & lightMatrix)
{
	m_shadowMapTexture = shadowMap;
	m_lightMVP = lightMatrix;
}

void TreeRenderable::setSSAOInfo(const unsigned int & ssaoTex)
{
	m_ssaoTexture = ssaoTex;
}

void TreeRenderable::drawTreeRaw()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	drawBranches();
	drawLeaves();
}

void TreeRenderable::drawTree(const Camera & camera, const RenderSettings & settings)
{
	m_branchesShader.use();
	m_branchesShader.setMat4(m_branchesUniforms.projView, camera.getProjView());
	m_branchesShader.setVec3(m_branchesUniforms.cameraPos, camera.getPos());
	m_branchesShader.setVec3(m_branchesUniforms.lightSource, settings.sunPos);
	m_branchesShader.setMat4(m_branchesUniforms.lightMVP, m_lightMVP);
	m_branchesShader.setInt(m_branchesUniforms.useShadows, (int)settings.useShadows);
	m_branchesShader.setInt(m_branchesUniforms.useSSAO, (int)settings.useSSAO);
	m_branchesShader.setInt(m_branchesUniforms.useLighting, (int)settings.useLighting);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
	drawBranches();

	m_leavesShader.use();
	m_leavesShader.setMat4(m_leavesUniforms.projView, camera.getProjView());
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
	drawLeaves();

	glEnable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TreeRenderable::drawBranches()
{
	glBindVertexArray(m_mesh.branches.VAO);
	glDrawElements(GL_TRIANGLES, m_mesh.branches.indices.size(), GL_UNSIGNED_INT, 0);
}

void TreeRenderable::drawLeaves()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_leavesTexture);
	glBindVertexArray(m_mesh.leaves.VAO);
	glDrawElements(GL_TRIANGLES, m_mesh.leaves.indices.size(), GL_UNSIGNED_INT, 0);
}
