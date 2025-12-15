#pragma once

#include <d3d12.h>
#include <vector>

#include "Core.h"
#include "Math.h"

#include "GEMLoader.h"
#include "Animation.h"
#include "Textures.h"
#include "Shader.h"
#include "Collision.h"

struct STATIC_VERTEX {
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
};

struct ANIMATED_VERTEX {
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
	unsigned int bonesIDs[4];
	float boneWeights[4];
};

struct INSTANCE_VERTEX {
	Matrix w;
};

class VertexLayoutCache {
public:
	static const D3D12_INPUT_LAYOUT_DESC& getStaticLayout() {
		static const D3D12_INPUT_ELEMENT_DESC inputLayoutStatic[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		static const D3D12_INPUT_LAYOUT_DESC desc = { inputLayoutStatic, 4 };
		return desc;
	}

	static const D3D12_INPUT_LAYOUT_DESC& getAnimatedLayout() {
		static const D3D12_INPUT_ELEMENT_DESC inputLayoutAnimated[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		static const D3D12_INPUT_LAYOUT_DESC desc = { inputLayoutAnimated, 6 };
		return desc;
	}

	static const D3D12_INPUT_LAYOUT_DESC& getInstancedLayout() {
		static const D3D12_INPUT_ELEMENT_DESC inputLayoutStaticInstanced[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		};
		static const D3D12_INPUT_LAYOUT_DESC desc = { inputLayoutStaticInstanced, 8 };
		return desc;
	}
};

class Mesh {
public:
	ID3D12Resource* vertexBuffer;
	ID3D12Resource* indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	unsigned int numMeshIndices;
	D3D12_HEAP_PROPERTIES heapprops;

	ID3D12Resource* instanceBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW instanceView;
	unsigned int numInstances = 0;

	BoundingBox boundingBox;

	~Mesh() { clean(); }

	void init(Core* core, void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices) {
		
		memset(&heapprops, 0, sizeof(D3D12_HEAP_PROPERTIES));
		heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapprops.CreationNodeMask = 1;
		heapprops.VisibleNodeMask = 1;

		//create vertex buffer on the heap
		D3D12_RESOURCE_DESC vbDesc;
		memset(&vbDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		vbDesc.Width = numVertices * vertexSizeInBytes;
		vbDesc.Height = 1;
		vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vbDesc.DepthOrArraySize = 1;
		vbDesc.MipLevels = 1;
		vbDesc.SampleDesc.Count = 1;
		vbDesc.SampleDesc.Quality = 0;
		vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//allocate memory 
		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&vertexBuffer));
		//copy vertices using helper function
		core->uploadResource(vertexBuffer, vertices, numVertices * vertexSizeInBytes, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		//fill in view in helper function
		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.StrideInBytes = vertexSizeInBytes;
		vbView.SizeInBytes = numVertices * vertexSizeInBytes;

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
		hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &ibDesc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&indexBuffer));
		core->uploadResource(indexBuffer, indices, numIndices * sizeof(unsigned int), D3D12_RESOURCE_STATE_INDEX_BUFFER);

		ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R32_UINT;
		ibView.SizeInBytes = numIndices * sizeof(unsigned int);

		numMeshIndices = numIndices;
	}

	void init(Core* core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices) {
		init(core, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size());
		inputLayoutDesc = VertexLayoutCache::getStaticLayout();

		boundingBox = BoundingBox();

		for (int i = 0; i < vertices.size(); i++) {
			boundingBox.extend(vertices[i].pos);
		}
	}

	void init(Core* core, std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices) {
		init(core, &vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size());
		inputLayoutDesc = VertexLayoutCache::getAnimatedLayout();

		boundingBox = BoundingBox();
		for (int i = 0; i < vertices.size(); i++) {
			boundingBox.extend(vertices[i].pos);
		}
	}

