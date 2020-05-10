#include "Application.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <iostream>
#include "generators\SkeletonGenerator.h"
#include "generators\TreeGenerator.h"
#include "imgui\imgui.h"
#include "imgui\imgui-SFML.h"
#include "utils\MathUtil.h"

Application::Application()
{
	srand(time(0));

	initWindowOpenGL();
	ImGui::SFML::Init(m_window);

	m_input.fibStart = 1;
	m_input.iterations = 10;
	m_input.angle = 0.94f;
	m_input.angleDecreaseFactor = 0.97f;
	m_input.displacementAngle = 0.157f;
	m_input.length = 3.7f;
	m_input.lengthDecreaseFactor = 0.97f;
	m_input.sunReach = 0.3f;
	m_input.branchTaper = 0.86f;
	m_input.rotate = 0.f;
	m_input.leafDensity = 2.f;
	m_input.sunAzimuth = 0.f;
	m_input.depthOfField = 0.12f;
	m_input.autoRotate = false;
	m_input.multisampling = false;
	m_input.sceneRotate = { 0.f,10.f,70.f };

	m_treeRenderer.loadResources({ SCR_WIDTH, SCR_HEIGHT });
	m_tree.createNewTree(m_input);
	m_treeRenderer.createDrawable(m_tree, m_input);
}

Application::~Application()
{
	std::cout << "\nApplication destroying...";
}

void Application::run()
{
	sf::Clock clock;

	float accumDT = 0.f;

	while (m_window.isOpen())
	{
		const float dt = clock.restart().asSeconds();
		accumDT += dt;

		m_fps.update(dt);
		ImGui::SFML::Update(m_window, sf::seconds(dt));
		input(dt);
		while (accumDT >= 1.f / 60)
		{
			accumDT -= 1.f / 60;
			update(1.f / 60);
		}
		draw();
	}
	std::cout << "\nEnd of run....";
	glfwTerminate();
	ImGui::SFML::Shutdown();
	std::cout << "\nRun ended!";
}

