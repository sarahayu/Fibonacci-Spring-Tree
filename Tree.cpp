#include "Tree.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>
#include "generators\SkeletonGenerator.h"
#include "generators\TreeGenerator.h"
#include "renderers\TreeRenderer.h"
#include "utils\MathUtil.h"
#include "TreeSkeleton.h"
#include "InputData.h"
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

Tree::Tree(const Context & context)
	: m_context(context)
{
}

void Tree::createNewTree()
{
	auto fibs = getFibStart(m_context.input->fibStart);
	SkeletonGenerator::generate(m_treeSkeleton, { fibs.first, fibs.second, m_context.input->iterations });
	
	updateExistingTree();
}

void Tree::updateExistingTree()
{
	TreeGenerator::generate(m_treeSkeleton, m_treeBranches, {
		m_context.input->angle,
		m_context.input->angleDecreaseFactor,
		m_context.input->displacementAngle,
		m_context.input->length,
		m_context.input->lengthDecreaseFactor,
		m_context.input->sunReach
	});
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
		if (branch.endNode)
			m_leafPositions.push_back(branch.start + (branch.end - branch.start) / 2.f);
}