	//now add another init below your others for static INSTANCED meshes - another overloaded init
	void init(Core* core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices, std::vector<INSTANCE_VERTEX> instances) {

		//first does normal init for mesh class 
		init(core, vertices.data(), sizeof(STATIC_VERTEX), vertices.size(), indices.data(), indices.size());

		//then creates instance buffer on heap
		D3D12_RESOURCE_DESC inbDesc = {};
		memset(&inbDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		inbDesc.Width = instances.size() * sizeof(INSTANCE_VERTEX);
		inbDesc.Height = 1;
		inbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		inbDesc.DepthOrArraySize = 1;
		inbDesc.MipLevels = 1;
		inbDesc.SampleDesc.Count = 1;
		inbDesc.SampleDesc.Quality = 0;
		inbDesc.Alignment = 0;
		inbDesc.Format = DXGI_FORMAT_UNKNOWN;
		inbDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		inbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &inbDesc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&instanceBuffer));
		core->uploadResource(instanceBuffer, instances.data(), instances.size() * sizeof(INSTANCE_VERTEX), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		instanceView.BufferLocation = instanceBuffer->GetGPUVirtualAddress();
		instanceView.StrideInBytes = sizeof(INSTANCE_VERTEX);
		instanceView.SizeInBytes = instances.size() * sizeof(INSTANCE_VERTEX);

		//set correct layout
		inputLayoutDesc = VertexLayoutCache::getInstancedLayout();

		boundingBox = BoundingBox();
		for (int i = 0; i < vertices.size(); i++) {
			boundingBox.extend(vertices[i].pos);
		}
	}

	void draw(Core* core) {
		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->getCommandList()->IASetVertexBuffers(0, 1, &vbView);
		core->getCommandList()->IASetIndexBuffer(&ibView);
		core->getCommandList()->DrawIndexedInstanced(numMeshIndices, 1, 0, 0, 0);
	}

	void drawInstanced(Core* core, UINT numInstances) {
		D3D12_VERTEX_BUFFER_VIEW bufferViews[2] = { vbView, instanceView };

		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->getCommandList()->IASetVertexBuffers(0, 2, bufferViews);
		core->getCommandList()->IASetIndexBuffer(&ibView);
		core->getCommandList()->DrawIndexedInstanced(numMeshIndices, numInstances, 0, 0, 0);
	}


	void clean() {
		if (indexBuffer) indexBuffer->Release();
		if (vertexBuffer) vertexBuffer->Release();
		if (instanceBuffer) instanceBuffer->Release(); // Release new buffer
	}

};


class StaticMesh {
public:
	std::vector<Mesh*> meshes;
	std::vector<std::string> textureFilenames;

	void init(Core* core, std::string filename, TextureManager* textureManager) {
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
			std::string rawPath = gemmeshes[i].material.find("albedo").getValue();

			size_t lastSlash = rawPath.find_last_of("\\/");
			std::string filename = (lastSlash == std::string::npos) ? rawPath : rawPath.substr(lastSlash + 1);

			std::string fullPath = "Resources/Models/Textures/" + filename;

			textureManager->loadTexture(core, rawPath, fullPath);
			textureFilenames.push_back(rawPath);
			mesh->init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}
	}

	void draw(Core* core, Shaders* shaders, TextureManager* textureManager) {
		for (int i = 0; i < meshes.size(); i++) {
			shaders->updateTexturePS(core, "static", "tex", textureManager->find(textureFilenames[i]));
			meshes[i]->draw(core);
		}
	}
};

class AnimatedMesh {
public:
	std::vector<Mesh*> meshes;
	Animation animation;
	std::vector<std::string> textureFilenames;


