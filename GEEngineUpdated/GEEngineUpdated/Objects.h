#pragma once

#include "core.h"
#include "maths.h"
#include "Mesh.h"
#include "PipelineState.h"
#include "Shader.h"
#include "Textures.h"

class Plane {
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
		shaders->load(core, "plane", "Resources/Shaders/VS.hlsl", "Resources/Shaders/PSSolid.hlsl");
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

	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix& w) {
		shaders->updateConstantVS("plane", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("plane", "staticMeshBuffer", "W", &w);
		shaders->apply(core, shaderName);
		psos->bind(core, "planePSO");
		mesh.draw(core);
	}
};

class Cube
{
public:
	// Create instance of mesh
	Mesh mesh;
	std::string shaderName;


	// Helper function for plane
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	// Implement plane, 2 triangles
	void init(Core* core, PSOManager* psos, Shaders* shaders)
	{
		std::vector<STATIC_VERTEX> vertices;
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);

		vertices.push_back(addVertex(p0, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p3, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));
		vertices.push_back(addVertex(p5, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p4, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));
		vertices.push_back(addVertex(p4, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p0, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p3, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p1, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p2, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));
		vertices.push_back(addVertex(p3, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));
		vertices.push_back(addVertex(p4, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p0, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));


		std::vector<unsigned int> indices;
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(2); indices.push_back(3);
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(4); indices.push_back(6); indices.push_back(7);
		indices.push_back(8); indices.push_back(9); indices.push_back(10);
		indices.push_back(8); indices.push_back(10); indices.push_back(11);
		indices.push_back(12); indices.push_back(13); indices.push_back(14);
		indices.push_back(12); indices.push_back(14); indices.push_back(15);
		indices.push_back(16); indices.push_back(17); indices.push_back(18);
		indices.push_back(16); indices.push_back(18); indices.push_back(19);
		indices.push_back(20); indices.push_back(21); indices.push_back(22);
		indices.push_back(20); indices.push_back(22); indices.push_back(23);

		mesh.init(core, vertices, indices);

		// Load the shaders
		shaders->load(core, "StaticModelUntextured", "Resources/Shaders/VS.hlsl", "Resources/Shaders/PSSolid.hlsl");
		shaderName = "StaticModelUntextured";
		psos->createPSO(core, "StaticModelUntexturedPSO", shaders->find("StaticModelUntextured")->vs, shaders->find("StaticModelUntextured")->ps, VertexLayoutCache::getStaticLayout());
	}

	// draw function for spinning lights and pulsing triangle
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix& w)
	{
		Matrix cubeWorld;
		core->beginRenderPass();

		shaders->updateConstantVS("StaticModelUntextured", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("StaticModelUntextured", "staticMeshBuffer", "W", &w);
		shaders->apply(core, shaderName);
		psos->bind(core, "StaticModelUntexturedPSO");
		mesh.draw(core);
	}
};

class Sphere
{
public:
	// Create instance of mesh
	Mesh mesh;
	std::string shaderName;


	// Helper function for plane
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	// Implement plane, 2 triangles
	void init(Core* core, PSOManager* psos, Shaders* shaders, int rings, int segments, float radius)
	{
		std::vector<STATIC_VERTEX> vertices;
		for (int lat = 0; lat <= rings; lat++) {
			float theta = lat * M_PI / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);
			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * M_PI / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta,
					radius * sinTheta * sinPhi);
				Vec3 normal = position.normalize();
				float tu = 1.0f - (float)lon / segments;
				float tv = 1.0f - (float)lat / rings;
				vertices.push_back(addVertex(position, normal, tu, tv));
			}
		}

		std::vector<unsigned int> indices;
		for (int lat = 0; lat < rings; lat++)
		{
			for (int lon = 0; lon < segments; lon++)
			{
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;
				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);
				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}

		mesh.init(core, vertices, indices);

		// Load the shaders
		shaders->load(core, "StaticModelUntextured", "Resources/Shaders/VS.hlsl", "Resources/Shaders/PSSolid.hlsl");
		shaderName = "StaticModelUntextured";
		psos->createPSO(core, "StaticModelUntexturedPSO", shaders->find("StaticModelUntextured")->vs, shaders->find("StaticModelUntextured")->ps, VertexLayoutCache::getStaticLayout());
	}

	// draw function for spinning lights and pulsing triangle
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp)
	{
		Matrix cubeWorld;
		cubeWorld.scaling(Vec3(20.0f, 20.0f, 20.0f));
		core->beginRenderPass();

		shaders->updateConstantVS("StaticModelUntextured", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("StaticModelUntextured", "staticMeshBuffer", "W", &cubeWorld);
		shaders->apply(core, shaderName);
		psos->bind(core, "StaticModelUntexturedPSO");
		mesh.draw(core);
	}
};