void Application::input(const float & deltatime)
{
	sf::Event evnt;

	while (m_window.pollEvent(evnt))
	{
		ImGui::SFML::ProcessEvent(evnt);

		switch (evnt.type)
		{
		case sf::Event::Closed:
			m_window.close();
			break;
		case sf::Event::Resized:
		{			
			SCR_WIDTH = evnt.size.width;
			SCR_HEIGHT = evnt.size.height;
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			m_treeRenderer.reloadFramebuffers({ SCR_WIDTH, SCR_HEIGHT });
		}
		break;
		case sf::Event::MouseWheelScrolled:
			m_input.sceneRotate.z += evnt.mouseWheelScroll.delta;
			m_input.sceneRotate.z = std::max(m_input.sceneRotate.z, 0.f);
			break;
		}
	}

	sf::Vector2i currentPos = sf::Mouse::getPosition(m_window);
	bool mouseWithin = false;

	ImGui::Begin("Realtime tweaks", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);

	bool modified = false;

	modified |= ImGui::SliderAngle("Branch Off Angle", &m_input.angle, 0.f, 359.f);
	modified |= ImGui::SliderFloat("Branch Off Angle Decrease", &m_input.angleDecreaseFactor, 0.01f, 0.99f);
	modified |= ImGui::SliderAngle("Straight Growth Angle", &m_input.displacementAngle, 0.f);
	modified |= ImGui::SliderFloat("Branch Length", &m_input.length, 0.1f, 100.f);
	modified |= ImGui::SliderFloat("Branch Length Decrease", &m_input.lengthDecreaseFactor, 0.01f, 0.99f);
	modified |= ImGui::SliderFloat("Sun Reach", &m_input.sunReach, 0.f, 0.9f);

	if (modified)
		m_tree.updateExistingTree(m_input);

	if (ImGui::SliderFloat("Branch Taper", &m_input.branchTaper, 0.8f, 1.f)
		|| modified)
		m_treeRenderer.updateBranchesDrawable(m_tree, m_input);

	ImGui::SliderFloat("Rotate", &m_input.rotate, -10.f, 10.f);

	if (ImGui::SliderFloat("Leaf Density", &m_input.leafDensity, 0.1f, 5.f)
		|| modified)
		m_treeRenderer.updateLeavesDrawable(m_tree, m_input);

	ImGui::SliderFloat("Sun angle", &m_input.sunAzimuth, -10.f, 10.f);
	ImGui::SliderFloat("Depth of Field Aperture", &m_input.depthOfField, 0.f, 1.f);
	ImGui::Checkbox("Auto-Rotate", &m_input.autoRotate);
	ImGui::Checkbox("Multisampling", &m_input.multisampling);

	namespace mu = MathUtil;

	sf::Vector2f winPos = ImGui::GetWindowPos();
	sf::Vector2f winSize = ImGui::GetWindowSize();
	mouseWithin |= sf::FloatRect(winPos, winSize).contains(sf::Vector2f(currentPos));

	ImVec2 nextWindowPos(winPos.x, winPos.y + winSize.y);

	ImGui::End();

	ImGui::SetNextWindowPos(nextWindowPos);

	ImGui::Begin("Generate new tree", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::InputInt("Fibonacci Offset", &m_input.fibStart);
	ImGui::InputInt("Fibonacci Iterations", &m_input.iterations);
	if (ImGui::Button("Generate!"))
	{
		m_tree.createNewTree(m_input);
		m_treeRenderer.createDrawable(m_tree, m_input);
	}

	winPos = ImGui::GetWindowPos();
	winSize = ImGui::GetWindowSize();
	mouseWithin |= sf::FloatRect(winPos, winSize).contains(sf::Vector2f(currentPos));

	ImGui::End();

	static sf::Vector2i lastMousePos = sf::Mouse::getPosition(m_window);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouseWithin)
	{
		sf::Vector2f rotate(currentPos - lastMousePos);
		m_input.sceneRotate += glm::vec3(rotate.x, rotate.y, 0.f) * 0.1f;
	}
	//m_input.sceneRotate.y = std::min(PI / 2 - 0.1f, std::max(m_input.sceneRotate.y, 0.f));

	lastMousePos = currentPos;
}

void Application::update(const float & deltatime)
{
	float theta = m_input.sceneRotate.x + m_input.rotate;
	float azimuth = m_input.sunAzimuth;
	if (m_input.autoRotate)
	{
		theta += m_clock.getElapsedTime().asSeconds() / 10;
		azimuth += m_clock.getElapsedTime().asSeconds() / 5;
	}
	//float cosY = std::cos(m_input.sceneRotate.y);
	float radius = m_input.sceneRotate.z;
	m_camera.setPos({ radius * std::cos(theta), 20.f, radius * std::sin(theta) });
	m_camera.setFocusPos({ 0.f, m_input.sceneRotate.y, 0.f });
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 300.f);
	projection = glm::translate(projection, { -10.f/*(float)SCR_HEIGHT / 2 - (float)SCR_WIDTH / 2*/,0.f,0.f });
	m_camera.setProjection(projection);

	m_input.sunPos = MathUtil::getSunPos(azimuth);
}

void Application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_treeRenderer.draw(m_camera, m_input);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_window.pushGLStates();
	m_window.resetGLStates();

	m_fps.draw(m_window);
	ImGui::SFML::Render();

	m_window.popGLStates();

	m_window.display();
}

void Application::initWindowOpenGL()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.depthBits = 24;
	settings.attributeFlags |= sf::ContextSettings::Debug;
	settings.stencilBits = 8;
	settings.sRgbCapable = true;
	m_window.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "Fibonacci Spring Tree", sf::Style::Default, settings);

	if (!gladLoadGL()) exit(-1);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_MULTISAMPLE);

	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
}