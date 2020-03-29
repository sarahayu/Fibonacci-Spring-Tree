#include "TreeRenderer.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\utils\MathUtil.h"
#include "..\Tree.h"

void TreeRenderer::loadResources(const sf::Vector2i & screenDimensions)
{
	m_componentRenderer.loadResources();
	m_blurRenderer.loadResources(screenDimensions);
}

void TreeRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_blurRenderer.reloadFramebuffers(screenDimensions);
}

void TreeRenderer::draw(const Tree & tree, Camera & camera, const RenderSettings & settings)
{
	m_blurRenderer.draw(m_componentRenderer, tree, camera, settings);
}