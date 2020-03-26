#pragma once
#include <array>
#include "TreeSkeleton.h"
#include "Branch.h"

class TreeRenderer;
struct Camera;
struct InputData;

class Tree
{
public:
	struct Context
	{
		InputData *input;
	};

	Tree(const Context &context);

	void createNewTree();
	void updateExistingTree();

	const TreeBranches& getBranches() const;
	const std::vector<sf::Vector3f>& getLeaves() const;

private:
	void saveLeafPositions();

	Context m_context;

	TreeSkeleton m_treeSkeleton;
	TreeBranches m_treeBranches;
	std::vector<sf::Vector3f> m_leafPositions;
};