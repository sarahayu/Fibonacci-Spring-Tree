#pragma once

class Tree;
template <typename T>
struct Mesh;
struct BasicVertex;
struct LeafVertex;
struct RenderSettings;

namespace TreeMeshMaker
{
	void createBranchesMesh(const Tree &tree, Mesh<BasicVertex> &branches, const RenderSettings &settings);
	void createLeavesMesh(const Tree &tree, Mesh<LeafVertex> &leaves, const RenderSettings &settings);
}