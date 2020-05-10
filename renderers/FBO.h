#pragma once
#include <SFML\Graphics.hpp>
#include <glad\glad.h>
#include "..\utils\ShaderUtil.h"

class FBO
{
public:
	FBO(const bool &multiSample = false);
	virtual ~FBO();

	virtual void rebuild(const sf::Vector2i &dimensions);
	virtual void destroy();

	void bind(const unsigned int &target = GL_FRAMEBUFFER);
	void bindAndClear();
	virtual void bindTexture(const unsigned int &attachment) const;
	const unsigned int getTextureID(const unsigned int &attachment) const;
	const sf::Vector2i getDimensions() const;

	void attachDepthTexture(const TexParams &params = getDefaultDepthAttachmentParams());		// attaches GL_DEPTH_ATTACHMENT
	const unsigned int attachColorTexture(const TexParams &params = TexParams());				// attaches GL_COLOR_ATTACHMENTx
	void attachDepthBuffer();
	void attachColorBuffer();

	void attachRenderbuffer(const unsigned int &attachment, const unsigned int &internalformat);
	void attachTexture(const unsigned int &attachment, const TexParams &params = TexParams());

	static const TexParams getDefaultDepthAttachmentParams();

private:

	unsigned int m_ms;
	sf::Vector2i m_dimensions;
	unsigned int m_nextColorAttachment = GL_COLOR_ATTACHMENT0;

	unsigned int m_FBO;
	std::map<unsigned int, unsigned int> m_textures;
	std::vector<unsigned int> m_buffers;
};