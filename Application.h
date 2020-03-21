#pragma once
#include <SFML\Graphics.hpp>
#include <glm\glm.hpp>
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
	void loadLeafTexture();
	void initBranchObject();
	void initLeavesObject();
	void saveLeafPositions();
	void drawBranch(const TreeGenerator::Branch &branch);
	void drawLeaves(const sf::Vector3f &position);

	int SCR_WIDTH = 1000, SCR_HEIGHT = 650;

	sf::Clock m_clock;
	sf::RenderWindow m_window;
	TreeSkeleton m_treeSkeleton;
	TreeGenerator::Branches m_treeBranches;
	std::vector<sf::Vector3f> m_leafPositions;

	struct {
		glm::vec3 pos;
		glm::mat4 view;
	} m_camera;

	struct {
		unsigned int ID;
		int u_model;
		int u_view;
		int u_projection;
		int u_cameraPos;
	} m_treeShader;

	struct {
		unsigned int ID;
		int u_model;
		int u_view;
		int u_projection;
		int u_cameraPos;
		int u_texture;
	} m_leafShader;

	const static int CYLINDER_FACE_VERT_COUNT = 5;

	struct {
		std::array<sf::Vector3f, CYLINDER_FACE_VERT_COUNT * 2 * 2> vertices;
		std::array<unsigned int, CYLINDER_FACE_VERT_COUNT * 2 * 6> indices;
		unsigned int VBO, VAO, EBO;
	} m_branchDrawable;

	struct {
		std::array<sf::Vector3f, 4> vertices;
		std::array<unsigned int, 6> indices;
		unsigned int VBO, VAO, EBO, texture;
	} m_leavesDrawable;

	struct {
		int fibStart = 1;
		int iterations = 12;
		float angle = 0.94f;
		float angleDecreaseFactor = 0.97f;
		float displacementAngle = 0.26f;
		float length = 3.7f;
		float lengthDecreaseFactor = 0.97f;
		float sunReach = 0.15f;
		float branchTaper = 0.06f;
		float rotate = 0.f;
		float leafDensity = 4.6f;
	} m_input;
};