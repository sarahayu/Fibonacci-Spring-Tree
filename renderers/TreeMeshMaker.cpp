#include "TreeMeshMaker.h"
#include <SFML\Graphics.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "..\RenderSettings.h"
#include "..\Tree.h"
#include "Mesh.h"
#include "..\utils\MathUtil.h"

namespace
{
	const int CYLINDER_FACE_VERT_COUNT = 5,
		BRANCH_VERT_COUNT = CYLINDER_FACE_VERT_COUNT * 2, 
		BRANCH_IND_COUNT = CYLINDER_FACE_VERT_COUNT * 2 * 6;
	
	const std::array<LeafVertex, 4> LEAVES_VERTS = {
		LeafVertex{ glm::vec3{ 0.5f,0.5f,0.f }, glm::vec3{}, glm::vec2{ 1.f,-1.f }, 0.f },
		{ { 0.5f,-0.5f,0.f },{}, { 1.f,0.f }, 0.f },
		{ { -0.5f,-0.5f,0.f },{}, { 0.f,0.f }, 0.f },
		{ { -0.5f,0.5f,0.f },{},  { 0.f,-1.f }, 0.f }
	};
	const std::array<unsigned int, 6> LEAVES_IND = { 0,3,2,0,2,1 };

	const std::array<unsigned int, BRANCH_IND_COUNT> BRANCH_IND = []() {
		std::array<unsigned int, BRANCH_IND_COUNT> indices;
		int offset = 0;
		for (int i = 0; i < CYLINDER_FACE_VERT_COUNT; i++)
		{
			bool last = i == CYLINDER_FACE_VERT_COUNT - 1;

			indices[offset++] = i;
			indices[offset++] = i + CYLINDER_FACE_VERT_COUNT;
			indices[offset++] = last ? CYLINDER_FACE_VERT_COUNT : i + CYLINDER_FACE_VERT_COUNT + 1;

			indices[offset++] = i;
			indices[offset++] = last ? CYLINDER_FACE_VERT_COUNT : i + CYLINDER_FACE_VERT_COUNT + 1;
			indices[offset++] = last ? 0 : i + 1;
		}

		return indices;
	}();

	const std::array<BranchVertex, BRANCH_VERT_COUNT> getBranchVertices(const float &taper)
	{
		const float PI2 = 3.14159265f * 2;
		std::array<BranchVertex, BRANCH_VERT_COUNT> vertices;
		int offset = 0;
		const float yNormalAngle = -std::atanf(1 - taper);
		const glm::mat4 tiltUp = glm::rotate(glm::mat4(1.f), yNormalAngle, { 0.f,0.f,1.f });

		for (int i = 0; i < CYLINDER_FACE_VERT_COUNT; i++)
		{
			int offset2 = offset++;
			float x = std::cos(PI2 / CYLINDER_FACE_VERT_COUNT * i),
				z = std::sin(PI2 / CYLINDER_FACE_VERT_COUNT * i);
			glm::vec3 normal = glm::vec4{ x,0.f,z,1.f };

			vertices[offset2] = { { x, 0.f, z }, normal };
			vertices[offset2 + CYLINDER_FACE_VERT_COUNT] = { { x * taper, 1.f, z * taper }, normal };
		}

		return vertices;
	}
}

void TreeMeshMaker::createBranchesMesh(const Tree & tree, TreeMesh & mesh, const RenderSettings & settings)
{
	auto &vertices = mesh.branches.vertices;
	vertices.clear();
	auto &indices = mesh.branches.indices;
	indices.clear();

	auto branchVerts = getBranchVertices(settings.branchTaper);

	int indexOffset = 0;
	for (const Branch &branch : tree.getBranches())
	{
		const sf::Vector3f position = branch.start;
		const sf::Vector2f rotation = branch.rotation;
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, MathUtil::toGLM3(position));
		model = glm::rotate(model, -rotation.x, { 0.f,1.f,0.f });
		model = glm::rotate(model, rotation.y, { 0.f,0.f,1.f });
		float shrinkScale = std::pow(settings.branchTaper, branch.generation);
		model = glm::scale(model, { shrinkScale, branch.length * 1.05f, shrinkScale });

		for (const auto &vert : branchVerts)
			vertices.push_back({
			model * glm::vec4(vert.position, 1.f),
			glm::mat3(glm::transpose(glm::inverse(model))) * vert.normal
		});

		for (const unsigned int &index : BRANCH_IND)
			indices.push_back(indexOffset + index);

		indexOffset += BRANCH_VERT_COUNT;
	}

}


void TreeMeshMaker::createLeavesMesh(const Tree & tree, TreeMesh & mesh, const RenderSettings & settings)
{
	auto &vertices = mesh.leaves.vertices;
	vertices.clear();
	auto &indices = mesh.leaves.indices;
	indices.clear();

	float faceRotate = 0.f;
	int indexOffset = 0;
	for (const sf::Vector3f &position : tree.getLeaves())
	{
		float density = settings.leafDensity;
		float outAngle = (std::atan2f(position.z, position.x) - PI / 2);

		for (int i = 0; i < 3; i++)
		{
			glm::mat4 model = glm::mat4(1.f);
			model = glm::translate(model, MathUtil::toGLM3(position));
			model = glm::rotate(model, -outAngle + glm::radians((i-1) * 60.f), { 0.f,1.f,0.f });

			model = glm::translate(model, { 0.f, 1.f, 0.f });
			model = glm::rotate(model, glm::radians(-10.f), { 1.f,0.f,0.f });
			model = glm::translate(model, { 0.f, -1.f, 0.f });

			if (i != 1) model = glm::rotate(model, glm::radians(10.f), { 0.f,0.f,1.f });
			model = glm::translate(model, { 0.f, 0.5f, 0.f });
			model = glm::rotate(model, glm::radians(faceRotate), { 0.f,0.f,1.f });
			model = glm::scale(model, glm::vec3(settings.leafDensity));

			for (const auto &vert : LEAVES_VERTS)
				vertices.push_back({
				model * glm::vec4(vert.position, 1.f),
				glm::mat3(glm::transpose(glm::inverse(model))) * glm::normalize(glm::vec3(0.f,0.f,1.f)),
				vert.texCoord,
				position.y
			});

			for (const auto &index : LEAVES_IND)
				indices.push_back(index + indexOffset);

			indexOffset += LEAVES_VERTS.size();
			faceRotate += 130.f;
		}
	}

}