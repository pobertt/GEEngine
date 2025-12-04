#pragma once
#include "core.h"
#include "Mesh.h"
#include "PipeLineState.h"
#include "Shader.h"
#include "maths.h"

class Sphere
{
public:
	// Create instance of mesh
	Mesh mesh;

	// Create instance of Pipeline State Manager
	PSOManager psos;

	// Create instance of Shader
	Shader shader;

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
	void init(Core* core, int rings, int segments, float radius, PSOManager* psos, Shaders* shaders)
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

		shaders->load(core, "StaticModelUntextured", "VertexShader.hlsl", "PixelShader.hlsl");
		shaderName = "StaticModelUntextured";
		psos->createPSO(core, "StaticModelUntexturedPSO", shaders->find("StaticModelUntextured")->vs, shaders->find("StaticModelUntextured")->ps, VertexLayoutCache::getStaticLayout());
	}

	// draw function for spinning lights and pulsing triangle
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
