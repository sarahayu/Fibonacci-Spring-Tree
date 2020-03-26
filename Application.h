#pragma once
#include <SFML\Graphics.hpp>
#include <glm\glm.hpp>
#include <array>
#include "renderers\TreeRenderer.h"
#include "utils\MathUtil.h"
#include "InputData.h"
#include "Camera.h"
#include "Tree.h"

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

	int SCR_WIDTH = 1000, SCR_HEIGHT = 650;

	sf::Clock m_clock;
	sf::RenderWindow m_window;
	InputData m_input;
	Camera m_camera;
	Tree m_tree;
	TreeRenderer m_treeRenderer;
};