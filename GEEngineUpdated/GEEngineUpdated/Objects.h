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
		shaders->load(core, "static", "Resources/Shaders/VSSkybox.hlsl", "Resources/Shaders/PS.hlsl");
		shaderName = "static";
		psos->createPSO(core, "staticPSO", shaders->find("static")->vs, shaders->find("static")->ps, VertexLayoutCache::getStaticLayout());
	}

	// draw function for spinning lights and pulsing triangle
	void draw(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* textureManager, Matrix& vp, Matrix& w)
	{
		Matrix cubeWorld;
		core->beginRenderPass();

		shaders->updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("static", "staticMeshBuffer", "W", &w);
		shaders->updateTexturePS(core, "static", "tex", textureManager->find("SkyboxTex"));
		shaders->apply(core, shaderName);
		psos->bind(core, "staticPSO");

		mesh.draw(core);

	}
};

class Skybox
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

	void init(Core* core, PSOManager* psos, Shaders* shaders)
	{
		std::vector<STATIC_VERTEX> vertices;

		// Define the 8 corners of the cube
		float s = 50.0f; // Make it big! (Skybox size)
		Vec3 p0 = Vec3(-s, -s, -s); // Back Bottom Left
		Vec3 p1 = Vec3(s, -s, -s); // Back Bottom Right
		Vec3 p2 = Vec3(s, s, -s); // Back Top Right
		Vec3 p3 = Vec3(-s, s, -s); // Back Top Left
		Vec3 p4 = Vec3(-s, -s, s); // Front Bottom Left
		Vec3 p5 = Vec3(s, -s, s); // Front Bottom Right
		Vec3 p6 = Vec3(s, s, s); // Front Top Right
		Vec3 p7 = Vec3(-s, s, s); // Front Top Left

		// UV Grid Steps (4 columns, 3 rows)
		float u0 = 0.00f;
		float u1 = 0.25f;
		float u2 = 0.50f;
		float u3 = 0.75f;
		float u4 = 1.00f;

		float v0 = 0.00f;
		float v1 = 1.0f / 3.0f; // ~0.333
		float v2 = 2.0f / 3.0f; // ~0.666
		float v3 = 1.00f;

		// --- FACE 1: FRONT (Z+) ---
		// Uses the Center grid (Column 1, Row 1)
		// Mapping: TopLeft(u1, v1) -> BottomRight(u2, v2)
		// Vertices: p7, p6, p5, p4 (TopLeft, TopRight, BotRight, BotLeft)
		vertices.push_back(addVertex(p4, Vec3(0, 0, 1), u1, v2)); // BotLeft
		vertices.push_back(addVertex(p5, Vec3(0, 0, 1), u2, v2)); // BotRight
		vertices.push_back(addVertex(p6, Vec3(0, 0, 1), u2, v1)); // TopRight
		vertices.push_back(addVertex(p7, Vec3(0, 0, 1), u1, v1)); // TopLeft

		// --- FACE 2: BACK (Z-) ---
		// Uses the Rightmost grid (Column 3, Row 1)
		vertices.push_back(addVertex(p1, Vec3(0, 0, -1), u3, v2)); // BotLeft
		vertices.push_back(addVertex(p0, Vec3(0, 0, -1), u4, v2)); // BotRight
		vertices.push_back(addVertex(p3, Vec3(0, 0, -1), u4, v1)); // TopRight
		vertices.push_back(addVertex(p2, Vec3(0, 0, -1), u3, v1)); // TopLeft

		// --- FACE 3: LEFT (X-) ---
		// Uses the Leftmost grid (Column 0, Row 1)
		vertices.push_back(addVertex(p0, Vec3(-1, 0, 0), u0, v2)); // BotLeft
		vertices.push_back(addVertex(p4, Vec3(-1, 0, 0), u1, v2)); // BotRight
		vertices.push_back(addVertex(p7, Vec3(-1, 0, 0), u1, v1)); // TopRight
		vertices.push_back(addVertex(p3, Vec3(-1, 0, 0), u0, v1)); // TopLeft

		// --- FACE 4: RIGHT (X+) ---
		// Uses the Right-Center grid (Column 2, Row 1)
		vertices.push_back(addVertex(p5, Vec3(1, 0, 0), u2, v2)); // BotLeft
		vertices.push_back(addVertex(p1, Vec3(1, 0, 0), u3, v2)); // BotRight
		vertices.push_back(addVertex(p2, Vec3(1, 0, 0), u3, v1)); // TopRight
		vertices.push_back(addVertex(p6, Vec3(1, 0, 0), u2, v1)); // TopLeft

		// --- FACE 5: TOP (Y+) ---
		// Uses the Top grid (Column 1, Row 0)
		vertices.push_back(addVertex(p7, Vec3(0, 1, 0), u1, v1)); // BotLeft
		vertices.push_back(addVertex(p6, Vec3(0, 1, 0), u2, v1)); // BotRight
		vertices.push_back(addVertex(p2, Vec3(0, 1, 0), u2, v0)); // TopRight
		vertices.push_back(addVertex(p3, Vec3(0, 1, 0), u1, v0)); // TopLeft

		// --- FACE 6: BOTTOM (Y-) ---
		// Uses the Bottom grid (Column 1, Row 2)
		vertices.push_back(addVertex(p4, Vec3(0, -1, 0), u1, v2)); // TopLeft (relative to texture)
		vertices.push_back(addVertex(p5, Vec3(0, -1, 0), u2, v2)); // TopRight
		vertices.push_back(addVertex(p1, Vec3(0, -1, 0), u2, v3)); // BotRight
		vertices.push_back(addVertex(p0, Vec3(0, -1, 0), u1, v3)); // BotLeft

		// INDICES: DOUBLE-SIDED (Draws both Inside and Outside)
		std::vector<unsigned int> indices;
		for (int i = 0; i < 6; i++) {
			unsigned int base = i * 4;

			// 1. CLOCKWISE TRIANGLES (Visible from Outside)
			indices.push_back(base + 0);
			indices.push_back(base + 1);
			indices.push_back(base + 2);

			indices.push_back(base + 0);
			indices.push_back(base + 2);
			indices.push_back(base + 3);

			// 2. COUNTER-CLOCKWISE TRIANGLES (Visible from Inside)
			// We just flip the last two numbers
			indices.push_back(base + 0);
			indices.push_back(base + 2);
			indices.push_back(base + 1);

			indices.push_back(base + 0);
			indices.push_back(base + 3);
			indices.push_back(base + 2);
		}

		mesh.init(core, vertices, indices);

		// Load the shaders
		shaders->load(core, "static", "Resources/Shaders/VSSkybox.hlsl", "Resources/Shaders/PS.hlsl");
		shaderName = "static";
		psos->createPSO(core, "staticPSO", shaders->find("static")->vs, shaders->find("static")->ps, VertexLayoutCache::getStaticLayout());
	}

	// draw function for spinning lights and pulsing triangle
	void draw(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* textureManager, Matrix& vp, Matrix& w, Vec3 playerPos)
	{
		Matrix skyboxWorld;
		skyboxWorld.identity();
		skyboxWorld.translation(playerPos);
		core->beginRenderPass();

		shaders->updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("static", "staticMeshBuffer", "W", &skyboxWorld);
		shaders->updateTexturePS(core, "static", "tex", textureManager->find("SkyboxTex"));
		shaders->apply(core, shaderName);
		psos->bind(core, "staticPSO");
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
		shaders->load(core, "static", "Resources/Shaders/VSSkybox.hlsl", "Resources/Shaders/PS.hlsl");
		shaderName = "static";
		psos->createPSO(core, "staticPSO", shaders->find("static")->vs, shaders->find("static")->ps, VertexLayoutCache::getStaticLayout());
	}

	// draw function for spinning lights and pulsing triangle
	void draw(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* textureManager, Matrix& vp, Matrix& w)
	{
		Matrix cubeWorld;
		core->beginRenderPass();

		shaders->updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("static", "staticMeshBuffer", "W", &w);
		shaders->updateTexturePS(core, "static", "tex", textureManager->find("SkyboxTex"));
		shaders->apply(core, shaderName);
		psos->bind(core, "staticPSO");
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


class InstancedModels {
public:
	InstancedMesh mesh;
	std::vector<Vec3> modelPositions;

	const std::vector<Vec3>& getPositions() const { return modelPositions; }

	void init(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* texturemanager, std::string filename, UINT numOfInstances, float minSpacing, float rangeMinX, float rangeMaxX, float rangeMinZ, float rangeMaxZ) {
		mesh.init(core, filename, texturemanager, numOfInstances, minSpacing, rangeMinX, rangeMaxX, rangeMinZ, rangeMaxZ);

		this->modelPositions = mesh.getPositions();
		
		shaders->load(core, "instanced", "Resources/Shaders/VSInstanced.hlsl", "Resources/Shaders/PS.hlsl");
		psos->createPSO(core, "instancedPSO", shaders->find("instanced")->vs, shaders->find("instanced")->ps, VertexLayoutCache::getInstancedLayout());
	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* texturemanager, Matrix& vp, UINT numOfInstances) {
		psos->bind(core, "instancedPSO");
		shaders->updateConstantVS("instanced", "staticMeshBuffer", "VP", &vp);
		

		shaders->apply(core, "instanced");

		mesh.draw(core, shaders, texturemanager, numOfInstances);
	}
};

class InstancedTrees {
public:
	InstancedModels model;
	float time = 0.0f;
	float windStrength = 1.0f;
	Vec3 windDir = Vec3(1.0f, 0.0f, 0.0f);

	BoundingBox collider;

	void init(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* textureManager, std::string filename, UINT numOfInstances, float minSpacing, float rangeMinX, float rangeMaxX, float rangeMinZ, float rangeMaxZ) {
		model.init(core, psos, shaders, textureManager, filename, numOfInstances, minSpacing, rangeMinX, rangeMaxX, rangeMinZ, rangeMaxZ);
	}

	void update(float dt) {
		time += dt;

		if (time > 100.0f) time -= 100.0f;

	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* textureManager, Matrix& vp, UINT numOfInstances) {
		shaders->updateConstantVS("instanced", "staticMeshBuffer", "Time", &time);

		shaders->updateConstantVS("instanced", "staticMeshBuffer", "WindDirection", &windDir);

		shaders->updateConstantVS("instanced", "staticMeshBuffer", "WindStrength", &windStrength);

		model.draw(core, psos, shaders, textureManager, vp, numOfInstances);
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