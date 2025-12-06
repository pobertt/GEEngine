#pragma once

#include "core.h"
#include <unordered_map>
#include "string"

// Pipeline State Manager
class PSOManager
{
public:
	std::unordered_map<std::string, ID3D12PipelineState*> psos;

	void createPSO(Core* core, const std::string& name, ID3DBlob* vs, ID3DBlob* ps, D3D12_INPUT_LAYOUT_DESC layout)
	{
		// Avoid creating extra state
		if (psos.find(name) != psos.end())
		{
			OutputDebugStringA(("PSOManager::createPSO - PSO already exists: " + name + "\n").c_str());
			return;
		}

		// Validate inputs early
		if (!core || !core->device || !core->rootSignature) {
			OutputDebugStringA(("PSOManager::createPSO - Core or RootSignature invalid for PSO: " + name + "\n").c_str());
			return;
		}
		if (!vs || !ps) {
			OutputDebugStringA(("PSOManager::createPSO - VS/PS blobs are null for PSO: " + name + "\n").c_str());
			return;
		}
		if (layout.pInputElementDescs == nullptr || layout.NumElements == 0) {
			OutputDebugStringA(("PSOManager::createPSO - Input layout invalid for PSO: " + name + "\n").c_str());
			return;
		}

		// Configure GPU pipeline with shaders, layout and Root Signature
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = layout;
		desc.pRootSignature = core->rootSignature;
		desc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
		desc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

		// Rasterizer
		D3D12_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
		rasterDesc.FrontCounterClockwise = FALSE;
		rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterDesc.DepthClipEnable = TRUE;
		rasterDesc.MultisampleEnable = FALSE;
		rasterDesc.AntialiasedLineEnable = FALSE;
		rasterDesc.ForcedSampleCount = 0;
		rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		desc.RasterizerState = rasterDesc;

		// Depth-stencil
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depthStencilDesc.StencilEnable = FALSE;
		desc.DepthStencilState = depthStencilDesc;

		// Blend
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlend =
		{
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		{
			blendDesc.RenderTarget[i] = defaultRenderTargetBlend;
		}
		desc.BlendState = blendDesc;

		// Render target and samples
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0; // Explicit to avoid driver defaults
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;

		ID3D12PipelineState* pso = nullptr;
		HRESULT hr = core->device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
		if (FAILED(hr) || !pso) {
			// Dump detailed info to diagnose layout/semantic mismatches
			OutputDebugStringA(("CreateGraphicsPipelineState FAILED for PSO '" + name + "' (hr=" + std::to_string(hr) + ")\n").c_str());

			// Print expected VS input signature (from reflection if available, else print provided layout)
			OutputDebugStringA("InputLayout provided to PSO:\n");
			for (UINT i = 0; i < layout.NumElements; ++i) {
				const auto& e = layout.pInputElementDescs[i];
				char buf[256];
				sprintf_s(buf,
					"  [%u] Semantic=%s Index=%u Format=%u Slot=%u Offset=%u\n",
					i, e.SemanticName, e.SemanticIndex, (unsigned)e.Format, e.InputSlot, e.AlignedByteOffset);
				OutputDebugStringA(buf);
			}

			OutputDebugStringA("Hint: Ensure VSAnim.hlsl uses the same semantic names and types as the layout (e.g., BONEIDS=uint4 -> R32G32B32A32_UINT, BONEWEIGHTS=float4 -> R32G32B32A32_FLOAT).\n");
			return;
		}

		// Insert into map
		psos.insert({ name, pso });
		OutputDebugStringA(("PSOManager::createPSO - PSO created: " + name + "\n").c_str());
	}

	bool hasPSO(const std::string& name) const {
		auto it = psos.find(name);
		return it != psos.end() && it->second != nullptr;
	}

	void bind(Core* core, const std::string& name) {
		auto it = psos.find(name);
		if (it == psos.end() || it->second == nullptr) {
			OutputDebugStringA(("PSOManager::bind - PSO not found or null: " + name + "\n").c_str());
			// Dump known PSO names to aid diagnosing name mismatches
			std::string known = "Known PSOs: ";
			for (const auto& kv : psos) {
				known += kv.first + " ";
			}
			known += "\n";
			OutputDebugStringA(known.c_str());
			return;
		}
		if (!core || !core->getCommandList()) {
			OutputDebugStringA(("PSOManager::bind - Core or CommandList invalid for PSO: " + name + "\n").c_str());
			return;
		}
		core->getCommandList()->SetPipelineState(it->second);
	}
};