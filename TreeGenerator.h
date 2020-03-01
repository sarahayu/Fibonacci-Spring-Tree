#pragma once
#include <vector>
#include <SFML\Graphics.hpp>

class TreeSkeleton;

struct TreeGenerator
{
	struct GeneratorParams
	{
		TreeSkeleton &tree;
		float angle;
		float angleDecreaseFactor;
		float displacementAngle;
		float length;
		float lengthDecreaseFactor;
	};

	static void generate(std::vector<sf::Vector3f> &branchPositions, std::vector<int> &generation, const GeneratorParams &params);
};