#pragma once
#include <SFML\Graphics.hpp>
#include <array>
#include "TreeShaders.h"

struct Camera;

class TreeComponentRenderer
{
public:
	void loadResources();

	void prepareBranchDraw(Camera &camera, const sf::Vector3f &lightSource, const float &taper);
	void prepareLeavesDraw(Camera & camera, const float &elapsed, const float &leafDensity);
	void drawBranch(const sf::Vector3f &position, const sf::Vector2f &rotation, const float &length, const int &generation);
	void drawLeaves(const std::vector<sf::Vector3f> &positions);

private:
	TreeShaders m_shaders;

	struct {
		float branchTaper = -1.f;
		float leafDensity;
	} m_settings;

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
};