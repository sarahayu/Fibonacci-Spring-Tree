#include "TreeRenderable.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "..\..\RenderSettings.h"
#include "..\Model.h"
#include "TreeMeshMaker.h"

TreeRenderable::TreeRenderable(const RenderSettings & settings)
	: RenderableObject(settings)
{
}

void TreeRenderable::createMesh(const Tree & tree, const RenderSettings & settings)
{
	updateLeavesMesh(tree, settings);
	updateBranchesMesh(tree, settings);
}

void TreeRenderable::createMesh() {}

void TreeRenderable::pushRender(Model & model) const
{
	model.pushRender(Model::SOLID_MESH, m_branches.VAO, m_branches.indices.size(), true);
	model.pushRender(Model::LEAVES_MESH, m_leaves.VAO, m_leaves.indices.size());
}

void TreeRenderable::updateLeavesMesh(const Tree & tree, const RenderSettings & settings)
{
	TreeMeshMaker::createLeavesMesh(tree, m_leaves, settings);
	MeshHelper::GLsetup(m_leaves);
}

void TreeRenderable::updateBranchesMesh(const Tree & tree, const RenderSettings & settings)
{
	TreeMeshMaker::createBranchesMesh(tree, m_branches, settings);
	MeshHelper::GLsetup(m_branches);
}