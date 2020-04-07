#include "ScreenQuad.h"
#include <glad\glad.h>
#include "..\utils\ShaderUtil.h"

ScreenQuad & ScreenQuad::getQuad()
{
	static ScreenQuad quad;
	return quad;
}

void ScreenQuad::draw(const bool & useTextureShader)
{
	if (useTextureShader) glUseProgram(m_quadShader.ID);
	glBindVertexArray(m_quadDrawable.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

ScreenQuad::ScreenQuad()
{
	glGenVertexArrays(1, &m_quadDrawable.VAO);
	glGenBuffers(1, &m_quadDrawable.VBO);
	glBindVertexArray(m_quadDrawable.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadDrawable.VBO);
	glBufferData(GL_ARRAY_BUFFER, m_quadDrawable.vertices.size() * sizeof(sf::Vector2f), m_quadDrawable.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	ShaderUtil::linkShader(m_quadShader.ID, "quad-shader");
}
