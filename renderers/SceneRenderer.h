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
	FBO m_depthBuffer;

	struct {
		unsigned int VAO, VBO;
		std::array<glm::vec3, 6> vertices;
	} m_waterPlane;

	unsigned int m_bgShader;
	struct {
		unsigned int ID;
		float cameraPitch, screenHeight;
	} m_waterShader;
	struct {
		unsigned int ID;
		float lightMVP;
	} m_shadowShader;
};