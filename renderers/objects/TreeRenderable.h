#pragma once
#include <SFML\Graphics.hpp>
#include <array>
#include "RenderableObject.h"
#include "..\Mesh.h"

struct RenderSettings;
class Tree;
class Model;

class TreeRenderable : public RenderableObject
{
public:
	TreeRenderable(const RenderSettings &settings);

	void createMesh(const Tree &tree, const RenderSettings &settings);
	void createMesh() override;
	void pushRender(Model &model) const override;

	void updateLeavesMesh(const Tree &tree, const RenderSettings &settings);
	void updateBranchesMesh(const Tree &tree, const RenderSettings &settings);

private:
	Mesh<BasicVertex> m_branches;
	Mesh<LeafVertex> m_leaves;
};