#pragma once
#include "TreeComponentRenderer.h"
#include "SceneRenderer.h"
#include "Mesh.h"

struct RenderSettings;
struct Camera;
class Tree;

class TreeRenderer
{
public:
	void loadResources(const sf::Vector2i &screenDimensions);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void createDrawable(const Tree &tree, const RenderSettings &settings);

	void updateLeavesDrawable(const Tree &tree, const RenderSettings &settings);
	void updateBranchesDrawable(const Tree &tree, const RenderSettings &settings);

	void draw(const Camera &camera, const RenderSettings &settings);

private:

	TreeComponentRenderer m_componentRenderer;
	SceneRenderer m_blurRenderer;

	TreeMesh m_mesh;

	sf::Clock m_clock;
};