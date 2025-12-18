cbuffer SkyboxBuffer : register(b0)
{
    float4x4 ViewProjection;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;

    float3 LocalPos : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    output.LocalPos = input.Pos.xyz;

    output.Pos = mul(input.Pos, ViewProjection);

    output.Pos.z = output.Pos.w;

    return output;
}