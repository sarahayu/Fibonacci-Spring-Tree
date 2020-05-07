#pragma once
#include <array>
#include "BranchNode.h"
#include "Branch.h"

class MasterRenderer;
class Camera;
struct RenderSettings;

class Tree
{
public:
	void createNewTree(const RenderSettings &settings);
	void updateExistingTree(const RenderSettings &settings);

	const TreeBranches& getBranches() const;
	const std::vector<sf::Vector3f>& getLeaves() const;

private:
	void saveLeafPositions();

	BranchNode m_treeSkeleton;
	TreeBranches m_treeBranches;
	std::vector<sf::Vector3f> m_leafPositions;


};