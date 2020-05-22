#include "RenderableObject.h"
#include "..\Model.h"

void RenderableCollection::createMeshes()
{
	for (const auto &object : objects) object->createMesh();
}

void RenderableCollection::pushRenders(Model & model) const
{
	for (const auto &object : objects) object->pushRender(model);
}

RenderableObject::RenderableObject(const RenderSettings & settings)
	: m_renderSettings(settings)
{
}
