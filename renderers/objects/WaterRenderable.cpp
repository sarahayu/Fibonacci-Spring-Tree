#include "WaterRenderable.h"
#include "..\..\RenderSettings.h"
#include "..\Model.h"

WaterRenderable::WaterRenderable(const RenderSettings & settings)
	: RenderableObject(settings)
{
}

void WaterRenderable::createMesh()
{
	m_water.vertices.insert(m_water.vertices.end(), {
		{ -100.f,3.f,-100.f },
		{ -100.f,3.f,100.f },
		{ 100.f,3.f,100.f },
		{ 100.f,3.f,-100.f }
	});

	m_water.indices.insert(m_water.indices.end(), {
		0,1,2,0,2,3
	});

	MeshHelper::GLsetup(m_water);
}

void WaterRenderable::pushRender(Model & model) const
{
	if (m_renderSettings.renderBackground) model.pushRender(Model::WATER_MESH, m_water.VAO, m_water.indices.size());
}
