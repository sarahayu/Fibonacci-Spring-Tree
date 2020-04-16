#include "FBO.h"
#include <glad\glad.h>
#include <glm\glm.hpp>
#include <iostream>

FBO::FBO(const bool & multiSample)
	: m_ms(multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D)
{
}

FBO::~FBO()
{
	destroy();
}

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
	glBindTexture(m_ms, m_texture);
}

const unsigned int FBO::getTextureID() const
{
	return m_texture;
}

const sf::Vector2i FBO::getDimensions() const
{
	return m_dimensions;
}

void FBO::attachDepthTexture()
{
	attachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);
	glTexParameteri(m_ms, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(m_ms, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_ms, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(m_ms, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glm::vec4 color = glm::vec4(1.f);
	glTexParameterfv(m_ms, GL_TEXTURE_BORDER_COLOR, &color[0]);

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
	if (m_ms == GL_TEXTURE_2D_MULTISAMPLE)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, internalformat, m_dimensions.x, m_dimensions.y);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_dimensions.x, m_dimensions.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rb);
	m_buffers.push_back(rb);
}

void FBO::attachTexture(const unsigned int & attachment, const unsigned int & format, const unsigned int & dataType)
{
	glGenTextures(1, &m_texture);
	glBindTexture(m_ms, m_texture);
	if (m_ms == GL_TEXTURE_2D_MULTISAMPLE)
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, format, m_dimensions.x, m_dimensions.y, GL_TRUE);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_dimensions.x, m_dimensions.y, 0, format, dataType, nullptr);
	glTexParameteri(m_ms, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_ms, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_ms, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_ms, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, m_ms, m_texture, 0);
}
