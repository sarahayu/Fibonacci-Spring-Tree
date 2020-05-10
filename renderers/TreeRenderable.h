#pragma once
#include <SFML\Graphics.hpp>
#include <array>
#include "..\Camera.h"
#include "Shader.h"
#include "Mesh.h"

class Camera;
struct RenderSettings;
class Tree;

class TreeRenderable
{
public:

	void loadResources();

	void createDrawable(const Tree &tree, const RenderSettings &settings);

	void updateLeavesDrawable(const Tree &tree, const RenderSettings &settings);
	void updateBranchesDrawable(const Tree &tree, const RenderSettings &settings);

	void setShadowInfo(const unsigned int &shadowMap, const glm::mat4 &lightMatrix);
	void setSSAOInfo(const unsigned int &ssaoTex);

	void drawBranches();
	void drawLeaves();
	void drawTreeRaw();
	void drawTree(const Camera &camera, const RenderSettings &settings);

private:

	TreeMesh m_mesh;

	Shader m_treeShader;
	struct {
		int projView;
		int cameraPos;
		int lightSource;
		int lightMVP;
	} m_treeUniforms;

	Shader m_leafShader;
	struct {
		int projView;
		int cameraPos;
		int lightSource;
		int time;
		int leafSize;
		int lightMVP;
	} m_leafUniforms;

	unsigned int m_leavesTexture;
	unsigned int m_shadowMapTexture;
	unsigned int m_ssaoTexture;
	glm::mat4 m_lightMVP;
};