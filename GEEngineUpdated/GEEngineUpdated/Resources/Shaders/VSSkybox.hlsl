// --------------------------------------------------------------------------------
// Constant Buffer
// --------------------------------------------------------------------------------
cbuffer SkyboxBuffer : register(b0)
{
    // This combines View and Projection. 
    // REMINDER: In C++, ensure the View matrix has 0,0,0 translation!
    float4x4 ViewProjection;
};

// --------------------------------------------------------------------------------
// Input / Output Structures
// --------------------------------------------------------------------------------
struct VS_INPUT
{
    // We match your "StaticMesh" layout so DirectX doesn't crash
    float4 Pos : POSITION;
    float3 Normal : NORMAL; // Ignored
    float3 Tangent : TANGENT; // Ignored
    float2 UV : TEXCOORD; // Ignored (We calculate our own UVs in the Pixel Shader)
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    
    // This MUST match the struct in your Pixel Shader
    float3 LocalPos : TEXCOORD;
};

// --------------------------------------------------------------------------------
// Vertex Shader Main Function
// --------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // 1. Pass the 3D position to the Pixel Shader
    // We use the LOCAL position (before rotation/movement) as the direction vector.
    // The Pixel Shader uses this to figure out "where on the sphere are we?"
    output.LocalPos = input.Pos.xyz;

    // 2. Calculate Screen Position
    // The matrix handles the rotation (looking around).
    output.Pos = mul(input.Pos, ViewProjection);

    // 3. (Recommended) Depth Hack
    // This forces the skybox to always be at the farthest possible depth (Z=1.0).
    // This allows you to render the skybox LAST, which is faster for performance.
    // NOTE: This requires your C++ DepthStencilState to use D3D11_COMPARISON_LESS_EQUAL.
    output.Pos.z = output.Pos.w;

    return output;
}