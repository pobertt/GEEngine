#pragma once
#include "core.h"
#include "Mesh.h"
#include "PipeLineState.h"
#include "Shader.h"

class Plane
{
public:
	Mesh mesh;
	std::string shaderName;
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		Frame frame;
		frame.fromVector(n);
		v.tangent = frame.u;
		v.tu = tu;
		v.tv = tv;
		return v;
	}
	void init(Core* core, PSOManager* psos, Shaders* shaders)
	{
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-1, 0, -1), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(1, 0, -1), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-1, 0, 1), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(1, 0, 1), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);
		mesh.init(core, vertices, indices);
		shaders->load(core, "StaticModelUntextured", "VS.txt", "PSUntextured.txt");
		shaderName = "StaticModelUntextured";
		psos->createPSO(core, "StaticModelUntexturedPSO", shaders->find("StaticModelUntextured")->vs, shaders->find("StaticModelUntextured")->ps, VertexLayoutCache::getStaticLayout());
	}
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp)
	{
		Matrix planeWorld;
		shaders->updateConstantVS("StaticModelUntextured", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("StaticModelUntextured", "staticMeshBuffer", "W", &planeWorld);
		shaders->apply(core, shaderName);
		psos->bind(core, "StaticModelUntexturedPSO");
		mesh.draw(core);
	}
};
