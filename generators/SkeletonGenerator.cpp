#include "SkeletonGenerator.h"
#include <random>
#include "..\utils\MathUtil.h"
#include "..\utils\SimplexNoise.h"
#include "..\BranchNode.h"
#include <ctime>

namespace
{
	static int offset = 0;

	struct BranchInfo
	{
		int weight;
		sf::Vector3f noisePos;
	};

	const float randomAngle(const sf::Vector3f &pos)
	{
		const sf::Vector3f newPos = pos * 0.5f + sf::Vector3f(offset, offset, offset);
		return  PI * ((float)(SimplexNoise::noise(newPos.x, newPos.y, newPos.z)));

		/*
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<> dis(0, 100);

		return  PI * ((float)(dis(gen)) / 100 - 0.5f);*/
	}
}

void SkeletonGenerator::generate(BranchNode & tree, const GeneratorParams & params)
{
	offset = std::rand();
	tree.destroy();
	std::vector<std::pair<BranchNode::Ptr, BranchInfo>> branches;
	int fib1 = params.firstRow, fib2 = params.secondRow;

	branches.emplace_back(tree.addChild(0.f, true), BranchInfo{ 1 });

	sf::Vector3f noisePos(0.f, 1.f, 0.f);
	for (int i = 0; i < fib2 - 1; i++)
	{
		float theta = TWO_PI / (fib2 - 1) * i;
		branches.emplace_back(tree.addChild(theta, false), BranchInfo{ 1, noisePos });
		noisePos.x += 1.f;
	}

	std::vector<int> branchWeightProbabilities;
	branchWeightProbabilities.insert(branchWeightProbabilities.end(), fib2, 1);
	int branchTotalWeight = fib2;


	for (int i = 1; i < params.iterations; i++)
	{
		std::vector<std::pair<BranchNode::Ptr, BranchInfo>> childBranches;
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
			if (std::find(weightsChosen.begin(), weightsChosen.end(), branch.second.weight) != weightsChosen.end())
			{
				sf::Vector3f newNoisePos1 = branch.second.noisePos + sf::Vector3f(0.f, 1.f, 0.f),
					newNoisePos2 = branch.second.noisePos + sf::Vector3f(0.f, 0.f, 1.f);
				childBranches.emplace_back(branch.first->addChild(randomAngle(newNoisePos1), true), BranchInfo{ branch.second.weight + 1,newNoisePos1 });
				childBranches.emplace_back(branch.first->addChild(randomAngle(newNoisePos2), false), BranchInfo{ branch.second.weight, newNoisePos2 });
				newWeightProbs.insert(newWeightProbs.end(), {
					branch.second.weight + 1,
					branch.second.weight
				});
				newBranchWeight += branch.second.weight * 2 + 1;
			}
			else
			{
				sf::Vector3f newNoisePos = branch.second.noisePos + sf::Vector3f(0.f, 1.f, 0.f);
				childBranches.emplace_back(branch.first->addChild(randomAngle(newNoisePos), true), BranchInfo{ branch.second.weight + 2, newNoisePos });
				newWeightProbs.push_back(branch.second.weight + 2);
				newBranchWeight += branch.second.weight + 2;
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
