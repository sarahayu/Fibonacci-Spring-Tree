#pragma once

class TreeSkeleton;

struct SkeletonGenerator
{
	struct GeneratorParams
	{
		int firstRow;
		int secondRow;
		int iterations;
	};

	static void generate(TreeSkeleton &tree, const GeneratorParams &params);
};