#pragma once
#include <SFML\Graphics.hpp>
#include <array>

class TreeComponentRenderer;
class Tree;
struct InputData;
struct Camera;

class BlurRenderer
{
public:
	struct Context
	{
		InputData *input;
		int *scrWidth, *scrHeight;
	};

	struct RenderParams
	{
		Camera *camera;
		sf::Vector3f lightSource;
		float depthOfField;
		float branchTaper;
		float leafDensity;
	};

	BlurRenderer(const Context &context);

	void loadResources();

	void draw(TreeComponentRenderer &componentRenderer, const Tree &tree, const RenderParams &params);

private:

	Context m_context;

	std::array<sf::Vector2f, 6> quadVertices = {
		sf::Vector2f{ -1.f,1.f },
		{ -1.f,-1.f },
		{ 1.f,-1.f },
		{ -1.f,1.f },
		{ 1.f,-1.f },
		{ 1.f,1.f }
	};
	unsigned int quadVAO, quadVBO;
	unsigned int quadShader;
	unsigned int rbA1 = 0, rbA2, fboA = 0, texB = 0, fboB, rbB;
};