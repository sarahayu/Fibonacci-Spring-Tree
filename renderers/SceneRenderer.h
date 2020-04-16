#pragma once
#include <array>
#include "BlurRenderer.h"
#include "MSFBO.h"

class TreeRenderer;
struct TreeMesh;
struct RenderSettings;
struct Camera;

class SceneRenderer
{
public:

	void loadResources(const sf::Vector2i &screenDimensions);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void draw(TreeRenderer &componentRenderer, const TreeMesh &treeMesh, const Camera &camera, const RenderSettings &settings);

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
		int projView;
		int lightMVP;
		int windowSize;
		int shadowMap;
		int screenTexture;
	} m_waterShader;
	struct {
		unsigned int ID;
		int lightMVP;
	} m_shadowShader;
};