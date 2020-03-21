#include "Application.h"
#include "imgui\imgui.h"
#include "imgui\imgui-SFML.h"
#include "SkeletonGenerator.h"
#include "TreeGenerator.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
{
	srand(time(0));

	initWindowOpenGL();
	ImGui::SFML::Init(m_window);

	initShaders();
	loadLeafTexture();
	initBranchObject();
	initLeavesObject();

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

	while (m_window.isOpen())
	{
		const float dt = clock.restart().asSeconds();

		ImGui::SFML::Update(m_window, sf::seconds(dt));
		input();
		update(dt);
		draw();
	}
/*
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);*/
	glfwTerminate();
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

	if (ImGui::SliderFloat("Branch Taper", &m_input.branchTaper, 0.f, 0.2f))
		initBranchObject();

	ImGui::SliderFloat("Rotate", &m_input.rotate, -10.f, 10.f);
	if (ImGui::SliderFloat("Leaf Density", &m_input.leafDensity, 0.1f, 5.f) || modified)
		saveLeafPositions();

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
}

void Application::update(float deltatime)
{
}

void Application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float theta = m_clock.getElapsedTime().asSeconds() / 5 + m_input.rotate;
	m_camera.pos = { -50.0f * std::cos(theta), 0.f, -50.0f * std::sin(theta) };
	m_camera.view = glm::lookAt(m_camera.pos, { 0.f,20.f,0.f }, { 0.f,1.f,0.f });
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);

	glEnable(GL_CULL_FACE);
	glUseProgram(m_treeShader.ID);
	glUniformMatrix4fv(m_treeShader.u_view, 1, GL_FALSE, &m_camera.view[0][0]);
	glUniformMatrix4fv(m_treeShader.u_projection, 1, GL_FALSE, &projection[0][0]);
	glUniform3f(m_treeShader.u_cameraPos, m_camera.pos.x, m_camera.pos.y, m_camera.pos.z);
	glBindVertexArray(m_branchDrawable.VAO);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (const auto &branch : m_treeBranches)
		drawBranch(branch);

	glDisable(GL_CULL_FACE);
	glUseProgram(m_leafShader.ID);
	glUniformMatrix4fv(m_leafShader.u_view, 1, GL_FALSE, &m_camera.view[0][0]);
	glUniformMatrix4fv(m_leafShader.u_projection, 1, GL_FALSE, &projection[0][0]);
	glBindTexture(GL_TEXTURE_2D, m_leavesDrawable.texture);
	glBindVertexArray(m_leavesDrawable.VAO);

	for (const auto &leaf : m_leafPositions)
		drawLeaves(leaf);

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

	glClearColor(115.0 / 255, 194.0 / 255, 251.0 / 255, 1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Application::initShaders()
{
	unsigned int vert, frag;
	int success;
	char infoLog[512];

	auto load = [&](const GLenum &shaderType, const std::string &file, unsigned int &shader) {
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::stringstream shaderStream;

		try
		{
			shaderFile.open(file);
			shaderStream << shaderFile.rdbuf();
		}
		catch (const std::ifstream::failure &failed)
		{
			std::cout << "\nShader file could not be read!";
		}

		std::string shaderString = shaderStream.str();
		const char *shaderCode = shaderString.c_str();
		const bool isVertex = shaderType == GL_VERTEX_SHADER;
		shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "\n" << (isVertex ? "Vertex" : "Fragment") << " compilation failed! " << infoLog;

		}
	};

	auto linkShader = [&](unsigned int &ID) {
		ID = glCreateProgram();
		glAttachShader(ID, vert);
		glAttachShader(ID, frag);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			std::cout << "\nShader linking failed!";
		}
		glDeleteShader(vert);
		glDeleteShader(frag);
	};

	load(GL_VERTEX_SHADER, "shaders/tree-shader.vs", vert);
	load(GL_FRAGMENT_SHADER, "shaders/tree-shader.fs", frag);
	linkShader(m_treeShader.ID);
	std::cout << "\nTree shader successfully loaded";

	load(GL_VERTEX_SHADER, "shaders/leaf-shader.vs", vert);
	load(GL_FRAGMENT_SHADER, "shaders/leaf-shader.fs", frag);
	linkShader(m_leafShader.ID);
	std::cout << "\nLeaf shader successfully loaded";


	glUseProgram(m_treeShader.ID);
	m_treeShader.u_model = glGetUniformLocation(m_treeShader.ID, "model");
	m_treeShader.u_view = glGetUniformLocation(m_treeShader.ID, "view");
	m_treeShader.u_projection = glGetUniformLocation(m_treeShader.ID, "projection");
	m_treeShader.u_cameraPos = glGetUniformLocation(m_treeShader.ID, "cameraPos");

	glUseProgram(m_leafShader.ID);
	m_leafShader.u_model = glGetUniformLocation(m_leafShader.ID, "model");
	m_leafShader.u_view = glGetUniformLocation(m_leafShader.ID, "view");
	m_leafShader.u_projection = glGetUniformLocation(m_leafShader.ID, "projection");
	m_leafShader.u_cameraPos = glGetUniformLocation(m_leafShader.ID, "cameraPos");

}

