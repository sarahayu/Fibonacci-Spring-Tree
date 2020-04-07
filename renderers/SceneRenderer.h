#pragma once
#include <array>
#include "FBO.h"
#include "BlurRenderer.h"

class TreeComponentRenderer;
struct TreeMesh;
struct RenderSettings;
struct Camera;

class SceneRenderer
{
public:

	void loadResources(const sf::Vector2i &screenDimensions);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void draw(TreeComponentRenderer &componentRenderer, const TreeMesh &treeMesh, const Camera &camera, const RenderSettings &settings);

private:

	sf::Vector2i m_screenDimensions;

	BlurRenderer m_blurRenderer;
	FBO m_accumBuffer;
	unsigned int m_bgShader;
	unsigned int m_waterShader;
};