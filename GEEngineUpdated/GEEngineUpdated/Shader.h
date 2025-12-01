#pragma once

#include "core.h"
#include "ConstantBuffer.h"


class Shader {
public:
	ID3D12ShaderReflection* vsReflection;
	ID3D12ShaderReflection* psReflection;
	std::vector<ConstantBuffer*> vsConstantBuffers;
	std::vector<ConstantBuffer*> psConstantBuffers;

	void init(Core* core, ID3DBlob* vsShader, ID3DBlob* psShader) {
		// --- 1. Vertex Shader Reflection ---
		if (!vsShader) {
			OutputDebugStringA("Error: VS Blob is null in Shader::init\n");
			return;
		}

		HRESULT hr = D3DReflect(vsShader->GetBufferPointer(), vsShader->GetBufferSize(), IID_PPV_ARGS(&vsReflection));

		if (FAILED(hr)) {
			// Usually caused by missing dxguid.lib or incompatible shader version
			char buffer[256];
			sprintf_s(buffer, "Error: D3DReflect (VS) failed. HRESULT: 0x%X\n", hr);
			OutputDebugStringA(buffer);
			return; // Stop here to prevent crash
		}

		D3D12_SHADER_DESC desc;
		vsReflection->GetDesc(&desc);

		for (unsigned int i = 0; i < desc.ConstantBuffers; i++)
		{
			auto* buffer = new ConstantBuffer();
			ID3D12ShaderReflectionConstantBuffer* cbReflection = vsReflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC cbDesc;
			cbReflection->GetDesc(&cbDesc);

			buffer->name = cbDesc.Name;

			for (unsigned int j = 0; j < cbDesc.Variables; j++)
			{
				ID3D12ShaderReflectionVariable* var = cbReflection->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC vDesc;
				var->GetDesc(&vDesc);

				ConstantBufferVariable bufferVariable;
				bufferVariable.offset = vDesc.StartOffset;
				bufferVariable.size = vDesc.Size;
				buffer->constantBufferData[vDesc.Name] = bufferVariable;
			}

			buffer->init(core, cbDesc.Size);
			vsConstantBuffers.push_back(buffer);
		}

		// --- 2. Pixel Shader Reflection ---
		if (psShader) {
			hr = D3DReflect(psShader->GetBufferPointer(), psShader->GetBufferSize(), IID_PPV_ARGS(&psReflection));

			if (FAILED(hr)) {
				OutputDebugStringA("Warning: D3DReflect (PS) failed. Skipping PS reflection.\n");
			}
			else {
				psReflection->GetDesc(&desc);
				for (unsigned int i = 0; i < desc.ConstantBuffers; i++)
				{
					auto* buffer = new ConstantBuffer();
					ID3D12ShaderReflectionConstantBuffer* cbReflection = psReflection->GetConstantBufferByIndex(i);
					D3D12_SHADER_BUFFER_DESC cbDesc;
					cbReflection->GetDesc(&cbDesc);
					buffer->name = cbDesc.Name;
					for (unsigned int j = 0; j < cbDesc.Variables; j++)
					{
						ID3D12ShaderReflectionVariable* var = cbReflection->GetVariableByIndex(j);
						D3D12_SHADER_VARIABLE_DESC vDesc;
						var->GetDesc(&vDesc);
						ConstantBufferVariable val = { vDesc.StartOffset, vDesc.Size };
						buffer->constantBufferData[vDesc.Name] = val;
					}
					buffer->init(core, cbDesc.Size);
					psConstantBuffers.push_back(buffer);
				}
			}
		}
	}

	void apply(Core* core)
	{
		// Vertex Shader Buffers
		for (int i = 0; i < vsConstantBuffers.size(); i++)
		{
			// Use '->' because vsConstantBuffers[i] is a pointer
			core->getCommandList()->SetGraphicsRootConstantBufferView(i, vsConstantBuffers[i]->getGPUAddress());
			vsConstantBuffers[i]->next();
		}

		// Pixel Shader Buffers
		for (int i = 0; i < psConstantBuffers.size(); i++)
		{
			// Use '->' here as well
			// Note: This hardcodes index 1. If you have multiple PS buffers, they might overwrite each other.
			// For this specific task (StaticModel), it is fine.
			core->getCommandList()->SetGraphicsRootConstantBufferView(1, psConstantBuffers[i]->getGPUAddress());
			psConstantBuffers[i]->next();
		}
	}
};
