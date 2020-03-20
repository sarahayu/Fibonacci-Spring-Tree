#pragma once
#include <vector>
#include <SFML\Graphics.hpp>

class TreeSkeleton;


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

	struct Branch
	{
		sf::Vector3f start;
		sf::Vector3f end;
		sf::Vector2f rotation;
		float length;
		int generation;
	};

	typedef std::vector<Branch> Branches;

	static void generate(TreeSkeleton &tree, Branches &branches, const GeneratorParams &params);
};