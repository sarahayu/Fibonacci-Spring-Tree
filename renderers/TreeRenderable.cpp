#include "TreeRenderable.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "..\utils\MathUtil.h"
#include "..\utils\ShaderUtil.h"
#include "..\RenderSettings.h"
#include "..\Branch.h"
#include "TreeMeshMaker.h"
#include "Mesh.h"


void TreeRenderable::loadResources()
{
	m_treeShader.loadFromFile("tree-shader");
	m_leafShader.loadFromFile("leaf-shader");

	m_treeShader.use();
	m_treeUniforms.projView = m_treeShader.getLocation("projView");
	m_treeUniforms.cameraPos = m_treeShader.getLocation("cameraPos");
	m_treeUniforms.lightSource = m_treeShader.getLocation("lightSource");
	m_treeUniforms.lightMVP = m_treeShader.getLocation("lightMVP");

	m_leafShader.use();
	m_leafUniforms.projView = m_leafShader.getLocation("projView");
	m_leafUniforms.cameraPos = m_leafShader.getLocation("cameraPos");
	m_leafUniforms.lightSource = m_leafShader.getLocation("lightSource");
	m_leafUniforms.time = m_leafShader.getLocation("time");
	m_leafUniforms.leafSize = m_leafShader.getLocation("leafSize");
	m_leafUniforms.lightMVP = m_leafShader.getLocation("lightMVP");
	m_leafShader.setInt(m_leafShader.getLocation("leafTexture"), 0);
	m_leafShader.setInt(m_leafShader.getLocation("shadowMap"), 1);

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

const float TreeRenderable::getElapsedTime() const
{
	return m_clock.getElapsedTime().asSeconds();
}

void TreeRenderable::drawTreeRaw()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	drawBranches();
	drawLeaves();
}

void TreeRenderable::drawTree(const Camera & camera, const RenderSettings & settings)
{
	m_treeShader.use();
	m_treeShader.setMat4(m_treeUniforms.projView, camera.getProjView());
	m_treeShader.setVec3(m_treeUniforms.cameraPos, camera.getPos());
	m_treeShader.setVec3(m_treeUniforms.lightSource, settings.sunPos);
	m_treeShader.setMat4(m_treeUniforms.lightMVP, m_lightMVP);

	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	drawBranches();

	m_leafShader.use();
	m_treeShader.setMat4(m_leafUniforms.projView, camera.getProjView());
	m_treeShader.setVec3(m_leafUniforms.cameraPos, camera.getPos());
	m_treeShader.setVec3(m_leafUniforms.lightSource, settings.sunPos);
	m_treeShader.setFloat(m_leafUniforms.leafSize, settings.leafDensity);
	m_treeShader.setFloat(m_leafUniforms.time, m_clock.getElapsedTime().asSeconds());
	m_treeShader.setMat4(m_leafUniforms.lightMVP, m_lightMVP);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glDisable(GL_CULL_FACE);
	drawLeaves();

	glEnable(GL_CULL_FACE);
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
