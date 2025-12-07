#pragma once
#include "core.h"
#include "Static_Vertex.h"
#include "Animation.h"
#include "Shader.h"
#include "GEMLoader.h"
#include "PipeLineState.h"

struct ANIMATED_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
	unsigned int bonesIDs[4];
	float boneWeights[4];
};

class Mesh
{
public:
	// Vertex Buffer
	ID3D12Resource* vertexBuffer;      // vertex buffer member variable
	D3D12_VERTEX_BUFFER_VIEW vbView;   // view member variable
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout; // Flexible storage
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;     // overall description of layout, array of invididual elements

	// Index Buffer
	ID3D12Resource* indexBuffer;
	D3D12_INDEX_BUFFER_VIEW ibView;
	unsigned int numMeshIndices;

	void init(Core* core, void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices)   // number of bytes per vertex and number of vertices
	{
		// Defensive checks
		if (!core || (!vertices && numVertices > 0) || (!indices && numIndices > 0) || numVertices < 0 || numIndices < 0) {
			OutputDebugStringA("Mesh::init - invalid parameters\n");
			return;
		}

		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapprops.CreationNodeMask = 1;                  // only one GPU
		heapprops.VisibleNodeMask = 1;

		// Create vertex buffer on the heap
		D3D12_RESOURCE_DESC vbDesc = {};
		vbDesc.Width = numVertices * vertexSizeInBytes;
		vbDesc.Height = 1;
		vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vbDesc.DepthOrArraySize = 1;
		vbDesc.MipLevels = 1;
		vbDesc.SampleDesc.Count = 1;
		vbDesc.SampleDesc.Quality = 0;
		vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		// Create index buffer on the heap
		D3D12_RESOURCE_DESC ibDesc;
		memset(&ibDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		ibDesc.Width = numIndices * sizeof(unsigned int);
		ibDesc.Height = 1;
		ibDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ibDesc.DepthOrArraySize = 1;
		ibDesc.MipLevels = 1;
		ibDesc.SampleDesc.Count = 1;
		ibDesc.SampleDesc.Quality = 0;
		ibDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		HRESULT hr;
		hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &ibDesc,
			D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&indexBuffer));
		if (FAILED(hr)) {
			OutputDebugStringA("CreateCommittedResource (index) failed\n");
			return;
		}
		core->uploadResource(indexBuffer, indices, numIndices * sizeof(unsigned int),
			D3D12_RESOURCE_STATE_INDEX_BUFFER);

		// Allocatre memory
		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&vertexBuffer));

		// Copy vertices using our helper function
		core->uploadResource(vertexBuffer, vertices, numVertices * vertexSizeInBytes, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// Fill in vertex buffer view in helper function
		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.StrideInBytes = vertexSizeInBytes;                         // how big each vertex is
		vbView.SizeInBytes = numVertices * vertexSizeInBytes;

		// Fill in index buiffer view in helper function
		ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R32_UINT;
		ibView.SizeInBytes = numIndices * sizeof(unsigned int);
		numMeshIndices = numIndices;

		// Fill in layout (safe initialization)
		inputLayout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		inputLayoutDesc.NumElements = static_cast<UINT>(inputLayout.size());
		inputLayoutDesc.pInputElementDescs = inputLayout.data();
	}

	// WHAT TO DRAW
	// Add commands for drawing
	// Specify type of geometry, where the geometry is (the view), issue command to draw
	void draw(Core* core)
	{
		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->getCommandList()->IASetVertexBuffers(0, 1, &vbView);
		core->getCommandList()->IASetIndexBuffer(&ibView);
		core->getCommandList()->DrawIndexedInstanced(numMeshIndices, 1, 0, 0, 0);
	}


	// Overload function
	void init(Core* core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices)
	{
		init(core, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size());
		// Static mesh layout (matches STATIC_VERTEX and typical VS input)
		inputLayout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		inputLayoutDesc.NumElements = (UINT)inputLayout.size();
		inputLayoutDesc.pInputElementDescs = inputLayout.data();
	}

	void init(Core* core, std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices) {
		// Call base init to setup buffers (it will set up default layout, but we overwrite it below)
		init(core, &vertices[0], sizeof(ANIMATED_VERTEX), static_cast<int>(vertices.size()), &indices[0], static_cast<int>(indices.size()));

		// Ensure animated layout matches VSAnim.hlsl (already correct)
		inputLayout = {
			{ "POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONEIDS",     0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		inputLayoutDesc.NumElements = (UINT)inputLayout.size();
		inputLayoutDesc.pInputElementDescs = inputLayout.data();
	}
};

class StaticMesh {
public:
	std::vector<Mesh*> meshes;

	void init(Core* core, std::string filename) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);

		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh* mesh = new Mesh();
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}
			mesh->init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}
	}

	void draw(Core* core)
	{
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->draw(core);
		}
	}
};

