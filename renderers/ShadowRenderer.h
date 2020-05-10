#pragma once
#include "FBO.h"
#include "Shader.h"

class TreeRenderable;
struct TreeMesh;
struct RenderSettings;
class Camera;

class ShadowRenderer
{
public:
	void loadResources();
	void bindShadowTexture() const;
	const glm::mat4 getLightMVP() const;

	void draw(TreeRenderable &treeRenderable, const Camera &camera, const RenderSettings &settings);

private:

	const glm::mat4 calculateLightMVP(const glm::vec3 sunPosition);

	FBO m_depthBuffer;
	glm::mat4 m_lightMVP;

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