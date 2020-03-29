#pragma once
#include "TreeComponentRenderer.h"
#include "BlurRenderer.h"

struct RenderSettings;
struct Camera;
class Tree;

class TreeRenderer
{
public:
	void loadResources(const sf::Vector2i &screenDimensions);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void draw(const Tree &tree, Camera &camera, const RenderSettings &settings);

private:
	TreeComponentRenderer m_componentRenderer;
	BlurRenderer m_blurRenderer;

	sf::Clock m_clock;
};