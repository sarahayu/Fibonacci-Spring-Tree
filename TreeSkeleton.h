#pragma once
#include <memory>
#include <vector>

class TreeSkeleton
{
public:
	typedef std::shared_ptr<TreeSkeleton> Ptr;

	TreeSkeleton(const float &rotation = 0.f);
	void destroy();

	const int getGeneration() const;
	const float getRotation() const;
	std::vector<Ptr> getChildren();
	Ptr addChild(const float &rotation);

private:
	int generation = 1;
	float rotation;
	std::vector<Ptr> children;
};