#include "TreeGenerator.h"
#include "TreeSkeleton.h"
#include "MathUtil.h"

namespace
{
	struct Branch
	{
		sf::Vector3f position;
		sf::Vector2f rotation;
	};

	void insert(std::vector<sf::Vector3f> &positions, const sf::Vector3f &first, const sf::Vector3f &second)
	{
		positions.insert(positions.end(), {
			first,
			second
		});
	}

	void addBranch(const Branch &parent, TreeSkeleton::Ptr & child, 
		const TreeGenerator::GeneratorParams &params,
		std::vector<sf::Vector3f>& branchPositions, std::vector<int>& generation)
	{
		auto newParams = params;
		newParams.angle *= params.angleDecreaseFactor;
		newParams.length *= params.lengthDecreaseFactor;
		
		sf::Vector3f newBranchRelativePos = rotate(
			rotate({
			0.f,-newParams.length,0.f
		}, {
			child->getRotation(), child->getGrowingStraight() ? newParams.displacementAngle : newParams.angle
		}), parent.rotation);

		sf::Vector3f newBranchPos = parent.position + newBranchRelativePos;

		float azimuth = std::atan2f(newBranchRelativePos.z, newBranchRelativePos.x);
		float inclination = std::acos(
			newBranchRelativePos.y /
			std::sqrt(newBranchRelativePos.x*newBranchRelativePos.x
				+ newBranchRelativePos.y*newBranchRelativePos.y 
				+ newBranchRelativePos.z*newBranchRelativePos.z));
		sf::Vector2f newBranchRotation = { std::atan2f(newBranchRelativePos.z, newBranchRelativePos.x),
			PI - std::abs(inclination) };
		
		insert(branchPositions, parent.position, newBranchPos);
		generation.push_back(child->getGeneration());

		Branch newBranch = { newBranchPos, newBranchRotation };
		
		for (auto &c : child->getChildren())
			addBranch(newBranch, c, newParams, branchPositions, generation);
	}


}

void TreeGenerator::generate(std::vector<sf::Vector3f>& branchPositions, std::vector<int>& generation, const GeneratorParams & params)
{
	branchPositions.clear();
	generation.clear();
	sf::Vector3f treeStart = { 400.f, 600.f, 0.f };
	sf::Vector3f nextVertex = treeStart + sf::Vector3f(0.f, -params.length, 0.f);

	insert(branchPositions, treeStart, nextVertex);
	generation.push_back(1);

	for (auto &c : params.tree.getChildren())
		addBranch({ nextVertex,{} }, c, params, branchPositions, generation);
}
