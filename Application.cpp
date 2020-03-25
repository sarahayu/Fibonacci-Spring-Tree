#include "Application.h"
#include "imgui\imgui.h"
#include "imgui\imgui-SFML.h"
#include "SkeletonGenerator.h"
#include "TreeGenerator.h"
#include "ShaderUtil.h"
#include <iostream>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>

namespace
{
	const std::pair<int,int> getFibStart(const int &offset)
	{
		int first = 1, second = 1;
		for (int i = 1; i < offset; i++)
		{
			int next = first + second;
			first = second;
			second = next;
		}

		return{first, second};
	}

	void print(TreeSkeleton::Ptr &skelly)
	{
		std::cout << "\n" << skelly->getGeneration() << ": " << skelly->getRotation();
		for (auto &s : skelly->getChildren())
			print(s);
	}

	void toString(TreeSkeleton &skeleton)
	{
		std::cout << "\n" << skeleton.getGeneration() << ": " << skeleton.getRotation();
		for (auto &s : skeleton.getChildren())
			print(s);
	}
}

Application::Application()
	: m_treeObject({ &m_input })
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

	m_treeObject.loadResources();

	auto fibs = getFibStart(m_input.fibStart);
	SkeletonGenerator::generate(m_treeSkeleton, { fibs.first, fibs.second, m_input.iterations });
	TreeGenerator::generate(m_treeSkeleton, m_treeBranches, {
		m_input.angle,
		m_input.angleDecreaseFactor,
		m_input.displacementAngle,
		m_input.length,
		m_input.lengthDecreaseFactor,
		m_input.sunReach
	});
	saveLeafPositions();
}

void Application::run()
{
	sf::Clock clock;

	float accumDT = 0.f;

	while (m_window.isOpen())
	{
		const float dt = clock.restart().asSeconds();
		accumDT += dt;

		ImGui::SFML::Update(m_window, sf::seconds(dt));
		input(dt);
		while (accumDT >= 1.f / 60)
		{
			accumDT -= 1.f / 60;
			update(1.f / 60);
		}
		draw();
	}
/*
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);*/
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
			SCR_WIDTH = evnt.size.width, SCR_HEIGHT = evnt.size.height;
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
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

	if (modified)
		TreeGenerator::generate(m_treeSkeleton, m_treeBranches, {
			m_input.angle,
			m_input.angleDecreaseFactor,
			m_input.displacementAngle,
			m_input.length,
			m_input.lengthDecreaseFactor,
			m_input.sunReach
		});

	ImGui::SliderFloat("Branch Taper", &m_input.branchTaper, 0.f, 0.2f);

	ImGui::SliderFloat("Rotate", &m_input.rotate, -10.f, 10.f);
	if (ImGui::SliderFloat("Leaf Density", &m_input.leafDensity, 0.1f, 5.f) || modified)
		saveLeafPositions();

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
	if (ImGui::Button("Generate!"))
	{
		auto fibs = getFibStart(m_input.fibStart);
		SkeletonGenerator::generate(m_treeSkeleton, { fibs.first, fibs.second, m_input.iterations });
		TreeGenerator::generate(m_treeSkeleton, m_treeBranches, {
			m_input.angle,
			m_input.angleDecreaseFactor,
			m_input.displacementAngle,
			m_input.length,
			m_input.lengthDecreaseFactor,
			m_input.sunReach
		});
		saveLeafPositions();
	}

	ImGui::End();

	const float speed = 3.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		m_focusPoint.z += deltatime * speed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		m_focusPoint.z -= deltatime * speed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		m_focusPoint.x -= deltatime * speed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		m_focusPoint.x += deltatime * speed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		m_focusPoint.y -= deltatime * speed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		m_focusPoint.y += deltatime * speed;
}

