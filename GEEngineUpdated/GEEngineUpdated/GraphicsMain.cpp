#include "window.h"
#include "core.h"
#include "maths.h"
#include "PipeLineState.h"
#include "GamesEngineeringBase.h"
#include "Mesh.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "ConstantBuffer.h"
#include <fstream>
#include <sstream>

using namespace std;

struct PRIM_VERTEX
{
	Vec3 position;
	Colour colour;
};

//Tri pulse
struct alignas(16) ConstantBuffer1
{
	float time;
};

//Tri spinning lighjt
struct alignas(16) ConstantBuffer2
{
	float time;
	float padding[3];
	Vec4 lights[4];
};

class Plane {
public:
	Mesh mesh;
	PSOManager psos;
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;
	ConstantBuffer constantBuffer;
	Shader shader;

	//ConstantBufferClass constantBuffer;

	string ReadFile(string filename)
	{
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	void LoadShaders(Core* core)
	{
		// Compile Vertex shader
		std::string vsSource = ReadFile("VertexShader.hlsl");

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
		std::string psSource = ReadFile("PixelShader.hlsl");

		hr = D3DCompile(psSource.c_str(), strlen(psSource.c_str()), NULL, NULL,
			NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);

		// CHeck if pixel shader compiles
		if (FAILED(hr))
		{
			if (status)
				OutputDebugStringA((char*)status->GetBufferPointer());
		}

		// Create pipeline state
		psos.createPSO(core, "StaticModel", vertexShader, pixelShader, mesh.inputLayoutDesc);
	}


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

	void init(Core* core) {
		constantBuffer.init(core, sizeof(StaticMeshConstantBuffer), 2);

		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-15, 0, -15), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(15, 0, -15), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-15, 0, 15), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(15, 0, 15), Vec3(0, 1, 0), 1, 1));

		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);

		mesh.init(core, vertices, indices);

		std::string vsSource = ReadFile("VertexShader.hlsl");
		std::string psSource = ReadFile("PixelShader.hlsl");

		ID3DBlob* errorBlob = nullptr;

		HRESULT hr = D3DCompile(vsSource.c_str(), vsSource.length(), NULL, NULL, NULL,
			"VS", "vs_5_0", 0, 0, &vertexShader, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			return;
		}

		hr = D3DCompile(psSource.c_str(), psSource.length(), NULL, NULL, NULL,
			"PS", "ps_5_0", 0, 0, &pixelShader, &errorBlob);
		if (FAILED(hr)) {
			if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			return;
		}

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_INPUT_LAYOUT_DESC layoutDesc = {};
		layoutDesc.pInputElementDescs = inputElementDescs;
		layoutDesc.NumElements = 4;

		shader.init(core, vertexShader, pixelShader);

		// 4. Create PSO with the explicit layout
		psos.createPSO(core, "StaticModel", vertexShader, pixelShader, layoutDesc);
	}

	void draw(Core* core, ShaderManager* shaders, Matrix vp) {
		psos.bind(core, "StaticModel");

		// Change 1: Initialize Identity Matrix so the plane isn't collapsed to zero
		Matrix planeWorld;
		planeWorld.identity();

		shaders->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		shaders->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vp);

		// Change 2: BIND the buffer. Without this, the shader reads garbage/null.
		shaders->bind(core);

		mesh.draw(core);
	}
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	Window win;
	Core core;
	GamesEngineeringBase::Timer tim;
	Matrix temp; // Helper for non-static matrix methods

	int WIDTH = 1024;
	int HEIGHT = 1024;
	win.initialize("My Window", WIDTH, HEIGHT);
	core.init(win.hwnd, WIDTH, HEIGHT);


	ShaderManager shaders;
	Plane plane;

	shaders.init(&core);
	plane.init(&core);

	// 45 degree FOV, Aspect Ratio 1.0, Near 1.0, Far 2000.0
	Matrix p = temp.perspectiveProjection(3.14159f / 4.0f, 1.0f, 1.0f, 2000.0f);

	float totalTime = 0.0f;

	// --- MAIN LOOP ---
	while (true)
	{
		// Window Message Processing
		win.processMessages();
		if (win.keys[VK_ESCAPE] == 1) break;

		// Time Update
		float dt = tim.dt();
		totalTime += dt;

		// Calculate camera position rotating around center
		Vec3 from = Vec3(11.0f * cosf(totalTime), 5.0f, 11.0f * sinf(totalTime));
		Vec3 target = Vec3(0.0f, 1.0f, 0.0f); // Look at center (where plane is)
		Vec3 up = Vec3(0.0f, 1.0f, 0.0f);

		// View Matrix
		Matrix v = temp.lookAtMatrix(from, target, up);

		// View-Projection Matrix
		Matrix vp = v.multiply(p);

		core.resetCommandList();
		core.beginFrame(); // Clears screen to blue

		core.getCommandList()->SetGraphicsRootSignature(core.rootSignature);

		plane.draw(&core, &shaders, vp);

		core.finishFrame(); // Present
	}

	// Cleanup
	core.flushGraphicsQueue();
	shaders.destroy();
	// plane.mesh.destroy(); // Ideally cleanup mesh resources too

	return 0;
}

// Need Shader Manager and Implement Reflection
// Shader Manager: Load Shader and Shader Reflection(Access constant buffer with shader you are reflecting)