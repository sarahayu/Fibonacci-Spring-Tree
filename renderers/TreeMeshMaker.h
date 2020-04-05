#pragma once

class Tree;
struct TreeMesh;
struct RenderSettings;

namespace TreeMeshMaker
{
	void createBranchesMesh(const Tree &tree, TreeMesh &mesh, const RenderSettings &settings);
	void createLeavesMesh(const Tree &tree, TreeMesh &mesh, const RenderSettings &settings);
}