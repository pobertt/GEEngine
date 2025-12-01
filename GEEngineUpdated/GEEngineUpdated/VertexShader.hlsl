cbuffer staticMeshBuffer
{
    float4x4 W;
    float4x4 VP;
};

// Vertex Shader
struct VS_INPUT
{
    float4 Pos : POSITION;
    //float4 Colour : COLOUR;
    // float4 Pos : POS;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

// Pixel Shader
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    //float3 Colour : COLOUR;
    // float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};
//mul(input.Normal, transpose(inverse((float3x3)W)));

// Passthrough Shader
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = mul(input.Pos, W);
    output.Pos = mul(output.Pos, VP);
    output.Normal = mul(input.Normal, (float3x3) W);
    output.Tangent = mul(input.Tangent, (float3x3) W);
    output.TexCoords = input.TexCoords;
    return output;
}

/*
class ScreenSpaceTriangle     // only comtains one triangle with three vertices
{
public:
	PRIM_VERTEX vertices[3];
	Mesh mesh; // stores vertex buffer

	ScreenSpaceTriangle() {}
	void init(Core* core)
	{
		vertices[0].position = Vec3(0, 1.0f, 0);
		vertices[0].colour = Colour(0, 1.0f, 0);
		vertices[1].position = Vec3(-1.0f, -1.0f, 0);
		vertices[1].colour = Colour(1.0f, 0, 0);
		vertices[2].position = Vec3(1.0f, -1.0f, 0);
		vertices[2].colour = Colour(0, 0, 1.0f);

		//mesh.init(core, &vertices[0], sizeof(PRIM_VERTEX), 3);
	}
	void draw(Core* core)
	{
		mesh.draw(core);
	}
};

class Triangle1
{
public:
	// Vertex and pixel shader - member variables
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	// Instance of triangle
	ScreenSpaceTriangle triangle;
	// create instance of Pipeline State Manager
	PSOManager psos;

	//ConstantBuffer constantBuffer;
	ConstantBufferClass constantBuffer1;
	ConstantBufferClass constantBuffer2;
	// Function to read in a file
	string ReadFile(string filename)
	{
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	void LoadShaders(Core *core)
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
		psos.createPSO(core, "Triangle", vertexShader, pixelShader, triangle.mesh.inputLayoutDesc);
	}

	void init(Core* core)
	{
		triangle.init(core);
		LoadShaders(core);

		ConstantBuffer1 constBufferCPU;
		constBufferCPU.time = 0;

		constantBuffer1.init(core, sizeof(ConstantBuffer1), 3);
		constantBuffer2.init(core, sizeof(ConstantBuffer2), 3);
		
	}

	void draw(Core* core, ConstantBuffer1* cb) 
	{
		core->beginRenderPass();
		constantBuffer1.update(cb, sizeof(ConstantBuffer1), core->frameIndex());
		core->getCommandList()->SetGraphicsRootConstantBufferView(1, 
			constantBuffer1.getGPUAddress(core->frameIndex())
		);
		psos.bind(core, "Triangle");
		triangle.draw(core);

	}

	void draw2(Core* core, ConstantBuffer2* cb)
	{
		core->beginRenderPass();
		constantBuffer2.update(cb, sizeof(ConstantBuffer2), core->frameIndex());
		core->getCommandList()->SetGraphicsRootConstantBufferView(1,
			constantBuffer2.getGPUAddress(core->frameIndex())
		);
		psos.bind(core, "Triangle");
		triangle.draw(core);

	}
};
*/

//struct alignas(16) ConstantBuffer2
//{
//	float time;
//	float padding[3];
//	Vec4 lights[4];
//	dt = timer.dt();
//};

/* old
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)   // similar to main which was done previously
{
	Window win;    // creates an instance object of win

	Core core;

	GamesEngineeringBase::Timer tim;

	ConstantBuffer1 constBufferCPU1 = {};
	ConstantBuffer2 constBufferCPU2 = {};
	

	// Create an instance of Triangle1
	Triangle1 triangle1Draw;

	//Size
	int WIDTH = 1024;
	int HEIGHT = 1024;
	// Load the shaders


	win.initialize("My Window", 1024, 1024);    // creates a window
	core.init(win.hwnd, 1024, 1024);            // intialises graphics engine with a window handle

	triangle1Draw.init(&core);
	while (true)
	{
		core.resetCommandList();        // clears command list
		core.beginFrame();              // begins a new rendering frame

		win.processMessages();           // processes pending windows message

		if (win.keys[VK_ESCAPE] == 1)   // escape key closes the window
		{
			break;
		}

		float dt = tim.dt();

		// Draw the triangle pulsating
		//constBufferCPU1.time += dt;
		//triangle1Draw.draw(&core, &constBufferCPU1);

		

		//DRAW THE TRIANGLE WITH LIGHTS
		constBufferCPU2.time += dt;

		triangle1Draw.draw2(&core, &constBufferCPU2);

		for (int i = 0; i < 4; i++)
		{
			float angle = constBufferCPU2.time + (i * M_PI / 2.0f);
			constBufferCPU2.lights[i] = Vec4(WIDTH / 2.0f + (cosf(angle) * (WIDTH * 0.3f)), HEIGHT / 2.0f + (sinf(angle) * (HEIGHT * 0.3f)), 0, 0);
		}
		 
		core.finishFrame();             // finished rendering

	}


	core.flushGraphicsQueue();         // ensures GPU commands finish
	
	return 0;
}
*/