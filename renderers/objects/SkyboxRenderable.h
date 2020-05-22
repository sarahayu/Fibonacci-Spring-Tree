#pragma once
#include <SFML\Graphics.hpp>
#include <array>
#include "RenderableObject.h"
#include "..\Mesh.h"

class Model;

class SkyboxRenderable : public RenderableObject
{
public:
	SkyboxRenderable(const RenderSettings &settings);

	void createMesh() override;
	void pushRender(Model &model) const override;

private:
	Mesh<glm::vec3> m_skybox;
};