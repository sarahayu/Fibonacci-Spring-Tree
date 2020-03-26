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
#include "..\Camera.h"
#include "..\Tree.h"
#include "TreeComponentRenderer.h"

BlurRenderer::BlurRenderer(const Context & context)
	: m_context(context)
{
}

void BlurRenderer::loadResources()
{
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(sf::Vector2f), quadVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	ShaderUtil::linkShader("resources/shaders/quad-shader.vs", "resources/shaders/quad-shader.fs", quadShader);
	std::cout << "\nQuad shader successfully loaded!";

	glGenFramebuffers(1, &fboA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboA);

	glGenRenderbuffers(1, &rbA1);
	glBindRenderbuffer(GL_RENDERBUFFER, rbA1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *m_context.scrWidth, *m_context.scrHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbA1);

	glGenRenderbuffers(1, &rbA2);
	glBindRenderbuffer(GL_RENDERBUFFER, rbA2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, *m_context.scrWidth, *m_context.scrHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbA2);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "\nFramebuffer not complete!";
	}


	glGenFramebuffers(1, &fboB);
	glBindFramebuffer(GL_FRAMEBUFFER, fboB);

	glGenTextures(1, &texB);
	glBindTexture(GL_TEXTURE_2D, texB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *m_context.scrWidth, *m_context.scrHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texB, 0);

	glGenRenderbuffers(1, &rbB);
	glBindRenderbuffer(GL_RENDERBUFFER, rbB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *m_context.scrWidth, *m_context.scrHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbB);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "\nFramebuffer not complete!";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BlurRenderer::draw(TreeComponentRenderer & componentRenderer, const Tree & tree, const RenderParams & params)
{
	static sf::Clock clock;
	float elapsed = clock.getElapsedTime().asSeconds();
	int n = 4;
	glm::vec3 focus(0.f, 20.f, 0.f);

	glm::vec3 jitterCamRight = glm::normalize(glm::cross(focus - params.camera->pos, { 0.f,1.f,0.f }));
	glm::vec3 jitterCamUp = glm::normalize(glm::cross(focus - params.camera->pos, jitterCamRight));
	jitterCamUp.y = -jitterCamUp.y;

	glBindFramebuffer(GL_FRAMEBUFFER, fboA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);

	for (int i = 0; i < n; i++)
	{
		Camera copy = *params.camera;
		copy.pos += params.depthOfField * (jitterCamRight * std::cos(i * 2 * PI / n) + jitterCamUp * std::sin(i * 2 * PI / n));
		copy.view = glm::lookAt(copy.pos, focus, jitterCamUp);

		glBindFramebuffer(GL_FRAMEBUFFER, fboB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		componentRenderer.prepareBranchDraw(copy, params.lightSource, params.branchTaper + 0.8f);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for (const auto &branch : tree.getBranches())
			componentRenderer.drawBranch(branch.start, branch.rotation, branch.length, branch.generation);

		glDisable(GL_CULL_FACE);
		componentRenderer.prepareLeavesDraw(copy, elapsed, params.leafDensity);

		componentRenderer.drawLeaves(tree.getLeaves());

		glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		glBindTexture(GL_TEXTURE_2D, texB);
		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
		glBlendColor(0.f, 0.f, 0.f, 1.f / n);
		glEnable(GL_BLEND);

		glEnable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(quadShader);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboA);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glBlitFramebuffer(0, 0, *m_context.scrWidth, *m_context.scrHeight, 0, 0, *m_context.scrWidth, *m_context.scrHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
