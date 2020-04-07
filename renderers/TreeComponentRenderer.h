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

	void setCurrentModel(const glm::mat4 &model);
	void drawTree(const TreeMesh &mesh, const Camera &camera, const RenderSettings &settings);

private:
	TreeShaders m_shaders;

	glm::mat4 m_model;
	unsigned int m_leavesTexture;
};