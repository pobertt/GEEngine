Texture2D tex : register(t0);
SamplerState samplerLinear : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
    // 1. Sample the texture
    float4 color = tex.Sample(samplerLinear, input.UV);
    
    // 2. Alpha Test (The Cutout)
    // "clip(x)" discards the pixel if x < 0
    // So if alpha is less than 0.5, we subtract 0.5 to make it negative -> DISCARD
    clip(color.a - 0.5f);
    
    // Optional: Make it slightly darker at the bottom for fake AO
    // color.rgb *= (0.5 + input.UV.y * 0.5); 

    return color;
}