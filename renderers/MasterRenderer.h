#pragma once
#include "objects\TreeRenderable.h"
#include "SceneRenderer.h"

struct RenderSettings;
class Camera;

class MasterRenderer
{
public:
	void loadResources(const sf::Vector2i &screenDimensions, const RenderSettings &settings);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void createTreeMesh(const Tree &tree, const RenderSettings &settings);

	void updateLeavesMesh(const Tree &tree, const RenderSettings &settings);
	void updateBranchesMesh(const Tree &tree, const RenderSettings &settings);

	void draw(const Camera &camera, const RenderSettings &settings);

private:
	RenderableCollection m_objects;
	std::shared_ptr<TreeRenderable> m_treeRenderable;		// keep tree separate since we have to change mesh often

	SceneRenderer m_sceneRenderer;
};