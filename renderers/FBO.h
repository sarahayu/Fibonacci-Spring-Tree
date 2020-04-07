#pragma once
#include <SFML\Graphics.hpp>
#include <glad\glad.h>

class FBO
{
public:

	void reinstantiate(const sf::Vector2i &dimensions);
	void destroy();

	void bind(const unsigned int &target = GL_FRAMEBUFFER);
	void bindAndClear();
	void bindTexture();

	void attachColorTexture();
	void attachDepthBuffer();
	void attachColorBuffer();

	void attachRenderbuffer(const unsigned int &internalformat, const unsigned int &attachment);

private:

	sf::Vector2i m_dimensions;

	unsigned int m_FBO;
	unsigned int m_texture;
	std::vector<unsigned int> m_buffers;
};