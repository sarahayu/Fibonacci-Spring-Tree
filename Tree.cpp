#include "Tree.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>
#include "generators\SkeletonGenerator.h"
#include "generators\TreeGenerator.h"
#include "renderers\MasterRenderer.h"
#include "utils\MathUtil.h"
#include "BranchNode.h"
#include "RenderSettings.h"
#include "Camera.h"

namespace
{
	const std::pair<int, int> getFibStart(const int &offset)
	{
		int first = 1, second = 1;
		for (int i = 1; i < offset; i++)
		{
			int next = first + second;
			first = second;
			second = next;
		}

		return{ first, second };
	}
}

void Tree::createNewTree(const RenderSettings & settings)
{
	auto fibs = getFibStart(settings.fibStart);
	SkeletonGenerator::generate(m_treeSkeleton, { fibs.first, fibs.second, settings.iterations });
	
	updateExistingTree(settings);
}

void Tree::updateExistingTree(const RenderSettings & settings)
{
	TreeGenerator::generate(m_treeSkeleton, m_treeBranches, settings);
	saveLeafPositions();
}

const TreeBranches & Tree::getBranches() const
{
	return m_treeBranches;
}

const std::vector<sf::Vector3f>& Tree::getLeaves() const
{
	return m_leafPositions;
}

void Tree::saveLeafPositions()
{
	m_leafPositions.clear();
	for (const auto &branch : m_treeBranches)
		if (branch.hasLeaves)
			m_leafPositions.push_back(branch.start + (branch.end - branch.start) / 2.f);
}