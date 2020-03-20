#pragma once
#include <SFML\Graphics.hpp>
#include <array>
#include "TreeSkeleton.h"
#include "TreeGenerator.h"
#include "MathUtil.h"

class Application
{
public:
	Application();

	void run();

private:

	void input();
	void update(float deltatime);
	void draw();

	void initWindowOpenGL();
	void initShaders();
	void initBranchObject();
	void drawBranch(const TreeGenerator::Branch &branch);

	const static int SCR_WIDTH = 1000, SCR_HEIGHT = 650;

	sf::Clock m_clock;
	sf::RenderWindow m_window;
	TreeSkeleton m_treeSkeleton;
	TreeGenerator::Branches m_treeBranches;
	sf::VertexArray m_treeVertices;

	struct {
		unsigned int ID;
		int u_model;
		int u_view;
		int u_projection;
		int u_cameraPos;
	} m_treeShader;

	const static int CYLINDER_FACE_VERT_COUNT = 8;

	struct {
		std::array<sf::Vector3f, CYLINDER_FACE_VERT_COUNT * 2 * 2> vertices;
		std::array<unsigned int, CYLINDER_FACE_VERT_COUNT * 2 * 6> indices;
		unsigned int VBO, VAO, EBO;
	} m_branchDrawable;

	struct {
		int fibStart = 1;
		int iterations = 10;
		float angle = TWO_PI * 1 / 8;
		float angleDecreaseFactor = 0.97f;
		float displacementAngle = TWO_PI * 1 / 48;
		float length = 4.f;
		float lengthDecreaseFactor = 0.97f;
		float sunReach = 0.15f;
		float rotate = 0.f;
		float branchTaper = 0.02f;
	} m_input;
};