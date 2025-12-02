#pragma once
#include "ConstantBufferClass.h"
#include <fstream>
#include <sstream>


class Shader
{
public:

	// Vertex and pixel shader - member variables
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	std::vector<ConstantBufferClass *> vsConstantBuffers;
	std::vector<ConstantBufferClass *> psConstantBuffers;

	// Function to read in a file
	string ReadFile(string filename)
	{
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	// pass in shaders by filename
	void LoadShaders(const std::string& vsName, const std::string& psName)
	{
		// Compile Vertex shader
		std::string vsSource = ReadFile(vsName);

		ID3DBlob* status;
		HRESULT hr = D3DCompile(vsSource.c_str(), strlen(vsSource.c_str()), NULL,
			NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);

		// CHeck if vertex shader compiles
		if (FAILED(hr))
		{
			if (status)
				OutputDebugStringA((char*)status->GetBufferPointer());
		}

		// Compile pixel shader
		std::string psSource = ReadFile(psName);

		hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
			NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);

		// CHeck if pixel shader compiles
		if (FAILED(hr))
		{
			if (status)
				OutputDebugStringA((char*)status->GetBufferPointer());
		}
	}

	void ReflectShaders(Core *core, ID3DBlob* shader, bool isVS)
	{
		ID3D12ShaderReflection* reflection;
		D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_PPV_ARGS(&reflection));
		D3D12_SHADER_DESC desc;
		reflection->GetDesc(&desc);

		for (int i = 0; i < desc.ConstantBuffers; i++)
		{
			ConstantBufferClass *buffer = new ConstantBufferClass();
			ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC cbDesc;
			constantBuffer->GetDesc(&cbDesc);
			buffer->name = cbDesc.Name;
			unsigned int totalSize = 0;

			for (int j = 0; j < cbDesc.Variables; j++)
			{
				ID3D12ShaderReflectionVariable* var = constantBuffer->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC vDesc;
				var->GetDesc(&vDesc);
				ConstantBufferVariable bufferVariable;
				bufferVariable.offset = vDesc.StartOffset;
				bufferVariable.size = vDesc.Size;
				buffer->constantBufferData.insert({ vDesc.Name, bufferVariable });
				totalSize += bufferVariable.size;
			}
			buffer->init(core, totalSize, 1024);
			if (isVS)
			{
				vsConstantBuffers.push_back(buffer);
			}
			else
			{
				psConstantBuffers.push_back(buffer);
			}
		}
		reflection->Release();
	}

	void apply(Core* core)
	{
		for (int i = 0; i < vsConstantBuffers.size(); i++)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(0, vsConstantBuffers[i]->getGPUAddress());
			vsConstantBuffers[i]->next();
		}

		for (int i = 0; i < psConstantBuffers.size(); i++)
		{
			core->getCommandList()->SetGraphicsRootConstantBufferView(1, psConstantBuffers[i]->getGPUAddress());
			psConstantBuffers[i]->next();
		}
	}
};