#pragma once
#include <SFML\Graphics.hpp>
#include <array>

class ScreenQuad
{
public:
	static ScreenQuad& getQuad();

	void draw(const bool &useTextureShader = true);

private:
	ScreenQuad();

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

	struct {
		unsigned int ID;
	} m_quadShader;
};