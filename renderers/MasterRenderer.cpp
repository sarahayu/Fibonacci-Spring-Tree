#include "MasterRenderer.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <array>
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\utils\MathUtil.h"
#include "..\Tree.h"
#include "objects\LandRenderable.h"
#include "objects\SkyboxRenderable.h"
#include "objects\WaterRenderable.h"

void MasterRenderer::loadResources(const sf::Vector2i & screenDimensions, const RenderSettings & settings)
{
	m_sceneRenderer.loadResources(screenDimensions);
	m_treeRenderable = std::make_shared<TreeRenderable>(settings);

	m_objects.objects.push_back(m_treeRenderable);
	m_objects.objects.push_back(std::make_shared<LandRenderable>(settings));
	m_objects.objects.push_back(std::make_shared<SkyboxRenderable>(settings));
	m_objects.objects.push_back(std::make_shared<WaterRenderable>(settings));

	m_objects.createMeshes();
}

void MasterRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_sceneRenderer.reloadFramebuffers(screenDimensions);
}

void MasterRenderer::createTreeMesh(const Tree & tree, const RenderSettings & settings)
{
	m_treeRenderable->createMesh(tree, settings);
}

void MasterRenderer::updateLeavesMesh(const Tree & tree, const RenderSettings & settings)
{
	m_treeRenderable->updateLeavesMesh(tree, settings);
}

void MasterRenderer::updateBranchesMesh(const Tree & tree, const RenderSettings & settings)
{
	m_treeRenderable->updateBranchesMesh(tree, settings);
}

void MasterRenderer::draw(const Camera & camera, const RenderSettings & settings)
{
	m_sceneRenderer.draw(m_objects, camera, settings);
}