#include "WaterRenderable.h"
#include "..\..\RenderSettings.h"
#include "..\Model.h"

WaterRenderable::WaterRenderable(const RenderSettings & settings)
	: RenderableObject(settings)
{
}

void WaterRenderable::createMesh()
{
	const float width = 200.f;
	const int cells = 150;
	const int cellsPlusOne = cells + 1;
	const float cellWidth = width / cells;

	for (int z = 0; z < cellsPlusOne; z++)
		for (int x = 0; x < cellsPlusOne; x++)
		{
			const float X = -width / 2 + x * cellWidth,
				Z = -width / 2 + z * cellWidth;
			m_water.vertices.push_back({ X, 3.f, Z });
		}

	for (int i = 0; i < cells; i++)
		for (int j = 0; j < cells; j++)
		{
			auto &indices = m_water.indices;

			unsigned int v0 = i * cellsPlusOne + j,
				v1 = (i + 1) * cellsPlusOne + j,
				v2 = (i + 1) * cellsPlusOne + j + 1,
				v3 = i * cellsPlusOne + j + 1;

			indices.insert(indices.end(), {
				v0, v1, v2,
				v0, v2, v3
			});
		}

	MeshHelper::GLsetup(m_water);
}

void WaterRenderable::pushRender(Model & model) const
{
	if (m_renderSettings.renderBackground) model.pushRender(Model::WATER_MESH, m_water.VAO, m_water.indices.size());
}
