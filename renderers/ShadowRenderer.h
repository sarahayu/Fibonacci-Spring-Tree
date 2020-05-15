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
	void clear();
	const glm::mat4 getLightMVP() const;

	void draw(TreeRenderable &treeRenderable, const Camera &camera, const RenderSettings &settings);

private:

	const glm::mat4 calculateLightMVP(const glm::vec3 sunPosition);

	FBO m_depthBuffer;
	glm::mat4 m_lightMVP;

	Shader m_branchesShadowShader;
	struct {
		int lightMVP;
	} m_branchesShadowUniforms;

	Shader m_leavesShadowShader;
	struct {
		int lightMVP;
		int time;
	} m_leavesShadowUniforms;
};