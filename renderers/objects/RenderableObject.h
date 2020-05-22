#pragma once
#include <memory>
#include <vector>

class Model;
struct RenderSettings;

class RenderableObject
{
public:
	RenderableObject(const RenderSettings &settings);

	virtual void createMesh() = 0;
	virtual void pushRender(Model &model) const = 0;

protected:
	const RenderSettings &m_renderSettings;
};

struct RenderableCollection
{
	void createMeshes();
	void pushRenders(Model &model) const;
	std::vector<std::shared_ptr<RenderableObject>> objects;
};