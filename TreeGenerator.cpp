#include "TreeGenerator.h"
#include "TreeSkeleton.h"
#include "MathUtil.h"
#include <iostream>

namespace
{
	void addBranch(const TreeGenerator::Branch &parent, TreeSkeleton::Ptr & child,
		const TreeGenerator::GeneratorParams &params,
		TreeGenerator::Branches& branches)
	{
		auto newParams = params;
		newParams.angle *= params.angleDecreaseFactor;
		newParams.length *= params.lengthDecreaseFactor;
		
		sf::Vector3f newBranchRelativePos = rotate(
			rotate({
			0.0,newParams.length,0.0
		}, {
			child->getRotation(), child->getGrowingStraight() ? newParams.displacementAngle : newParams.angle
		}), { parent.rotation.x, parent.rotation.y * (1.f - params.sunReach) });

		sf::Vector3f newBranchPos = parent.end + newBranchRelativePos;

		float azimuth = std::atan2f(newBranchRelativePos.z, newBranchRelativePos.x);
		float inclination = std::acos(newBranchRelativePos.y / newParams.length);
		sf::Vector2f newBranchRotation = { azimuth + PI,inclination };

		TreeGenerator::Branch newBranch = { parent.end, newBranchPos, newBranchRotation, newParams.length, child->getGeneration() };
		branches.push_back(newBranch);
		
		for (auto &c : child->getChildren())
			addBranch(newBranch, c, newParams, branches);
	}


}

void TreeGenerator::generate(TreeSkeleton& tree, TreeGenerator::Branches& branches, const GeneratorParams& params)
{
	branches.clear();
	sf::Vector3f nextVertex = sf::Vector3f(0.0, params.length, 0.0);

	TreeGenerator::Branch first = { {}, nextVertex,{}, params.length, 1 };
	branches.push_back(first);

	for (auto &c : tree.getChildren())
		addBranch(first, c, params, branches);
}
