#pragma once
#include <array>
#include "BlurRenderer.h"
#include "MSFBO.h"
#include "Shader.h"

class TreeRenderable;
struct TreeMesh;
struct RenderSettings;
class Camera;

class SceneRenderer
{
public:

	void loadResources(const sf::Vector2i &screenDimensions);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void draw(TreeRenderable &treeRenderable, const Camera &camera, const RenderSettings &settings);

private:

	sf::Vector2i m_screenDimensions;

	BlurRenderer m_blurRenderer;
	FBO m_interBuffer;		// intermediate buffer
	FBO m_depthBuffer;

	struct {
		unsigned int VAO, VBO;
		std::array<glm::vec3, 6> vertices;
	} m_waterPlane;

	Shader m_bgShader;

	Shader m_waterShader;
	struct {
		int projView;
		int lightMVP;
		int windowSize;
	} m_waterUniforms;

	Shader m_branchShadowShader;
	struct {
		int lightMVP;
	} m_branchShadowUniforms;

	Shader m_leafShadowShader;
	struct {
		int lightMVP;
		int time;
	} m_leafShadowUniforms;
};