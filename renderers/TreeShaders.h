#pragma once
#include <glm\gtc\matrix_transform.hpp>
#include <SFML\Graphics.hpp>

struct Camera;
struct RenderSettings;

class TreeShaders
{
public:
	void loadResources();

	void prepareBranchDraw(const Camera &camera, const sf::Vector3f &lightSource, const glm::mat4 &lightMVP);
	void prepareLeavesDraw(const Camera &camera, const sf::Vector3f &lightSource, const float &leafSize, const float &elapsed, const glm::mat4 &lightMVP);
private:

	struct {
		unsigned int ID;
		int projView;
		int cameraPos;
		int lightSource;
		int lightMVP;
	} m_treeShader;

	struct {
		unsigned int ID;
		int projView;
		int cameraPos;
		int lightSource;
		int time;
		int leafSize;
		int lightMVP;
	} m_leafShader;
};