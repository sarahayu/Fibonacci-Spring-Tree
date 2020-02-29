#include "TreeGenerator.h"
#include "MathUtil.h"

namespace
{
	const float randomAngle()
	{
		return  TWO_PI * (std::rand() % 100) / 100;
	}
}

void TreeGenerator::generate(TreeSkeleton & tree, const GeneratorParams & params)
{
	tree.destroy();
	std::vector<std::pair<TreeSkeleton::Ptr, int>> branches;
	int fib1 = params.firstRow, fib2 = params.secondRow;

	branches.emplace_back(tree.addChild(0.f), 1);

	for (int i = 0; i < fib2 - 1; i++)
	{
		float theta = TWO_PI / (fib2 - 1) * i;
		branches.emplace_back(tree.addChild(theta), 1);
	}

	std::vector<int> branchWeightProbabilities;
	branchWeightProbabilities.insert(branchWeightProbabilities.end(), fib2, 1);
	int branchTotalWeight = fib2;


	for (int i = 1; i < params.iterations; i++)
	{
		std::vector<std::pair<TreeSkeleton::Ptr, int>> childBranches;
		std::vector<int> newWeightProbs;
		int newBranchWeight = 0;
		int branchCount = fib2;
		int branchesToAdd = fib1;

		int probability = std::rand() % branchTotalWeight + 1;
		std::vector<int> weightsChosen;

		for (auto probabilityHere = branchWeightProbabilities.begin();
			branchesToAdd-- > 0 && probabilityHere != branchWeightProbabilities.end();)
		{
			int weight = *probabilityHere;
			if (probability -= weight <= 0)
			{
				probabilityHere = branchWeightProbabilities.erase(probabilityHere);
				weightsChosen.push_back(weight);
			}
			else
				probabilityHere++;
		}

		for (int i = branches.size() - 1; i >= 1; i--)
			std::swap(branches[i], branches[std::rand() % i]);

		for (const auto &branch : branches)
		{
			if (std::find(weightsChosen.begin(), weightsChosen.end(), branch.second) != weightsChosen.end())
			{
				childBranches.emplace_back(branch.first->addChild(randomAngle()), branch.second + 1);
				childBranches.emplace_back(branch.first->addChild(randomAngle()), branch.second);
				newWeightProbs.insert(newWeightProbs.end(), {
					branch.second + 1,
					branch.second
				});
				newBranchWeight += branch.second * 2 + 1;
			}
			else
			{
				childBranches.emplace_back(branch.first->addChild(randomAngle()), branch.second + 2);
				newWeightProbs.push_back(branch.second + 2);
				newBranchWeight += branch.second + 2;
			}
		}

		int nextFib = fib1 + fib2;
		fib1 = fib2;
		fib2 = nextFib;

		branches = childBranches;
		branchWeightProbabilities = newWeightProbs;
		branchTotalWeight = newBranchWeight;
	}
}
