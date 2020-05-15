#include "TreeGenerator.h"
#include "..\BranchNode.h"
#include "..\utils\MathUtil.h"
#include "..\RenderSettings.h"
#include "..\Branch.h"
#include <iostream>

namespace
{
	const bool hasLeaves(BranchNode &branch)
	{
		if (!branch.hasChildren()) return true;

		auto nextNode = branch.getFirstChild();
		for (int i = 1; i <= 3; i++)
		{
			if (!nextNode->hasChildren()) return false;
			nextNode = nextNode->getFirstChild();
		}
		return (!nextNode->hasChildren());
	}

	void addBranch(const Branch &parent, BranchNode::Ptr & child,
		const RenderSettings &params,
		TreeBranches& branches)
	{
		auto newParams = params;
		newParams.angle *= params.angleDecreaseFactor;
		newParams.length *= params.lengthDecreaseFactor;

		if (!child->getGrowingStraight()) newParams.length *= params.lengthDecreaseFactor;
		
		sf::Vector3f newBranchRelativePos = MathUtil::rotate(
			MathUtil::rotate({
			0.0,newParams.length,0.0
		}, {
			child->getRotation(), child->getGrowingStraight() ? newParams.displacementAngle : newParams.angle
		}), { parent.rotation.x, parent.rotation.y * (1.f - params.sunReach) });
/*
		sf::Vector3f newBranchRelativePos = MathUtil::rotate({
			0.f, newParams.length, 0.f
		}, { child->getRotation(), ((child->getGrowingStraight() ? newParams.displacementAngle : newParams.angle) + parent.rotation.y) * (1.f - params.sunReach) });
*/
		sf::Vector3f newBranchPos = parent.end + newBranchRelativePos;

		float azimuth = std::atan2f(newBranchRelativePos.z, newBranchRelativePos.x);
		float inclination = std::acos(newBranchRelativePos.y / newParams.length);
		sf::Vector2f newBranchRotation = { azimuth + PI,inclination };
		//if (parent.rotation.y - newBranchRotation.y < 0.1f) newBranchRotation.x = child->getRotation() + parent.rotation.x;

		Branch newBranch = { 
			parent.end, newBranchPos, newBranchRotation,
			newParams.length, child->getGeneration(), hasLeaves(*child.get()) };
		branches.push_back(newBranch);
		
		for (auto &c : child->getChildren())
			addBranch(newBranch, c, newParams, branches);
	}


}

void TreeGenerator::generate(BranchNode& tree, TreeBranches& branches, const RenderSettings& params)
{
	branches.clear();

	const int startOffTrunkCount = 4;
	for (int i = 0; i < startOffTrunkCount; i++)
	{
		Branch trunk;
		trunk.start = { 0.f,params.length * i, 0.f };
		trunk.end = { 0.f,params.length * (i + 1), 0.f };
		trunk.length = params.length;
		trunk.generation = -(startOffTrunkCount - i) + 2;
		trunk.hasLeaves = i == startOffTrunkCount - 1 && hasLeaves(tree);
		branches.push_back(trunk);
	}

	Branch trunkEnd = branches.back();
	for (auto &c : tree.getChildren())
		//addBranch({ {},{ 0.f,params.length, 0.f },{}, params.length, 1, hasLeaves(tree) }, c, params, branches);
		addBranch(trunkEnd, c, params, branches);
}