	void init(Core* core, std::string filename, TextureManager* textureManager) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		loader.load(filename, gemmeshes, gemanimation);

		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh* mesh = new Mesh();
			std::vector<ANIMATED_VERTEX> vertices;

			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++) {
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);
			}
			std::string rawPath = gemmeshes[i].material.find("albedo").getValue();

			size_t lastSlash = rawPath.find_last_of("\\/");
			std::string filename = (lastSlash == std::string::npos) ? rawPath : rawPath.substr(lastSlash + 1);

			std::string fullPath = "Resources/Models/Textures/" + filename;

			textureManager->loadTexture(core, rawPath, fullPath);
			textureFilenames.push_back(rawPath);
			mesh->init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}

		memcpy(&animation.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));

		for (int i = 0; i < gemanimation.bones.size(); i++) {
			Bone bone;
			bone.name = gemanimation.bones[i].name;
			memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
			bone.parentIndex = gemanimation.bones[i].parentIndex;
			animation.skeleton.bones.push_back(bone);
		}

		for (int i = 0; i < gemanimation.animations.size(); i++) {
			std::string name = gemanimation.animations[i].name;
			AnimationSequence aseq;
			aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;

			for (int j = 0; j < gemanimation.animations[i].frames.size(); j++) {
				AnimationFrame frame;

				for (int index = 0; index < gemanimation.animations[i].frames[j].positions.size(); index++) {
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

	void draw(Core* core, Shaders* shaders, TextureManager* textureManager) {
		for (int i = 0; i < meshes.size(); i++) {
			shaders->updateTexturePS(core, "animated", "tex", textureManager->find(textureFilenames[i]));
			meshes[i]->draw(core);
		}
	}
};


class InstancedMesh {
public:
	std::vector<Mesh*> meshes;
	std::vector<std::string> texture_files;

	// Generate random instance transforms internally with minimum spacing in XZ
	void init(Core* core, std::string file, TextureManager* texturemanager, UINT numOfInstances, float minSpacing, float rangeMinX, float rangeMaxX, float rangeMinZ, float rangeMaxZ) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(file, gemmeshes);

		// Simple RNG
		auto frand = [](float a, float b) {
			return a + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (b - a);
		};

		// Generate positions with rejection sampling to enforce spacing in XZ
		std::vector<Vec3> positions;
		positions.reserve(numOfInstances);
		const int maxAttemptsPerInstance = 1000;
		for (UINT n = 0; n < numOfInstances; ++n) {
			bool placed = false;
			for (int attempt = 0; attempt < maxAttemptsPerInstance && !placed; ++attempt) {
				float x = frand(rangeMinX, rangeMaxX);
				float z = frand(rangeMinZ, rangeMaxZ);
				Vec3 candidate(x, 0.0f, z);

				bool ok = true;
				for (const auto& p : positions) {
					float dx = candidate.x - p.x;
					float dz = candidate.z - p.z;
					float dist2 = dx * dx + dz * dz;
					if (dist2 < (minSpacing * minSpacing)) { ok = false; break; }
				}
				if (ok) { positions.push_back(candidate); placed = true; }
			}
			// If we can't place respecting spacing, relax: place anyway to avoid infinite loop.
			if (!placed) {
				positions.push_back(Vec3(frand(rangeMinX, rangeMaxX), 0.0f, frand(rangeMinZ, rangeMaxZ)));
			}
		}

		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh* mesh = new Mesh();
			std::vector<STATIC_VERTEX> vertices;
			vertices.reserve(gemmeshes[i].verticesStatic.size());
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}

			std::vector<INSTANCE_VERTEX> instances(positions.size());
			for (size_t k = 0; k < positions.size(); ++k) {
				Matrix S, T;
				S.identity();
				S.scaling(Vec3(0.01f, 0.01f, 0.01f)); // keep your scale
				T.identity();
				T.translation(positions[k]);
				instances[k].w = T.multiply(S);
			}

			std::string rawPath = gemmeshes[i].material.find("albedo").getValue();
			size_t lastSlash = rawPath.find_last_of("\\/");
			std::string filename = (lastSlash == std::string::npos) ? rawPath : rawPath.substr(lastSlash + 1);
			std::string fullPath = "Resources/Models/Textures/" + filename;
			texturemanager->loadTexture(core, rawPath, fullPath);
			texture_files.push_back(rawPath);

			mesh->init(core, vertices, gemmeshes[i].indices, instances);
			meshes.push_back(mesh);
		}
	}

	void draw(Core* core, Shaders* shaders, TextureManager* textureManager, UINT numOfInstances) {
		for (int i = 0; i < meshes.size(); i++) {
			shaders->updateTexturePS(core, "instanced", "tex", textureManager->find(texture_files[i]));
			meshes[i]->drawInstanced(core, numOfInstances);
		}
	}
};