#pragma once
#include <glm\gtc\matrix_transform.hpp>

struct Camera;

class TreeShaders
{
public:
	void loadResources();

	void prepareBranchDraw(Camera &camera);
	void prepareLeavesDraw(Camera &camera);

	void setBranchModel(const glm::mat4 &model);
	void setLeavesModel(const glm::mat4 &model);
private:

	struct {
		unsigned int ID;
		int u_model;
		int u_view;
		int u_projection;
		int u_cameraPos;
	} m_treeShader;

	struct {
		unsigned int ID;
		int u_model;
		int u_view;
		int u_projection;
		int u_cameraPos;
		int u_texture;
	} m_leafShader;
};