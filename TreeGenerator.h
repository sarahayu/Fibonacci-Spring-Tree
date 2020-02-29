#pragma once
#include "TreeSkeleton.h"

struct GeneratorParams
{
	int firstRow;
	int secondRow;
	int iterations;
};

struct TreeGenerator
{
	static void generate(TreeSkeleton &tree, const GeneratorParams &params);
};