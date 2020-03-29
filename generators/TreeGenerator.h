#pragma once
#include <vector>
#include <SFML\Graphics.hpp>

class TreeSkeleton;
struct Branch;
typedef std::vector<Branch> TreeBranches;
struct RenderSettings;

struct TreeGenerator
{
	static void generate(TreeSkeleton &tree, TreeBranches &branches, const RenderSettings &params);
};