class AnimatedModel
{
public:
	std::vector<Mesh*> meshes;
	Animation animation;
	std::vector<std::string> textureFilenames;
	Shader shader;
	PSOManager psos;
	void init(Core* core, std::string filename)
	{
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		loader.load(filename, gemmeshes, gemanimation);

		// --- SAFETY CHECK 1: File Loading Failed ---
		if (gemmeshes.empty()) {
			OutputDebugStringA(("Error: No meshes found in " + filename + "\n").c_str());
			return;
		}

		for (int i = 0; i < gemmeshes.size(); i++)
		{
			// --- FIX START: Skip empty meshes ---
			if (gemmeshes[i].verticesAnimated.size() == 0) {
				continue; // Skip this mesh to avoid crashing on &vertices[0]
			}
			// --- FIX END ---
			Mesh* mesh = new Mesh();
			std::vector<ANIMATED_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++)
			{
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);
			}
			mesh->init(core, vertices, gemmeshes[i].indices);

			char buf[128];
			sprintf_s(buf, "Animated mesh indices: %u\n", mesh->numMeshIndices);
			OutputDebugStringA(buf);

			meshes.push_back(mesh);
		}

		// --- FIX START: Check if valid meshes exist before using meshes[0] ---
		if (meshes.empty()) {
			OutputDebugStringA("Error: All loaded meshes were empty or invalid.\n");
			return;
		}
		// --- FIX END ---

		// Load Shaders (only the matching pair; no fallback)
		shader.LoadShaders("VSAnim.hlsl", "PSUntextured.hlsl");

		if (!shader.vertexShader || !shader.pixelShader) {
			OutputDebugStringA("CRITICAL: VSAnim.hlsl or PSUntextured.hlsl failed to compile/load. Fix HLSL build settings.\n");
			return;
		}

		// Reflect shaders
		shader.ReflectShaders(core, shader.pixelShader, false);
		shader.ReflectShaders(core, shader.vertexShader, true);

		// Create PSO
		psos.createPSO(
			core,
			"AnimatedModelPSO",
			shader.vertexShader,
			shader.pixelShader,
			meshes[0]->inputLayoutDesc
		);

		if (!psos.hasPSO("AnimatedModelPSO")) {
			OutputDebugStringA("ERROR: AnimatedModelPSO creation failed after shader update.\n");
			return;
		}

		memcpy(&animation.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));
		for (int i = 0; i < gemanimation.bones.size(); i++)
		{
			Bone bone;
			bone.name = gemanimation.bones[i].name;
			memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
			bone.parentIndex = gemanimation.bones[i].parentIndex;
			animation.skeleton.bones.push_back(bone);
		}
		for (int i = 0; i < gemanimation.animations.size(); i++)
		{
			std::string name = gemanimation.animations[i].name;
			AnimationSequence aseq;
			aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
			for (int j = 0; j < gemanimation.animations[i].frames.size(); j++)
			{
				AnimationFrame frame;
				for (int index = 0; index < gemanimation.animations[i].frames[j].positions.size(); index++)
				{
					Vec3 p;
					Quaternion q;
					Vec3 s;
					memcpy(&p, &gemanimation.animations[i].frames[j].positions[index], sizeof(Vec3));
					frame.positions.push_back(p);
					memcpy(&q, &gemanimation.animations[i].frames[j].rotations[index], sizeof(Quaternion));
					frame.rotations.push_back(q);
					memcpy(&s, &gemanimation.animations[i].frames[j].scales[index], sizeof(Vec3));
					frame.scales.push_back(s);
				}
				aseq.frames.push_back(frame);
			}
			animation.animations.insert({ name, aseq });
		}
	}
	void updateWorld(Matrix& w)
	{
		shader.vsConstantBuffers[0]->update("W", &w);
	}
	void draw(Core* core, AnimationInstance* instance, Matrix& vp, Matrix& w)
	{
		// Begin the render pass before setting pipeline and drawing
		core->beginRenderPass();

		// Update constants first
		shader.vsConstantBuffers[0]->update("W", &w);
		shader.vsConstantBuffers[0]->update("VP", &vp);
		shader.vsConstantBuffers[0]->update("bones", instance->matrices);

		// Apply shaders/root signature (keep consistent with other draw paths)
		shader.apply(core);

		// Bind PSO (after apply, same pattern as Tree/Sphere)
		psos.bind(core, "AnimatedModelPSO");

		// Draw
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->draw(core);
		}
	}
};