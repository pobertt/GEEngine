Texture2D tex : register(t0);
SamplerState samplerLinear : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 pixelColor = tex.Sample(samplerLinear, input.TexCoords);
    clip(pixelColor.a - 0.5f);
    return pixelColor;
}