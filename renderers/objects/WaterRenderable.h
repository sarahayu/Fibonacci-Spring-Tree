#pragma once
#include "RenderableObject.h"
#include "..\Mesh.h"

class Model;

class WaterRenderable : public RenderableObject
{
public:
	WaterRenderable(const RenderSettings &settings);

	void createMesh() override;
	void pushRender(Model &model) const override;

private:
	Mesh<glm::vec3> m_water;
};