#include "LandRenderable.h"
#include "..\..\utils\MathUtil.h"
#include "..\..\utils\SimplexNoise.h"
#include "..\..\RenderSettings.h"
#include "..\Mesh.h"
#include "..\Model.h"

LandRenderable::LandRenderable(const RenderSettings & settings)
	: RenderableObject(settings)
{
}

void LandRenderable::createMesh()
{
	const float width = 200.f;
	const int cells = 150;
	const int cellsPlusOne = cells + 1;
	const float cellWidth = width / cells;

	const int rand = std::rand();

	glm::vec3 heightMap[cellsPlusOne*cellsPlusOne];
	glm::vec3 cellNormals[cells*cells];

	SimplexNoise noise(1.0f);

	for (int z = 0; z < cellsPlusOne; z++)
		for (int x = 0; x < cellsPlusOne; x++)
		{
			const float X = -width / 2 + x * cellWidth,
				Z = -width / 2 + z * cellWidth;
			const float distance = std::sqrt(X*X + Z*Z);
			const float hillFactor = std::max(std::min(std::min(std::pow(distance / 85, 5), -std::pow(distance / 100, 4) + 1.f), 1.f), 0.f);
			const float Y = ((noise.fractal(3, X / 50 + rand, Z / 50 - rand) / 2 + 0.5f) * 30 + 10) * hillFactor;
			heightMap[z * cellsPlusOne + x] = { X, Y, Z };
		}

	for (int z = 0; z < cells; z++)
		for (int x = 0; x < cells; x++)
		{
			glm::vec3 v0 = heightMap[z*cellsPlusOne + x],
				v1 = heightMap[(z + 1)*cellsPlusOne + x],
				v2 = heightMap[z*cellsPlusOne + x + 1];

			cellNormals[z*cells + x] = glm::cross(glm::normalize(v1 - v0), glm::normalize(v2 - v0));;
		}

	auto &vertices = m_ground.vertices;

	for (int z = 0; z < cellsPlusOne; z++)
		for (int x = 0; x < cellsPlusOne; x++)
		{
			int x_ = x == cells ? x - 1 : x,
				z_ = z == cells ? z - 1 : z;

			BasicVertex vertex;
			vertex.position = heightMap[z*cellsPlusOne + x];
			vertex.color = 1;

			if ((x == 0 && z == 0)
				|| (x == 0 && z == cells)
				|| (x == cells && z == 0)
				|| (x == cells && z == cells))
			{
				vertex.normal = cellNormals[z_*cells + x_];
			}
			else if (x == 0 || x == cells)
			{
				vertex.normal = (cellNormals[(z_ - 1)*cells + x_] + cellNormals[z_*cells + x_]) / 2.f;
			}
			else if (z == 0 || z == cells)
			{
				vertex.normal = (cellNormals[z_*cells + x_ - 1] + cellNormals[z_*cells + x_]) / 2.f;
			}
			else
			{
				vertex.normal = (cellNormals[z_*cells + x_]
					+ cellNormals[(z_ - 1)*cells + x_]
					+ cellNormals[(z_ - 1)*cells + x_ - 1]
					+ cellNormals[z_*cells + x_ - 1]) / 4.f;
			}

			vertices.push_back(vertex);
		}


	for (int i = 0; i < cells; i++)
		for (int j = 0; j < cells; j++)
		{
			auto &indices = m_ground.indices;

			unsigned int v0 = i * cellsPlusOne + j,
				v1 = (i + 1) * cellsPlusOne + j,
				v2 = (i + 1) * cellsPlusOne + j + 1,
				v3 = i * cellsPlusOne + j + 1;

			indices.insert(indices.end(), {
				v0, v1, v2,
				v0, v2, v3
			});
		}

	MeshHelper::GLsetup(m_ground);
}

void LandRenderable::pushRender(Model & model) const
{
	if (m_renderSettings.renderBackground) model.pushRender(Model::SOLID_MESH, m_ground.VAO, m_ground.indices.size());
}
