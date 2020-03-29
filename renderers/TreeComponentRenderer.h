#pragma once
#include <SFML\Graphics.hpp>
#include <array>
#include "TreeShaders.h"
#include "..\Camera.h"

struct Branch;
struct Camera;
typedef std::vector<Branch> TreeBranches;
struct RenderSettings;

class TreeComponentRenderer
{
public:
	void loadResources();
	void drawBranches(const TreeBranches &branches, const Camera &camera, const RenderSettings& params);
	void drawLeaves(const std::vector<sf::Vector3f> &positions, const Camera &camera, const RenderSettings &params);

private:
	TreeShaders m_shaders;

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