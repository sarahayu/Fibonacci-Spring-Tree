#pragma once
#include "TreeComponentRenderer.h"
#include "BlurRenderer.h"

struct InputData;
struct Camera;
class Tree;

class TreeRenderer
{
public:
	struct Context
	{
		InputData *input;
		int *scrWidth, *scrHeight;
	};

	TreeRenderer(const Context &context);

	void loadResources();

	void draw(const Tree &tree, Camera &camera, const sf::Vector3f &lightSource);

private:
	Context m_context;

	TreeComponentRenderer m_componentRenderer;
	BlurRenderer m_blurRenderer;

	sf::Clock m_clock;
};