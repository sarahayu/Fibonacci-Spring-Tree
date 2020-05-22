#pragma once
#include <SFML\Graphics.hpp>
#include <array>
#include "RenderableObject.h"
#include "..\Mesh.h"

class Model;

class LandRenderable : public RenderableObject
{
public:
	LandRenderable(const RenderSettings &settings);

	void createMesh() override;
	void pushRender(Model &model) const override;

private:
	Mesh<BasicVertex> m_ground;
};