#pragma once

#include "core.h"
#include "maths.h"
#include "Mesh.h"
#include "PipelineState.h"
#include "Shader.h"


class plane {
public:
	Mesh mesh;
	std::string shaderName;

	void init(Core* core, PSOManager* psos, Shaders* shaders) {
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-2, 0, -2), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(2, 0, -2), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-2, 0, 2), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(2, 0, 2), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);

		shaderName = "plane";
		mesh.init(core, vertices, indices);
		shaders->load(core, "plane", "Resources/Shaders/VS.hlsl", "Resources/Shaders/PS.hlsl");
		psos->createPSO(core, "planePSO", shaders->find("plane")->vs, shaders->find("plane")->ps, VertexLayoutCache::getStaticLayout());
	}

	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv) {
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0);
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp) {
		Matrix world;
		shaders->updateConstantVS("plane", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("plane", "staticMeshBuffer", "W", &world);
		shaders->apply(core, shaderName);
		psos->bind(core, "planePSO");
		mesh.draw(core);
	}
};

class staticModel {
public:
	StaticMesh mesh;
	std::string shaderName;

	void init(Core* core, PSOManager* psos, Shaders* shaders, std::string filename) {
		shaderName = "static";
		mesh.init(core, filename);
		shaders->load(core, "static", "Resources/Shaders/VS.hlsl", "Resources/Shaders/PS.hlsl");
		psos->createPSO(core, "staticPSO", shaders->find("static")->vs, shaders->find("static")->ps, VertexLayoutCache::getStaticLayout());
	}

	void update(Shaders* shaders, Matrix& w) {
		shaders->updateConstantVS("static", "staticMeshBuffer", "W", &w);
	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp) {
		Matrix world;
		world.scaling(Vec3(0.01f, 0.01f, 0.01f));
		world.translation(Vec3(5, 0, 0));
		shaders->updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("static", "staticMeshBuffer", "W", &world);
		shaders->apply(core, shaderName);
		psos->bind(core, "staticPSO");
		mesh.draw(core);
	}
};

class animatedModel {
public:
	AnimatedMesh mesh;
	std::string shaderName;

	void init(Core* core, PSOManager* psos, Shaders* shaders, std::string filename) {
		mesh.init(core, filename);
		shaders->load(core, "animated", "Resources/Shaders/VSAnimated.hlsl", "Resources/Shaders/PS.hlsl");
		psos->createPSO(core, "animatedPSO", shaders->find("animated")->vs, shaders->find("animated")->ps, VertexLayoutCache::getAnimatedLayout());
	}

	void update(Shaders* shaders, Matrix& w) {
		shaders->updateConstantVS("animated", "staticMeshBuffer", "W", &w);
	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* instance, Matrix& vp, Matrix& w) {
		psos->bind(core, "animatedPSO");
		shaders->updateConstantVS("animated", "staticMeshBuffer", "W", &w);
		shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("animated", "staticMeshBuffer", "bones", instance->matrices);
		shaders->apply(core, "animated");
		mesh.draw(core);
	}
};