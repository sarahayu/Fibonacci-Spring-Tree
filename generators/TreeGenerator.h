#pragma once
#include <vector>
#include <SFML\Graphics.hpp>

class TreeSkeleton;
struct Branch;

typedef std::vector<Branch> TreeBranches;

struct TreeGenerator
{
	struct GeneratorParams
	{
		float angle;
		float angleDecreaseFactor;
		float displacementAngle;
		float length;
		float lengthDecreaseFactor;
		float sunReach;
	};

	static void generate(TreeSkeleton &tree, TreeBranches &branches, const GeneratorParams &params);
};