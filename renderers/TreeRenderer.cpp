#include "TreeRenderer.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <array>
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\utils\MathUtil.h"
#include "..\Tree.h"
#include "TreeMeshMaker.h"


void TreeRenderer::loadResources(const sf::Vector2i & screenDimensions)
{
	m_componentRenderer.loadResources();
	m_blurRenderer.loadResources(screenDimensions);
}

void TreeRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_blurRenderer.reloadFramebuffers(screenDimensions);
}

void TreeRenderer::createDrawable(const Tree & tree, const RenderSettings & settings)
{
	updateLeavesDrawable(tree, settings);
	updateBranchesDrawable(tree, settings);
}

void TreeRenderer::updateLeavesDrawable(const Tree & tree, const RenderSettings & settings)
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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)(offsetof(LeafVertex, texCoord)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(LeafVertex), (void*)(offsetof(LeafVertex, yCenter)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TreeRenderer::updateBranchesDrawable(const Tree & tree, const RenderSettings & settings)
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

void TreeRenderer::draw(Camera & camera, const RenderSettings & settings)
{
	m_blurRenderer.draw(m_componentRenderer, m_mesh, camera, settings);
}