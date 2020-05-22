#include "SkyboxRenderable.h"
#include "..\..\RenderSettings.h"
#include "..\Model.h"

SkyboxRenderable::SkyboxRenderable(const RenderSettings & settings)
	: RenderableObject(settings)
{
}

void SkyboxRenderable::createMesh()
{
	m_skybox.vertices = {
		{ -1.0f, 1.0f, -1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		  
		{ -1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ -1.0f, 1.0f, 1.0f },
		  
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, -1.0f },
		  
		{ -1.0f, -1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		  
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
		  
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ -1.0f, -1.0f, 1.0f }
	};

	for (unsigned int i = 0; i < 24; i += 4)
		m_skybox.indices.insert(m_skybox.indices.end(), {
			i + 0,
			i + 1,
			i + 2,
			i + 0,
			i + 2,
			i + 3,
		});

	MeshHelper::GLsetup(m_skybox);
}

void SkyboxRenderable::pushRender(Model & model) const
{
	if (m_renderSettings.renderBackground) model.pushRender(Model::SKYBOX_MESH, m_skybox.VAO, m_skybox.indices.size());
}
