// PS_Grass.hlsl
Texture2D tex : register(t0);
SamplerState samplerLinear : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
    float3 worldPos : POSITION;
};

float4 PS(PS_INPUT input) : SV_Target0
{
    float4 color = tex.Sample(samplerLinear, input.texCoord);
    
    // Alpha Clipping: If pixel is transparent, don't draw it (Grass blades)
    if (color.a < 0.5f)
        discard;

    return color;
}