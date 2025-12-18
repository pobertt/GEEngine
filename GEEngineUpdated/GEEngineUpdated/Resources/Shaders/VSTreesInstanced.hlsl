// 1. UPDATE CBUFFER
// Add the global variables here. These apply to ALL instances.
cbuffer staticMeshBuffer : register(b0)
{
    // float4x4 W; // Not needed, you have it in VS_INPUT
    float4x4 VP;
    
    // --- NEW VARIABLES ---
    float Time;
    float3 WindDirection;
    float WindStrength;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
    float4x4 World : WORLD; // Per-instance matrix
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

    float4 animatedLocalPos = input.Pos;

    float heightThreshold = 200.0f;
    float movementFactor = max(0.0f, animatedLocalPos.y - heightThreshold) * 0.025;
    
    float instanceSeed = input.World._m30 + input.World._m32;

    float wave = sin(Time + instanceSeed + animatedLocalPos.x);
    
    float3 offset = WindDirection * wave * WindStrength * movementFactor;

    animatedLocalPos.xyz += offset;

    output.Pos = mul(animatedLocalPos, input.World);
    
    output.Pos = mul(output.Pos, VP);
    
    output.Normal = mul(input.Normal, (float3x3) input.World); 
    output.Tangent = mul(input.Tangent, (float3x3) input.World);
    output.TexCoords = input.TexCoords;

    return output;
}