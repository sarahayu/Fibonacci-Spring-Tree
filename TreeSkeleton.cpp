#include "TreeSkeleton.h"

TreeSkeleton::TreeSkeleton(const float & _rotation)
	: rotation(_rotation)
{
}

void TreeSkeleton::destroy()
{
	generation = 1;
	rotation = 0.f;
	children.clear();
}

const int TreeSkeleton::getGeneration() const
{
	return generation;
}

const float TreeSkeleton::getRotation() const
{
	return rotation;
}

const bool TreeSkeleton::getGrowingStraight() const
{
	return straightGrowth;
}

std::vector<TreeSkeleton::Ptr> TreeSkeleton::getChildren()
{
	return children;
}

TreeSkeleton::Ptr TreeSkeleton::addChild(const float & rotation, const bool & straightGrowth)
{
	Ptr child = std::make_shared<TreeSkeleton>(rotation);
	child->generation = generation + 1;
	child->straightGrowth = straightGrowth;
	children.emplace_back(child);
	return child;
}