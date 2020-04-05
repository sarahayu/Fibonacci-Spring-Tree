#pragma once
#include <SFML\Graphics.hpp>
#include <array>

class TreeComponentRenderer;
struct TreeMesh;
struct RenderSettings;
struct Camera;

class BlurRenderer
{
public:

	void loadResources(const sf::Vector2i &screenDimensions);
	void reloadFramebuffers(const sf::Vector2i &screenDimensions);

	void draw(TreeComponentRenderer &componentRenderer, const TreeMesh &treeMesh, const Camera &camera, const RenderSettings &settings);

private:

	sf::Vector2i m_screenDimensions;

	struct {
		std::array<sf::Vector2f, 6> vertices = {
			sf::Vector2f{ -1.f,1.f },
			{ -1.f,-1.f },
			{ 1.f,-1.f },
			{ -1.f,1.f },
			{ 1.f,-1.f },
			{ 1.f,1.f }
		};
		unsigned int VAO, VBO;
	} m_quadDrawable;

	unsigned int quadShader;

	struct {
		unsigned int FBO,
			depthRB,
			colorRB;
	} m_accumulationBuffer;

	struct {
		unsigned int FBO,
			colorTex,
			depthRB;
	} m_textureBuffer;
};