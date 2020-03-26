#include "TreeRenderer.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include "..\InputData.h"
#include "..\Camera.h"
#include "..\utils\MathUtil.h"
#include "..\Tree.h"

TreeRenderer::TreeRenderer(const Context & context)
	: m_context(context),
	m_blurRenderer({ context.input, context.scrWidth, context.scrHeight })
{
}

void TreeRenderer::loadResources()
{
	m_componentRenderer.loadResources();
	m_blurRenderer.loadResources();
}

void TreeRenderer::draw(const Tree & tree, Camera & camera, const sf::Vector3f & lightSource)
{
	m_blurRenderer.draw(m_componentRenderer, tree, 
		{ &camera, lightSource, m_context.input->depthOfField, m_context.input->branchTaper, m_context.input->leafDensity });
}