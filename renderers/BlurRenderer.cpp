#include "BlurRenderer.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "..\utils\ShaderUtil.h"
#include "..\utils\MathUtil.h"
#include "..\RenderSettings.h"
#include "..\Camera.h"
#include "..\Tree.h"
#include "TreeComponentRenderer.h"

void BlurRenderer::loadResources(const sf::Vector2i & screenDimensions)
{
	glGenVertexArrays(1, &m_quadDrawable.VAO);
	glGenBuffers(1, &m_quadDrawable.VBO);
	glBindVertexArray(m_quadDrawable.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadDrawable.VBO);
	glBufferData(GL_ARRAY_BUFFER, m_quadDrawable.vertices.size() * sizeof(sf::Vector2f), m_quadDrawable.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	ShaderUtil::linkShader("resources/shaders/quad-shader.vs", "resources/shaders/quad-shader.fs", quadShader);
	std::cout << "\nQuad shader successfully loaded!";

	reloadFramebuffers(screenDimensions);
}

void BlurRenderer::reloadFramebuffers(const sf::Vector2i & screenDimensions)
{
	m_screenDimensions = screenDimensions;

	if (m_accumulationBuffer.FBO)
	{
		glDeleteFramebuffers(1, &m_accumulationBuffer.FBO);
		glDeleteFramebuffers(1, &m_textureBuffer.FBO);
		glDeleteRenderbuffers(1, &m_accumulationBuffer.colorRB);
		glDeleteRenderbuffers(1, &m_accumulationBuffer.depthRB);
		glDeleteRenderbuffers(1, &m_textureBuffer.depthRB);
	}

	const int scrX = m_screenDimensions.x, scrY = m_screenDimensions.y;

	glGenFramebuffers(1, &m_accumulationBuffer.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_accumulationBuffer.FBO);

	glGenRenderbuffers(1, &m_accumulationBuffer.depthRB);
	glBindRenderbuffer(GL_RENDERBUFFER, m_accumulationBuffer.depthRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scrX, scrY);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_accumulationBuffer.depthRB);

	glGenRenderbuffers(1, &m_accumulationBuffer.colorRB);
	glBindRenderbuffer(GL_RENDERBUFFER, m_accumulationBuffer.colorRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, scrX, scrY);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_accumulationBuffer.colorRB);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "\nFramebuffer not complete!";

	glGenFramebuffers(1, &m_textureBuffer.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_textureBuffer.FBO);

	glGenTextures(1, &m_textureBuffer.colorTex);
	glBindTexture(GL_TEXTURE_2D, m_textureBuffer.colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrX, scrY, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureBuffer.colorTex, 0);

	glGenRenderbuffers(1, &m_textureBuffer.depthRB);
	glBindRenderbuffer(GL_RENDERBUFFER, m_textureBuffer.depthRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scrX, scrY);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_textureBuffer.depthRB);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "\nFramebuffer not complete!";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BlurRenderer::draw(TreeComponentRenderer & componentRenderer, const Tree & tree, const Camera & camera, const RenderSettings & settings)
{
	static sf::Clock clock;
	float elapsed = clock.getElapsedTime().asSeconds();
	int n = 1;
	glm::vec3 focus(0.f, 20.f, 0.f);

	glm::vec3 jitterCamRight = glm::normalize(glm::cross(focus - camera.pos, { 0.f,1.f,0.f }));
	glm::vec3 jitterCamUp = glm::normalize(glm::cross(focus - camera.pos, jitterCamRight));
	jitterCamUp.y = -jitterCamUp.y;

	glBindFramebuffer(GL_FRAMEBUFFER, m_accumulationBuffer.FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < n; i++)
	{
		Camera jitterCam = camera;
		//jitterCam.pos += settings.depthOfField * (jitterCamRight * std::cos(i * 2 * PI / n) + jitterCamUp * std::sin(i * 2 * PI / n));
		jitterCam.view = glm::lookAt(jitterCam.pos, focus, jitterCamUp);

		glBindFramebuffer(GL_FRAMEBUFFER, m_textureBuffer.FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		componentRenderer.drawBranches(tree.getBranches(), jitterCam, settings);
		componentRenderer.drawLeaves(tree.getLeaves(), jitterCam, settings);

		glBindFramebuffer(GL_FRAMEBUFFER, m_accumulationBuffer.FBO);
		glBindTexture(GL_TEXTURE_2D, m_textureBuffer.colorTex);

		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
		glBlendColor(0.f, 0.f, 0.f, 1.f / n);

		glUseProgram(quadShader);
		glBindVertexArray(m_quadDrawable.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_accumulationBuffer.FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	const int scrX = m_screenDimensions.x, scrY = m_screenDimensions.y;

	glBlitFramebuffer(0, 0, scrX, scrY, 0, 0, scrX, scrY, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
