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

	void drawTree(const TreeMesh &mesh, const Camera &camera, const RenderSettings &settings);

private:
	TreeShaders m_shaders;

	unsigned int m_leavesTexture;
};