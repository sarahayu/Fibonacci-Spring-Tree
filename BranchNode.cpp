#include "BranchNode.h"
#include <assert.h>

BranchNode::BranchNode(const float & _rotation)
	: rotation(_rotation)
{
}

void BranchNode::destroy()
{
	generation = 1;
	rotation = 0.f;
	children.clear();
}

const int BranchNode::getGeneration() const
{
	return generation;
}

const float BranchNode::getRotation() const
{
	return rotation;
}

const bool BranchNode::getGrowingStraight() const
{
	return straightGrowth;
}

std::vector<BranchNode::Ptr> BranchNode::getChildren()
{
	return children;
}

BranchNode::Ptr BranchNode::getFirstChild()
{
	assert(hasChildren());
	return children[0];
}

const bool BranchNode::hasChildren() const
{
	return children.size() != 0;
}

BranchNode::Ptr BranchNode::addChild(const float & rotation, const bool & straightGrowth)
{
	Ptr child = std::make_shared<BranchNode>(rotation);
	child->generation = straightGrowth ? generation + 1 : generation + 3;
	child->straightGrowth = straightGrowth;
	children.emplace_back(child);
	return child;
}