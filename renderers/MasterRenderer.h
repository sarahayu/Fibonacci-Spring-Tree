#pragma once
#include "TreeRenderable.h"
#include "SceneRenderer.h"

struct RenderSettings;
class Camera;

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

	TreeRenderable m_treeRenderable;
	SceneRenderer m_sceneRenderer;
};