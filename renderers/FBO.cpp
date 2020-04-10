#include "FBO.h"
#include <glad\glad.h>

void FBO::rebuild(const sf::Vector2i & dimensions)
{
	m_dimensions = dimensions;
	destroy();

	glGenFramebuffers(1, &m_FBO);
	bind();
}

void FBO::destroy()
{
	if (m_FBO)
	{
		glDeleteFramebuffers(1, &m_FBO);
		glDeleteRenderbuffers(m_buffers.size(), m_buffers.data());
		m_buffers.clear();
	}
}

void FBO::bind(const unsigned int & target)
{
	glBindFramebuffer(target, m_FBO);
}

void FBO::bindAndClear()
{
	bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FBO::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

const unsigned int FBO::getTextureID() const
{
	return m_texture;
}

void FBO::attachDepthTexture()
{
	attachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);
}

void FBO::attachColorTexture()
{
	attachTexture(GL_COLOR_ATTACHMENT0, GL_RGBA, GL_UNSIGNED_BYTE);
}

void FBO::attachDepthBuffer()
{
	attachRenderbuffer(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
}

void FBO::attachColorBuffer()
{
	attachRenderbuffer(GL_RGBA8, GL_COLOR_ATTACHMENT0);
}

void FBO::attachRenderbuffer(const unsigned int & internalformat, const unsigned int & attachment)
{
	unsigned int rb;
	glGenRenderbuffers(1, &rb);
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_dimensions.x, m_dimensions.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rb);
	m_buffers.push_back(rb);
}

void FBO::attachTexture(const unsigned int & attachment, const unsigned int & format, const unsigned int & dataType)
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_dimensions.x, m_dimensions.y, 0, format, dataType, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_texture, 0);
}
