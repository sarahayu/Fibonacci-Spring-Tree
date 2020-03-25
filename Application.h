#pragma once
#include <SFML\Graphics.hpp>
#include <glm\glm.hpp>
#include <array>
#include "TreeSkeleton.h"
#include "TreeGenerator.h"
#include "MathUtil.h"
#include "InputData.h"
#include "TreeDrawable.h"
#include "Camera.h"

class Application
{
public:
	Application();

	void run();

private:

	void input(const float &deltatime);
	void update(const float &deltatime);
	void draw();

	void initWindowOpenGL();
	void saveLeafPositions();
	void drawBranch(const TreeGenerator::Branch &branch);

	int SCR_WIDTH = 1000, SCR_HEIGHT = 650;

	sf::Vector3f m_focusPoint;

	sf::Clock m_clock;
	sf::RenderWindow m_window;
	TreeSkeleton m_treeSkeleton;
	TreeGenerator::Branches m_treeBranches;
	std::vector<sf::Vector3f> m_leafPositions;
	InputData m_input;
	Camera m_camera;
	TreeDrawable m_treeObject;
};