#pragma once
#include <memory>
#include <vector>

class BranchNode
{
public:
	typedef std::shared_ptr<BranchNode> Ptr;
	typedef std::vector<Ptr> Array;

	BranchNode(const float &rotation = 0.f);
	void destroy();

	const int getGeneration() const;
	const float getRotation() const;
	const bool getGrowingStraight() const;
	std::vector<Ptr> getChildren();
	Ptr getFirstChild();
	const bool hasChildren() const;
	Ptr addChild(const float &rotation, const bool &straightGrowth);

private:
	int generation = 1;
	float rotation = 0.f;
	bool straightGrowth = true;
	std::vector<Ptr> children;
};