class staticModel {
public:
	StaticMesh mesh;
	std::string shaderName;
	std::vector<std::string> textureFilenames;
	void init(Core* core, PSOManager* psos, Shaders* shaders, std::string filename, TextureManager* textureManager) {
		shaderName = "static";
		mesh.init(core, filename, textureManager);
		shaders->load(core, "static", "Resources/Shaders/VS.hlsl", "Resources/Shaders/PSSolid.hlsl");
		psos->createPSO(core, "staticPSO", shaders->find("static")->vs, shaders->find("static")->ps, VertexLayoutCache::getStaticLayout());
		//texture->load("Resources/Models/Textures/T-rex_Base_Color_alb.png");
	}

	void update(Shaders* shaders, Matrix& w) {
		shaders->updateConstantVS("static", "staticMeshBuffer", "W", &w);
	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix& w, TextureManager* textureManager) {
		
		shaders->updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("static", "staticMeshBuffer", "W", &w);
		shaders->apply(core, shaderName);
		psos->bind(core, "staticPSO");
		mesh.draw(core, shaders, textureManager);
	}
};

class animatedModel {
public:
	AnimatedMesh mesh;
	std::vector<std::string> textureFilenames;

	void init(Core* core, PSOManager* psos, Shaders* shaders, std::string filename, TextureManager* textureManager) {
		mesh.init(core, filename, textureManager);
		shaders->load(core, "animated", "Resources/Shaders/VSAnimated.hlsl", "Resources/Shaders/PS.hlsl");
		psos->createPSO(core, "animatedPSO", shaders->find("animated")->vs, shaders->find("animated")->ps, VertexLayoutCache::getAnimatedLayout());
	}

	void update(Shaders* shaders, Matrix& w) {
		shaders->updateConstantVS("animated", "staticMeshBuffer", "W", &w);
	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* instance, Matrix& vp, Matrix& w, TextureManager* textureManager) {
		psos->bind(core, "animatedPSO");
		shaders->updateConstantVS("animated", "staticMeshBuffer", "W", &w);
		shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("animated", "staticMeshBuffer", "bones", instance->matrices);
		shaders->apply(core, "animated");
		
		mesh.draw(core, shaders, textureManager);
	}
};

class Grass {
public:
	Mesh mesh;
	std::vector<Matrix> instances;

	void init(Core* core, PSOManager* psos, Shaders* shaders, std::string modelFile, int count) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(modelFile, gemmeshes);


		// Init mesh geometry
		std::vector<STATIC_VERTEX> vertices;
		for (int j = 0; j < gemmeshes[0].verticesStatic.size(); j++) {
			STATIC_VERTEX v;
			memcpy(&v, &gemmeshes[0].verticesStatic[j], sizeof(STATIC_VERTEX));
			vertices.push_back(v);
		}
		mesh.init(core, vertices, gemmeshes[0].indices);

