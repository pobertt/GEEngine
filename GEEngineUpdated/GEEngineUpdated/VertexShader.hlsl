// This must match the structure in ShaderManager::init
cbuffer staticMeshBuffer : register(b0)
{
    float4x4 W;
    float4x4 VP;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 1. Transform Local Position to World Position
    float4 posWorld = mul(float4(input.Pos, 1.0f), W);
    
    // 2. Transform World Position to Clip Space (Camera View)
    output.Pos = mul(posWorld, VP);
    
    // 3. Pass other data to Pixel Shader
    output.Normal = mul(input.Normal, (float3x3) W);
    output.Tangent = mul(input.Tangent, (float3x3) W);
    output.TexCoords = input.TexCoords;
    
    return output;
}