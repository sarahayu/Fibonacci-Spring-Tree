#pragma once
#include <vector>
#include "Shader.h"

class Camera;
struct RenderSettings;

class Model
{
public:
	enum Type
	{
		SOLID_MESH = 0,
		LEAVES_MESH,
		WATER_MESH,
		SKYBOX_MESH
	};

	void loadResources();

	void setReflectionPass(const bool &isReflectionPass);
	const bool onReflectionPass() const;
	void setShadowInfo(const unsigned int &shadowMap, const glm::mat4 &lightMatrix);
	void setSSAOInfo(const unsigned int &ssaoTex);
	void setReflectionTextureInfo(const unsigned int &reflTex);

	void pushRender(const Type &meshType, const unsigned int &vao, const unsigned int &indexCount, const bool &cullOnShadow = false);

	void renderBasic(const Type &meshType, const bool &shadowPass = false);
	void finishRender(const Camera & camera, const RenderSettings & settings);

	void clear();

private:

	struct MeshInfo
	{
		unsigned int vao, indexCount;
		bool cullOnShadow;
	};

	void render(const MeshInfo &mesh);

	std::vector<MeshInfo> m_solidMeshes;
	MeshInfo m_leavesMesh;
	MeshInfo m_waterMesh;
	MeshInfo m_skyboxMesh;

	bool m_reflectionPass = false;

	Shader m_solidShader;
	struct {
		int projection,
			view,
			cameraPos,
			lightSource,
			lightMVP,
			useShadows,
			useSSAO,
			useLighting;
	} m_solidUniforms;

	Shader m_leavesShader;
	struct {
		int projection,
			view,
			cameraPos,
			lightSource,
			time,
			leafSize,
			lightMVP,
			useShadows,
			useSSAO,
			useLighting;
	} m_leavesUniforms;

	Shader m_skyboxShader;
	struct {
		int projection,
			view,
			sunRotate;
	} m_skyboxUniforms;

	Shader m_waterShader;
	struct {
		int projection,
			view,
			lightMVP;
	} m_waterUniforms;

	unsigned int m_leavesTexture;
	unsigned int m_skyboxTexture;

	glm::mat4 m_lightMVP;
	unsigned int m_shadowMapTexture;
	unsigned int m_ssaoTexture;
	unsigned int m_reflTexture;
};