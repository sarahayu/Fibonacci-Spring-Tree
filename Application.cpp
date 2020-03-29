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

Application::Application()
{
	srand(time(0));

	initWindowOpenGL();
	ImGui::SFML::Init(m_window);

	m_input.fibStart = 1;
	m_input.iterations = 9;
	m_input.angle = 0.94f;
	m_input.angleDecreaseFactor = 0.97f;
	m_input.displacementAngle = 0.26f;
	m_input.length = 3.7f;
	m_input.lengthDecreaseFactor = 0.97f;
	m_input.sunReach = 0.15f;
	m_input.branchTaper = 0.06f;
	m_input.rotate = 0.f;
	m_input.leafDensity = 4.6f;
	m_input.sunAzimuth = 0.f;
	m_input.depthOfField = 0.12f;
	m_input.autoRotate = false;

	m_treeRenderer.loadResources({ SCR_WIDTH, SCR_HEIGHT });
	m_tree.createNewTree(m_input);
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
	glfwTerminate();
	ImGui::SFML::Shutdown();
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
		case sf::Event::KeyReleased:
			break;
		}
	}

	ImGui::Begin("Realtime tweaks", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);

	bool modified = false;

	modified |= ImGui::SliderAngle("Branch Off Angle", &m_input.angle, 0.f, 359.f);
	modified |= ImGui::SliderFloat("Branch Off Angle Decrease", &m_input.angleDecreaseFactor, 0.01f, 0.99f);
	modified |= ImGui::SliderAngle("Straight Growth Angle", &m_input.displacementAngle, 0.f);
	modified |= ImGui::SliderFloat("Branch Length", &m_input.length, 0.1f, 100.f);
	modified |= ImGui::SliderFloat("Branch Length Decrease", &m_input.lengthDecreaseFactor, 0.01f, 0.99f);
	modified |= ImGui::SliderFloat("Sun Reach", &m_input.sunReach, 0.f, 0.9f);

	if (modified) m_tree.updateExistingTree(m_input);

	ImGui::SliderFloat("Branch Taper", &m_input.branchTaper, 0.f, 0.2f);

	ImGui::SliderFloat("Rotate", &m_input.rotate, -10.f, 10.f);
	ImGui::SliderFloat("Leaf Density", &m_input.leafDensity, 0.1f, 5.f);

	ImGui::SliderFloat("Sun angle", &m_input.sunAzimuth, -10.f, 10.f);
	ImGui::SliderFloat("Depth of Field Aperture", &m_input.depthOfField, 0.f, 1.f);
	ImGui::Checkbox("Auto-Rotate", &m_input.autoRotate);

	ImVec2 nextWindowPos = ImGui::GetWindowPos();
	nextWindowPos.y += ImGui::GetWindowSize().y;

	ImGui::End();

	ImGui::SetNextWindowPos(nextWindowPos);

	ImGui::Begin("Generate new tree", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::InputInt("Fibonacci Offset", &m_input.fibStart);
	ImGui::InputInt("Fibonacci Iterations", &m_input.iterations);
	if (ImGui::Button("Generate!")) m_tree.createNewTree(m_input);

	ImGui::End();
}

void Application::update(const float & deltatime)
{
	float theta = m_input.rotate;
	if (m_input.autoRotate) theta += m_clock.getElapsedTime().asSeconds() / 5;
	m_camera.pos = { -50.0f * std::cos(theta), 0.f, -50.0f * std::sin(theta) };
	m_camera.view = glm::lookAt(m_camera.pos, { 0.f,20.f,0.f }, { 0.f,1.f,0.f });
	m_camera.projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);
	m_camera.projection = glm::translate(m_camera.projection, { -10.f/*(float)SCR_HEIGHT / 2 - (float)SCR_WIDTH / 2*/,0.f,0.f });

}

void Application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_treeRenderer.draw(m_tree, m_camera, m_input);

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
	settings.stencilBits = 8;
	settings.sRgbCapable = true;
	m_window.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "Fibonacci Spring Tree", sf::Style::Default, settings);

	if (!gladLoadGL()) exit(-1);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

	//glClearColor(233.f / 255, 159.f / 255, 143.f / 255, 1.f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}