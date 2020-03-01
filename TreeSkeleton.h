#pragma once
#include <memory>
#include <vector>

class TreeSkeleton
{
public:
	typedef std::shared_ptr<TreeSkeleton> Ptr;
	typedef std::vector<Ptr> Array;

	TreeSkeleton(const float &rotation = 0.f);
	void destroy();

	const int getGeneration() const;
	const float getRotation() const;
	const bool getGrowingStraight() const;
	std::vector<Ptr> getChildren();
	Ptr addChild(const float &rotation, const bool &straightGrowth);

private:
	int generation = 1;
	float rotation = 0.f;
	bool straightGrowth = true;
	std::vector<Ptr> children;
};