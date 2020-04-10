#pragma once
#include <array>
#include "renderers\TreeRenderer.h"
#include "utils\MathUtil.h"
#include "RenderSettings.h"
#include "Camera.h"
#include "Tree.h"
#include "FPS.h"

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

	int SCR_WIDTH = 1200, SCR_HEIGHT = 650;

	sf::Clock m_clock;
	sf::RenderWindow m_window;
	
	RenderSettings m_input;
	Camera m_camera;
	FPS m_fps;
	Tree m_tree;
	TreeRenderer m_treeRenderer;
};