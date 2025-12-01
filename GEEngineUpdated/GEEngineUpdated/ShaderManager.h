#pragma once

#include "core.h"
#include "math.h"
#include "shader.h"

struct StaticMeshConstantBuffer {
	Matrix W;
	Matrix VP;
};

class ShaderManager {
public:
	ID3D12Resource* constantBuffer;
	UINT8* mappedData;
	StaticMeshConstantBuffer localData; // Local shadow copy of the data
	Shader shader;

	void init(Core* core) {

		// 1. Calculate buffer size (aligned to 256 bytes)
		int bufferSize = (sizeof(StaticMeshConstantBuffer) + 255) & ~255;

		// 2. Create Upload Heap
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC cbDesc = {};
		cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbDesc.Width = bufferSize;
		cbDesc.Height = 1;
		cbDesc.DepthOrArraySize = 1;
		cbDesc.MipLevels = 1;
		cbDesc.SampleDesc.Count = 1;
		cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		core->device->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &cbDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&constantBuffer));

		// 3. Map memory for CPU writing
		constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));

		// Initialize matrices to Identity to prevent crashes if not set
		localData.W = Matrix();
		localData.VP = Matrix();


	}

	// Matches the syntax used in Slide 23 and 37
	void updateConstantVS(std::string shaderName, std::string bufferName, std::string variableName, void* data) {
		// In a full engine, you would use shaderName/bufferName to find specific buffers.
		// For this task, we assume we are editing our single StaticMesh buffer.

		if (variableName == "W") {
			memcpy(&localData.W, data, sizeof(Matrix));
		}
		else if (variableName == "VP") {
			memcpy(&localData.VP, data, sizeof(Matrix));
		}

		// Upload the updated data to the GPU
		memcpy(mappedData, &localData, sizeof(StaticMeshConstantBuffer));
	}

	// Helper to bind this buffer to the pipeline
	void bind(Core* core) {
		// Assuming the Constant Buffer is at Root Parameter Index 0
		core->getCommandList()->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());
	}

	void destroy() {
		if (constantBuffer) constantBuffer->Release();
	}
};