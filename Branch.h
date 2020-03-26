#pragma once
#include <SFML\Graphics.hpp>

struct Branch
{
	sf::Vector3f start;
	sf::Vector3f end;
	sf::Vector2f rotation;
	float length;
	int generation;
	bool endNode;
};

typedef std::vector<Branch> TreeBranches;
