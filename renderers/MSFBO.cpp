#include "MSFBO.h"
#include <iostream>

MSFBO::MSFBO()
	: FBO(true)
{
}

MSFBO::~MSFBO()
{
	m_sample.destroy();
}

void MSFBO::rebuild(const sf::Vector2i & dimensions)
{
	m_sample.rebuild(dimensions);
	m_sample.attachColorTexture();
	m_sample.attachDepthBuffer();
	FBO::rebuild(dimensions);
}

void MSFBO::bindTexture()
{
	m_sample.bindTexture();
}

void MSFBO::blitTexture()
{
	FBO::bind(GL_READ_FRAMEBUFFER);
	m_sample.bind(GL_DRAW_FRAMEBUFFER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int x = getDimensions().x, y = getDimensions().y;
	glBlitFramebuffer(0, 0, x, y, 0, 0, x, y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