		for (int i = 0; i < count; i++) {
			float rX = ((float)rand() / RAND_MAX) * 100.0f - 50.0f; // -50 to 50
			float rZ = ((float)rand() / RAND_MAX) * 100.0f - 50.0f; // -50 to 50
			float rScale = (0.5f + ((float)rand() / RAND_MAX) * 0.5f) * 1; // Random size

			Matrix S, R, T;
			S.scaling(Vec3(rScale, rScale, rScale));
			R.rotAroundY(((float)rand() / RAND_MAX) * 6.28f);
			T.translation(Vec3(rX, 0, rZ));

			instances.push_back(T.multiply(R).multiply(S));
		}

		mesh.initInstances(core, instances);

		shaders->load(core, "GrassInstanced", "Resources/Shaders/VSInstanced.hlsl", "Resources/Shaders/PS.hlsl");

		// Use the NEW Instanced Layout
		psos->createPSO(core, "GrassPSO", shaders->find("GrassInstanced")->vs, shaders->find("GrassInstanced")->ps, VertexLayoutCache::getInstancedLayout());
	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, float dt, TextureManager* texMan) {
		psos->bind(core, "GrassPSO");

		// We ONLY send VP. 'W' is handled automatically per-instance by the vertex buffer!
		shaders->updateConstantVS("GrassInstanced", "staticMeshBuffer", "VP", &vp);

		// Update Time
		static float t = 0; t += dt;
		shaders->updateConstantVS("GrassInstanced", "TimeBuffer", "time", &t);

		shaders->updateTexturePS(core, "GrassInstanced", "tex", texMan->find("GrassTexture")); // Ensure you load "GrassTexture"
		shaders->apply(core, "GrassInstanced");

		// Single efficient draw call
		mesh.drawInstanced(core);
	}
};

class MuzzleFlash {
public:
	float lifeTime = 0.0f;
	float maxLife = 0.05f; // Flash lasts 0.05 seconds
	bool active = false;

	// We reuse your existing Plane mesh for the billboard
	Plane* planeMesh = nullptr;

	// Transform
	Vec3 position;
	float scale = 0.15f;
	float randomRotation = 0.0f;

	void init(Plane* mesh) {
		planeMesh = mesh;
		active = false;
	}

	void activate(Vec3 gunTipPos) {
		position = gunTipPos;
		active = true;
		lifeTime = maxLife;

		// Random Z-rotation makes it look different every shot
		randomRotation = ((float)rand() / RAND_MAX) * 10.0f;
	}

	void update(float dt) {
		if (!active) return;

		lifeTime -= dt;
		if (lifeTime <= 0.0f) {
			active = false;
		}
	}

	// Screen-Aligned Billboard Logic
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, TextureManager* texMan, Vec3 camPos) {
		if (!active || !planeMesh) return;

		Vec3 normal = (camPos - position).normalize();
		Vec3 upRef(0, 1, 0);
		Vec3 right = upRef.Cross(normal).normalize();
		Vec3 up = normal.Cross(right).normalize();

		Matrix rot;
		
		// Simple Billboard Matrix:
		rot.a[0][0] = right.x; rot.a[0][1] = right.y; rot.a[0][2] = right.z;
		rot.a[1][0] = up.x;    rot.a[1][1] = up.y;    rot.a[1][2] = up.z;
		rot.a[2][0] = normal.x; rot.a[2][1] = normal.y; rot.a[2][2] = normal.z;

		// Apply Random Roll (Z-Rotation) to the billboard itself for variety
		Matrix roll;
		roll.rotAroundZ(randomRotation);
		rot = roll.multiply(rot);

		Matrix S, T;
		S.scaling(Vec3(scale, scale, scale));
		T.translation(position);

		Matrix world = T.multiply(rot).multiply(S);

		psos->bind(core, "transparent");
		shaders->updateTexturePS(core, "static", "tex", texMan->find("MuzzleFlashTex"));
		shaders->updateConstantVS("static", "staticMeshBuffer", "W", &world);
		shaders->updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
		shaders->apply(core, "static");

		planeMesh->mesh.draw(core);
	}
};