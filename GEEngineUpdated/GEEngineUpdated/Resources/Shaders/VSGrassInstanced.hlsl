cbuffer GrassBuffer : register(b0)
{
    float4x4 VP;
    float4x4 W;
    float Time;
    float3 Padding;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;
    float3 InstancePos : INSTANCEPOS;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = input.Pos;

    worldPos.xyz += input.InstancePos;

    if (input.UV.y < 0.1)
    {
        float wind = sin(Time * 2.0 + input.InstancePos.x + input.InstancePos.z);
        worldPos.x += wind * 0.2f;
    }

    // 4. Final Transform
    output.Pos = mul(worldPos, VP);
    output.UV = input.UV;
    
    return output;
}