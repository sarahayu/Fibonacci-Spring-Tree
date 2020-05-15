#pragma once
#include "FBO.h"
#include "Shader.h"

class TreeRenderable;
struct RenderSettings;
class Camera;

class SSAORenderer
{
public:

	void loadResources();
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);
	void clear();

	void bindSSAOTexture() const;

	void draw(TreeRenderable &treeRenderable, const Camera &camera, const RenderSettings &settings);

private:

	std::vector<glm::vec3> m_ssaoKernel;
	sf::Vector2i m_screenDimensions;

	unsigned int m_noiseTex;
	FBO m_geometryFBO;
	FBO m_ssaoFBO;
	FBO m_blurFBO;
	struct {
		unsigned int view,
			invView,
			projection;
	} m_branchesUniforms;
	Shader m_branchesGeomShader;
	struct {
		unsigned int view,
			invView,
			projection,
			time;
	} m_leavesUniforms;
	Shader m_leavesGeomShader;
	struct {
		unsigned int samples,
			projection,
			windowSize;
	} m_ssaoUniforms;
	Shader m_ssaoShader;
	Shader m_blurShader;
};