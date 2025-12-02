#pragma once
#include "core.h"
#include "Mesh.h"
#include "PipeLineState.h"
#include "Shader.h"

class Plane
{
public:
	// Create instance of mesh
	Mesh mesh;

	// Create instance of Pipeline State Manager
	PSOManager psos;

	// Create instance of Shader
	Shader shader;

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
	void init(Core* core)
	{
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-1, 0, -1), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(1, 0, -1), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-1, 0, 1), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(1, 0, 1), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		mesh.init(core, vertices, indices);

		// Load the shaders
		shader.LoadShaders("VertexShader.hlsl", "PixelShader.hlsl");

		// Reflect shaders to populate constant buffer offsets
		shader.ReflectShaders(core, shader.pixelShader, false);
		shader.ReflectShaders(core, shader.vertexShader, true);

		psos.createPSO(
			core,
			"Triangle",
			shader.vertexShader,
			shader.pixelShader,
			mesh.inputLayoutDesc
		);
	}

	// draw function for spinning lights and pulsing triangle
	void draw(Core* core, Matrix& w, Matrix& vp)
	{
		core->beginRenderPass();

		shader.vsConstantBuffers[0]->update("W", &w);
		shader.vsConstantBuffers[0]->update("VP", &vp);

		shader.apply(core);

		psos.bind(core, "Triangle");
		mesh.draw(core);
	}
};
