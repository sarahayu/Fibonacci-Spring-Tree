#pragma once

class BranchNode;

struct SkeletonGenerator
{
	struct GeneratorParams
	{
		int firstRow;
		int secondRow;
		int iterations;
	};

	static void generate(BranchNode &tree, const GeneratorParams &params);
};