#include "Application.h"
#include "imgui\imgui.h"
#include "imgui\imgui-SFML.h"
#include "SkeletonGenerator.h"
#include "TreeGenerator.h"
#include <iostream>

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
	: m_window(sf::VideoMode(800U, 800U), "Fibonacci Spring Tree"),
	m_treeVertices(sf::Lines)
{
	srand(time(0));

	ImGui::SFML::Init(m_window);

	auto fibs = getFibStart(m_input.fibStart);
	SkeletonGenerator::generate(m_treeSkeleton, { fibs.first, fibs.second, m_input.iterations });
	TreeGenerator::generate(m_treeBranchPos, m_treeGenerations, {
		m_treeSkeleton,
		m_input.angle,
		m_input.angleDecreaseFactor,
		m_input.displacementAngle,
		m_input.length,
		m_input.lengthDecreaseFactor
	});
}

void Application::run()
{
	sf::Clock clock;

	while (m_window.isOpen())
	{
		const float dt = clock.restart().asSeconds();

		ImGui::SFML::Update(m_window, sf::seconds(dt));
		input();
		update(dt);
		draw();
	}

	ImGui::SFML::Shutdown();
}

void Application::input()
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
		case sf::Event::KeyReleased:
			break;
		}
	}

	ImGui::Begin("Realtime tweaks", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);

	bool modified = false;

	modified |= ImGui::SliderAngle("Branch Off Angle", &m_input.angle, 0.f, 359.f);
	modified |= ImGui::SliderFloat("Branch Off Angle Decrease", &m_input.angleDecreaseFactor, 0.01f, 0.99f);
	modified |= ImGui::SliderAngle("Straight Growth Angle", &m_input.displacementAngle, 0.f);
	modified |= ImGui::SliderFloat("Branch Length", &m_input.length, 1.f, 100.f);
	modified |= ImGui::SliderFloat("Branch Length Decrease", &m_input.lengthDecreaseFactor, 0.01f, 0.99f);

	if (modified)
		TreeGenerator::generate(m_treeBranchPos, m_treeGenerations, {
			m_treeSkeleton,
			m_input.angle,
			m_input.angleDecreaseFactor,
			m_input.displacementAngle,
			m_input.length,
			m_input.lengthDecreaseFactor
		});

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
		TreeGenerator::generate(m_treeBranchPos, m_treeGenerations, {
			m_treeSkeleton,
			m_input.angle,
			m_input.angleDecreaseFactor,
			m_input.displacementAngle,
			m_input.length,
			m_input.lengthDecreaseFactor
		});
	}

	ImGui::End();
}

void Application::update(float deltatime)
{
	static float elapsed = 0.f;
	static sf::Color brown(54.f / 255, 26.f / 255, 13.f / 255);
	elapsed += deltatime;
	m_treeVertices.clear();
	for (int i = 0; i < m_treeGenerations.size(); i++)
	{
		sf::Color color = brown;
		color.a *= 1.f - (float)m_treeGenerations[i] / 20;
		sf::Vector3f pos1 = rotate(m_treeBranchPos[i * 2] - sf::Vector3f(400.f,0.f,0.f), { elapsed / 10, 0.f });
		sf::Vector3f pos2 = rotate(m_treeBranchPos[i * 2 + 1] - sf::Vector3f(400.f, 0.f, 0.f), { elapsed / 10, 0.f });
		m_treeVertices.append({ { pos1.x + 400.f, pos1.y },  color });
		m_treeVertices.append({ { pos2.x + 400.f, pos2.y },  color });
	}
}

void Application::draw()
{
	m_window.clear(sf::Color(230, 230, 230));
	m_window.draw(m_treeVertices);
	ImGui::SFML::Render();
	m_window.display();
}
