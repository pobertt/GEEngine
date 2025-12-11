// VS_Instanced.hlsl
cbuffer cameraBuffer : register(b0)
{
    float4x4 VP;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
    
    // Explicitly accept 4 vectors for the matrix to match C++ layout
    float4 w0 : WORLD0;
    float4 w1 : WORLD1;
    float4 w2 : WORLD2;
    float4 w3 : WORLD3;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
    float3 worldPos : POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // Reconstruct the matrix manually
    float4x4 instanceWorld;
    instanceWorld[0] = input.w0;
    instanceWorld[1] = input.w1;
    instanceWorld[2] = input.w2;
    instanceWorld[3] = input.w3;

    // Standard Math
    float4 posWorld = mul(float4(input.pos, 1.0f), instanceWorld);
    output.worldPos = posWorld.xyz;
    output.pos = mul(posWorld, VP);
    
    output.normal = normalize(mul(input.normal, (float3x3) instanceWorld));
    output.tangent = normalize(mul(input.tangent, (float3x3) instanceWorld));
    output.texCoord = input.texCoord;

    return output;
}