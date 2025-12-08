//#pragma once
//
//#include "core.h"
//#include "Mesh.h"
//#include "PipeLineState.h"
//#include "Shader.h"
//
//class Tree
//{
//public:
//	// Create instance of mesh
//	StaticMesh mesh;
//
//	// Implement plane, 2 triangles
//	void init(Core* core, PSOManager* psos, Shaders* shaders)
//	{
//		mesh.init(core, "Resources/Models/acacia_003.gem");
//		// Load the shaders
//		shaders->load(core, "VertexShader.hlsl", "PixelShader.hlsl");
//
//		// Reflect shaders to populate constant buffer offsets
//		shaders->ReflectShaders(core, shader.pixelShader, false);
//		shader.ReflectShaders(core, shader.vertexShader, true);
//
//		psos.createPSO(
//			core,
//			"Tree",
//			shader.vertexShader,
//			shader.pixelShader,
//			VertexLayoutCache::getStaticLayout()
//		);
//	}
//
//	// draw function for spinning lights and pulsing triangle
//	void draw(Core* core, Matrix& w, Matrix& vp)
//	{
//		core->beginRenderPass();
//
//		shader.vsConstantBuffers[0]->update("W", &w);
//		shader.vsConstantBuffers[0]->update("VP", &vp);
//
//		shader.apply(core);
//
//		psos.bind(core, "Tree");
//		mesh.draw(core);
//	}
//};
