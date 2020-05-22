#pragma once
#include <array>
#include "BlurRenderer.h"
#include "ShadowRenderer.h"
#include "SSAORenderer.h"
#include "MSFBO.h"
#include "Shader.h"
#include "Model.h"

class TreeRenderable;
class LandRenderable;
struct RenderableCollection;
struct TreeMesh;
struct RenderSettings;
class Camera;

class SceneRenderer
{
public:
	void loadResources(const sf::Vector2i &screenDimensions);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void draw(const RenderableCollection &objects, const Camera &camera, const RenderSettings &settings);

private:

	void renderInOrientation(const RenderableCollection &objects, const Camera &rightSideUpCamera, const RenderSettings &settings, const bool &rightSideUp);

	sf::Vector2i m_screenDimensions;

	Model m_sceneModel;

	BlurRenderer m_blurRenderer;
	ShadowRenderer m_shadowRenderer;
	SSAORenderer m_ssaoRenderer;

	FBO m_interBuffer;		// intermediate buffer
	FBO m_reflectionBuffer;
};