void Application::update(const float & deltatime)
{
	float theta = m_input.rotate;
	if (m_input.autoRotate) theta += m_clock.getElapsedTime().asSeconds() / 5;
	m_camera.pos = { -50.0f * std::cos(theta), 0.f, -50.0f * std::sin(theta) };
	m_camera.view = glm::lookAt(m_camera.pos, { 0.f,20.f,0.f }, { 0.f,1.f,0.f });
	m_camera.projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);
}

void Application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static std::array<sf::Vector2f, 6> quadVertices = {
		sf::Vector2f{ -1.f,1.f },
		{ -1.f,-1.f },
		{ 1.f,-1.f },
		{ -1.f,1.f },
		{ 1.f,-1.f },
		{ 1.f,1.f }
	};
	static unsigned int quadVAO, quadVBO;
	static unsigned int quadShader;
	static unsigned int rbA1 = 0, rbA2, fboA = 0, texB = 0, fboB, rbB;

	if (!fboA)
	{
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(sf::Vector2f), quadVertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		ShaderUtil::linkShader("shaders/quad-shader.vs", "shaders/quad-shader.fs", quadShader);
		std::cout << "\nQuad shader successfully loaded!";

		glGenFramebuffers(1, &fboA);
		glBindFramebuffer(GL_FRAMEBUFFER, fboA);

		glGenRenderbuffers(1, &rbA1);
		glBindRenderbuffer(GL_RENDERBUFFER, rbA1);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbA1);

		glGenRenderbuffers(1, &rbA2);
		glBindRenderbuffer(GL_RENDERBUFFER, rbA2);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbA2);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "\nFramebuffer not complete!";
		}


		glGenFramebuffers(1, &fboB);
		glBindFramebuffer(GL_FRAMEBUFFER, fboB);

		glGenTextures(1, &texB);
		glBindTexture(GL_TEXTURE_2D, texB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texB, 0);

		glGenRenderbuffers(1, &rbB);
		glBindRenderbuffer(GL_RENDERBUFFER, rbB);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbB);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "\nFramebuffer not complete!";
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	sf::Vector3f sunPos = { std::cos(m_input.sunAzimuth),0.7f,std::sin(m_input.sunAzimuth) };

	int n = 4;
	glm::vec3 focus(0.f, 20.f, 0.f);

	glm::vec3 jitterCamRight = glm::normalize(glm::cross(focus - m_camera.pos, { 0.f,1.f,0.f }));
	glm::vec3 jitterCamUp = glm::normalize(glm::cross(focus - m_camera.pos, jitterCamRight));
	jitterCamUp.y = -jitterCamUp.y;

	glBindFramebuffer(GL_FRAMEBUFFER, fboA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);

	for (int i = 0; i < n; i++)
	{
		Camera copy = m_camera;
		copy.pos += m_input.depthOfField * (jitterCamRight * std::cos(i * 2 * PI / n) + jitterCamUp * std::sin(i * 2 * PI / n));
		copy.view = glm::lookAt(copy.pos, focus, jitterCamUp);

		glBindFramebuffer(GL_FRAMEBUFFER, fboB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		m_treeObject.prepareBranchDraw(copy, sunPos);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for (const auto &branch : m_treeBranches)
			drawBranch(branch);

		glDisable(GL_CULL_FACE);
		m_treeObject.prepareLeavesDraw(copy);

		m_treeObject.drawLeaves(m_leafPositions);

		glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		glBindTexture(GL_TEXTURE_2D, texB);
		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
		glBlendColor(0.f, 0.f, 0.f, 1.f / n);	
		glEnable(GL_BLEND);

		glEnable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(quadShader);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboA);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_window.pushGLStates();
	m_window.resetGLStates();

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

void Application::saveLeafPositions()
{
	m_leafPositions.clear();
	for (const auto &branch : m_treeBranches)
		if (branch.endNode)
			m_leafPositions.push_back(branch.start + (branch.end - branch.start) / 2.f);
}

void Application::drawBranch(const TreeGenerator::Branch & branch)
{
	m_treeObject.drawBranch(branch.start, branch.rotation, branch.length, branch.generation);
}