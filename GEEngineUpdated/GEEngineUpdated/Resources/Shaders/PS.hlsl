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
    
    
    float dist = input.Pos.w;
    
    // no skybox fog
    if (dist > 400.0f)
    {
        return pixelColor;
    }

    float3 fogColor = float3(0.5f, 0.6f, 0.7f);
    float fogStart = 10.0f;
    float fogEnd = 50.0f; 

    float fogFactor = saturate((dist - fogStart) / (fogEnd - fogStart));

    float3 finalColor = lerp(pixelColor.rgb, fogColor, fogFactor);
    return float4(finalColor, pixelColor.a);
}