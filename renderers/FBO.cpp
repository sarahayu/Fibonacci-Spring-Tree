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
	m_nextColorAttachment = GL_COLOR_ATTACHMENT0;

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

void FBO::bindTexture(const unsigned int & attachment) const
{
	glBindTexture(m_ms, getTextureID(attachment));
}

const unsigned int FBO::getTextureID(const unsigned int & attachment) const
{
	return m_textures.at(attachment);
}

const sf::Vector2i FBO::getDimensions() const
{
	return m_dimensions;
}

void FBO::attachDepthTexture(const TexParams & params)
{
	attachTexture(GL_DEPTH_ATTACHMENT, params);

	glm::vec4 color = glm::vec4(1.f);
	glTexParameterfv(m_ms, GL_TEXTURE_BORDER_COLOR, &color[0]);
}

const unsigned int FBO::attachColorTexture(const TexParams & params)
{
	attachTexture(m_nextColorAttachment, params);
	return m_nextColorAttachment++;
}

void FBO::attachDepthBuffer()
{
	attachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8);
}

void FBO::attachColorBuffer()
{
	attachRenderbuffer(m_nextColorAttachment++, GL_RGBA8);
}

void FBO::attachRenderbuffer(const unsigned int & attachment, const unsigned int & internalformat)
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

void FBO::attachTexture(const unsigned int & attachment, const TexParams & params)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(m_ms, texture);
	if (m_ms == GL_TEXTURE_2D_MULTISAMPLE)
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, params.internalFormat, m_dimensions.x, m_dimensions.y, GL_TRUE);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, params.internalFormat, m_dimensions.x, m_dimensions.y, 0, params.format, params.type, nullptr);
	glTexParameteri(m_ms, GL_TEXTURE_MIN_FILTER, params.filter);
	glTexParameteri(m_ms, GL_TEXTURE_MAG_FILTER, params.filter);
	glTexParameteri(m_ms, GL_TEXTURE_WRAP_S, params.wrap);
	glTexParameteri(m_ms, GL_TEXTURE_WRAP_T, params.wrap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, m_ms, texture, 0);
	m_textures[attachment] = texture;
}

const TexParams FBO::getDefaultDepthAttachmentParams()
{
	TexParams params;
	params.format = params.internalFormat = GL_DEPTH_COMPONENT;
	params.type = GL_FLOAT;
	params.filter = GL_NEAREST;
	params.wrap = GL_CLAMP_TO_BORDER;

	return params;
}
