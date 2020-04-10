#pragma once
#include <SFML\Graphics.hpp>
#include <array>
#include "TreeShaders.h"
#include "..\Camera.h"

struct TreeMesh;
struct Camera;
struct RenderSettings;

class TreeComponentRenderer
{
public:
	void loadResources();

	void setShadowInfo(const unsigned int &shadowMap, const glm::mat4 &lightMatrix);

	void drawTreeRaw(const TreeMesh &mesh);
	void drawTree(const TreeMesh &mesh, const Camera &camera, const RenderSettings &settings);

private:

	void drawBranches(const TreeMesh &mesh);
	void drawLeaves(const TreeMesh &mesh);

	TreeShaders m_shaders;

	unsigned int m_leavesTexture;
	unsigned int m_shadowMapTexture;
	glm::mat4 m_lightMVP;
};