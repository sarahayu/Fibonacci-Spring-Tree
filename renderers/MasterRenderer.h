#pragma once
#include "TreeRenderer.h"
#include "SceneRenderer.h"
#include "Mesh.h"

struct RenderSettings;
struct Camera;
class Tree;

class MasterRenderer
{
public:

	void loadResources(const sf::Vector2i &screenDimensions);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void createDrawable(const Tree &tree, const RenderSettings &settings);

	void updateLeavesDrawable(const Tree &tree, const RenderSettings &settings);
	void updateBranchesDrawable(const Tree &tree, const RenderSettings &settings);

	void draw(const Camera &camera, const RenderSettings &settings);

private:

	TreeRenderer m_componentRenderer;
	SceneRenderer m_blurRenderer;

	TreeMesh m_mesh;

	sf::Clock m_clock;
};