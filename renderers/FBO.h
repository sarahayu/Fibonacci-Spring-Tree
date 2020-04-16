#pragma once
#include <SFML\Graphics.hpp>
#include <glad\glad.h>

class FBO
{
public:
	FBO(const bool &multiSample = false);
	virtual ~FBO();

	virtual void rebuild(const sf::Vector2i &dimensions);
	virtual void destroy();

	void bind(const unsigned int &target = GL_FRAMEBUFFER);
	void bindAndClear();
	virtual void bindTexture();
	const unsigned int getTextureID() const;
	const sf::Vector2i getDimensions() const;

	void attachDepthTexture();
	void attachColorTexture();
	void attachDepthBuffer();
	void attachColorBuffer();

	void attachRenderbuffer(const unsigned int &internalformat, const unsigned int &attachment);
	void attachTexture(const unsigned int &attachment, const unsigned int &format, const unsigned int &dataType);

private:

	unsigned int m_ms;
	sf::Vector2i m_dimensions;

	unsigned int m_FBO;
	unsigned int m_texture;
	std::vector<unsigned int> m_buffers;
};