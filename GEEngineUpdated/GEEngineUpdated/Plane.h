#pragma once
#include "core.h"
#include "Mesh.h"
#include "PipeLineState.h"
#include "Shader.h"

//class Plane
//{
//public:
//	// Create instance of mesh
//	Mesh mesh;
//
//	// Create instance of Pipeline State Manager
//	PSOManager psos;
//
//	// Create instance of Shader
//	Shader shader;
//
//	// Helper function for plane
//	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
//	{
//		STATIC_VERTEX v;
//		v.pos = p;
//		v.normal = n;
//		v.tangent = Vec3(0, 0, 0); // For now
//		v.tu = tu;
//		v.tv = tv;
//		return v;
//	}
//
//	// Implement plane, 2 triangles
//	void init(Core* core)
//	{
//		std::vector<STATIC_VERTEX> vertices;
//		vertices.push_back(addVertex(Vec3(-1, 0, -1), Vec3(0, 1, 0), 0, 0));
//		vertices.push_back(addVertex(Vec3(1, 0, -1), Vec3(0, 1, 0), 1, 0));
//		vertices.push_back(addVertex(Vec3(-1, 0, 1), Vec3(0, 1, 0), 0, 1));
//		vertices.push_back(addVertex(Vec3(1, 0, 1), Vec3(0, 1, 0), 1, 1));
//		std::vector<unsigned int> indices;
//		indices.push_back(2); indices.push_back(1); indices.push_back(0);
//		indices.push_back(1); indices.push_back(2); indices.push_back(3);
//		mesh.init(core, vertices, indices);
//
//		// Load the shaders
//		shader.LoadShaders("VertexShader.hlsl", "PixelShader.hlsl");
//
//		// Reflect shaders to populate constant buffer offsets
//		shader.ReflectShaders(core, shader.pixelShader, false);
//		shader.ReflectShaders(core, shader.vertexShader, true);
//
//		psos.createPSO(
//			core,
//			"Triangle",
//			shader.vertexShader,
//			shader.pixelShader,
//			mesh.inputLayoutDesc
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
//		psos.bind(core, "Triangle");
//		mesh.draw(core);
//	}
//};
//
//struct FloorSegment
//{
//    Plane plane;
//    Matrix world;
//    float length; // along +z
//};
//
//class Floor
//{
//public:
//    std::vector<FloorSegment> segments;
//    int count = 0;
//    float segmentLength = 20.0f; // each tile length along z
//    float width = 10.0f;         // optional scale on x
//
//    void init(Core* core, int numSegments)
//    {
//        segments.clear();
//        count = numSegments;
//        for (int i = 0; i < count; ++i)
//        {
//            FloorSegment seg;
//            seg.plane.init(core);            // your Plane::init(core) must exist
//            seg.length = segmentLength;
//            seg.world.identity();
//            // Scale plane to desired width/length; translation stacks afterward
//            seg.world.scaling(Vec3(width, 1.0f, segmentLength));
//            seg.world.translation(Vec3(0.0f, 0.0f, i * segmentLength));
//            segments.push_back(seg);
//        }
//    }
//
//    // Recycle the segment that falls behind camera/player
//    void update(const Vec3& playerPos)
//    {
//        for (int i = 0; i < count; ++i)
//        {
//            // If the segment is far behind the player, move it forward by N * segmentLength
//            float segZ = segments[i].world[14]; // m[14] is translation.z in your row-major Matrix
//            if (segZ + segmentLength < playerPos.z - segmentLength)
//            {
//                // Find max z among segments
//                float maxZ = segZ;
//                for (int j = 0; j < count; ++j)
//                {
//                    float z = segments[j].world[14];
//                    if (z > maxZ) maxZ = z;
//                }
//
//                // Move this segment just ahead of the furthest one
//                float newZ = maxZ + segmentLength;
//                Matrix w;
//                w.identity();
//                w.scaling(Vec3(width, 1.0f, segmentLength));
//                w.translation(Vec3(0.0f, 0.0f, newZ));
//                segments[i].world = w;
//            }
//        }
//    }
//
//    void draw(Core* core, Matrix& vp)
//    {
//        for (int i = 0; i < count; ++i)
//        {
//            segments[i].plane.draw(core, segments[i].world, vp);
//        }
//    }
//};
