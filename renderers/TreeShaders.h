#pragma once
#include <glm\gtc\matrix_transform.hpp>
#include <SFML\Graphics.hpp>

struct Camera;
struct RenderSettings;

class TreeShaders
{
public:
	void loadResources();

	void prepareBranchDraw(const Camera &camera, const sf::Vector3f &lightSource);
	void prepareLeavesDraw(const Camera &camera, const sf::Vector3f &lightSource, const float &leafSize);

	void setBranchModel(const glm::mat4 &model);
	void setLeavesModel(const glm::mat4 &model);
	void setLeavesTime(const float &elapsed);
private:

	struct {
		unsigned int ID;
		int u_model;
		int u_projView;
		int u_cameraPos;
		int u_lightSource;
	} m_treeShader;

	struct {
		unsigned int ID;
		int u_model;
		int u_projView;
		int u_cameraPos;
		int u_lightSource;
		int u_time;
		int u_leafSize;
	} m_leafShader;
};