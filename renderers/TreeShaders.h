#pragma once
#include <glm\gtc\matrix_transform.hpp>
#include <SFML\Graphics.hpp>

struct Camera;

class TreeShaders
{
public:
	void loadResources();

	void prepareBranchDraw(Camera &camera, const sf::Vector3f &lightSource);
	void prepareLeavesDraw(Camera &camera);

	void setBranchModel(const glm::mat4 &model);
	void setLeavesLocalModel(const glm::mat4 &local);
	void setLeavesGlobalModel(const glm::mat4 &global);
	void setLeavesTime(const float &elapsed);
private:

	struct {
		unsigned int ID;
		int u_model;
		int u_view;
		int u_projection;
		int u_cameraPos;
		int u_lightSource;
	} m_treeShader;

	struct {
		unsigned int ID;
		int u_localModel;
		int u_globalModel;
		int u_view;
		int u_projection;
		int u_time;
	} m_leafShader;
};