void Application::loadLeafTexture()
{
	glGenTextures(1, &m_leavesDrawable.texture);
	glBindTexture(GL_TEXTURE_2D, m_leavesDrawable.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrchannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("leaves.png", &width, &height, &nrchannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "\nFailed to load texture";
	stbi_image_free(data);
}

void Application::initBranchObject()
{
	const float PI2 = PI * 2;
	int offset = 0;
	float branchTaper = m_input.branchTaper + 0.8f;
	float yNormalAngle = std::atanf(1 - branchTaper);

	for (int i = 0; i < CYLINDER_FACE_VERT_COUNT; i++)
	{
		int offset2 = offset++;
		float x = std::cos(PI2 / CYLINDER_FACE_VERT_COUNT * i),
			z = std::sin(PI2 / CYLINDER_FACE_VERT_COUNT * i);
		m_branchDrawable.vertices[offset2 * 2] = { x, 0.f, z };
		m_branchDrawable.vertices[offset2 * 2 + CYLINDER_FACE_VERT_COUNT * 2] = { x * branchTaper, 1.f, z * branchTaper };
		m_branchDrawable.vertices[offset2 * 2 + 1] = rotate({ x,0.f,z }, { 0.f, yNormalAngle });
		m_branchDrawable.vertices[offset2 * 2 + CYLINDER_FACE_VERT_COUNT * 2 + 1] = rotate({ x,0.f,z }, { 0.f, yNormalAngle });
	}

	offset = 0;
	for (int i = 0; i < CYLINDER_FACE_VERT_COUNT; i++)
	{
		bool last = i == CYLINDER_FACE_VERT_COUNT - 1;

		m_branchDrawable.indices[offset++] = i;
		m_branchDrawable.indices[offset++] = i + CYLINDER_FACE_VERT_COUNT;
		m_branchDrawable.indices[offset++] = last ? CYLINDER_FACE_VERT_COUNT : i + CYLINDER_FACE_VERT_COUNT + 1;

		m_branchDrawable.indices[offset++] = i;
		m_branchDrawable.indices[offset++] = last ? CYLINDER_FACE_VERT_COUNT : i + CYLINDER_FACE_VERT_COUNT + 1;
		m_branchDrawable.indices[offset++] = last ? 0 : i + 1;
	}

	if (m_branchDrawable.VAO)
	{
		glDeleteVertexArrays(1, &m_branchDrawable.VAO);
		glDeleteBuffers(1, &m_branchDrawable.VBO);
		glDeleteBuffers(1, &m_branchDrawable.EBO);
	}

	glGenVertexArrays(1, &m_branchDrawable.VAO);
	glGenBuffers(1, &m_branchDrawable.VBO);
	glGenBuffers(1, &m_branchDrawable.EBO);
	glBindVertexArray(m_branchDrawable.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_branchDrawable.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sf::Vector3f) * m_branchDrawable.vertices.size(), m_branchDrawable.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_branchDrawable.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_branchDrawable.indices.size(), m_branchDrawable.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(sf::Vector3f), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(sf::Vector3f), (void*)(sizeof(sf::Vector3f)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Application::initLeavesObject()
{
	m_leavesDrawable.vertices = {
		sf::Vector3f{ 0.5f,0.5f,0.f },
		{ 0.5f,-0.5f,0.f },
		{ -0.5f,-0.5f,0.f },
		{ -0.5f,0.5f,0.f } };
	m_leavesDrawable.indices = { 0,3,2,0,2,1 };

	if (m_leavesDrawable.VAO)
	{
		glDeleteVertexArrays(1, &m_leavesDrawable.VAO);
		glDeleteBuffers(1, &m_leavesDrawable.VBO);
		glDeleteBuffers(1, &m_leavesDrawable.EBO);
	}

	glGenVertexArrays(1, &m_leavesDrawable.VAO);
	glGenBuffers(1, &m_leavesDrawable.VBO);
	glGenBuffers(1, &m_leavesDrawable.EBO);
	glBindVertexArray(m_leavesDrawable.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_leavesDrawable.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sf::Vector3f) * m_leavesDrawable.vertices.size(), m_leavesDrawable.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_leavesDrawable.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_leavesDrawable.indices.size(), m_leavesDrawable.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, { branch.start.x, branch.start.y, branch.start.z });
	model = glm::rotate(model, -branch.rotation.x, { 0.f,1.f,0.f });
	model = glm::rotate(model, branch.rotation.y, { 0.f,0.f,1.f });
	float shrinkScale = std::pow(m_input.branchTaper + 0.8f, branch.generation);
	model = glm::scale(model, { shrinkScale, branch.length * 1.05f, shrinkScale });
	glUniformMatrix4fv(m_treeShader.u_model, 1, GL_FALSE, &model[0][0]);

	glDrawElements(GL_TRIANGLES, m_branchDrawable.indices.size(), GL_UNSIGNED_INT, 0);
}

void Application::drawLeaves(const sf::Vector3f & position)
{
	glm::mat4 model = glm::translate(glm::mat4(1.f), { position.x,position.y,position.z });

	for (int i = 0; i < 3; i++)
	{
		glm::mat4 modelRotated = glm::rotate(model, glm::radians(i * 30.f), { 0.f,1.f,0.f });
		modelRotated = glm::scale(modelRotated, { m_input.leafDensity, m_input.leafDensity, m_input.leafDensity });
		glUniformMatrix4fv(m_leafShader.u_model, 1, GL_FALSE, &modelRotated[0][0]);
		glUniform3f(m_leafShader.u_cameraPos, m_camera.pos.x, m_camera.pos.y, m_camera.pos.z);
		glDrawElements(GL_TRIANGLES, m_leavesDrawable.indices.size(), GL_UNSIGNED_INT, 0);
	}
}
