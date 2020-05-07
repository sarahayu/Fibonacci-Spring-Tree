#pragma once
#include <vector>
#include <SFML\Graphics.hpp>

class BranchNode;
struct Branch;
typedef std::vector<Branch> TreeBranches;
struct RenderSettings;

struct TreeGenerator
{
	static void generate(BranchNode &tree, TreeBranches &branches, const RenderSettings &params);
};