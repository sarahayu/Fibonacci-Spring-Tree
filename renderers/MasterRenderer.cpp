#include "MasterRenderer.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <array>
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\utils\MathUtil.h"
#include "..\Tree.h"

void MasterRenderer::loadResources(const sf::Vector2i & screenDimensions)
{
	m_treeRenderable.loadResources();
	m_sceneRenderer.loadResources(screenDimensions);
}

void MasterRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_sceneRenderer.reloadFramebuffers(screenDimensions);
}

void MasterRenderer::createDrawable(const Tree & tree, const RenderSettings & settings)
{
	m_treeRenderable.createDrawable(tree, settings);
}

void MasterRenderer::updateLeavesDrawable(const Tree & tree, const RenderSettings & settings)
{
	m_treeRenderable.updateLeavesDrawable(tree, settings);
}

void MasterRenderer::updateBranchesDrawable(const Tree & tree, const RenderSettings & settings)
{
	m_treeRenderable.updateBranchesDrawable(tree, settings);
}

void MasterRenderer::draw(const Camera & camera, const RenderSettings & settings)
{
	m_sceneRenderer.draw(m_treeRenderable, camera, settings);
}