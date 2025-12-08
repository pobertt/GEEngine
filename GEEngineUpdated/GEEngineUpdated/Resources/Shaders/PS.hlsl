/*
cbuffer staticMeshBuffer : register(b0) // binds constant buffer to slot b0
{
    float time;
    float4 lights[6];   // number of lights
};
*/

/*
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Colour : COLOUR;
};
*/

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

/*
float dist(float2 screenspace_pos, float2 light)
{
    return (length(screenspace_pos - light) / (50.0 * abs(cos(time))));
}
*/

/*
float4 PS(PS_INPUT input) : SV_Target0
{
    float3 accumulated = float3(0, 0, 0);
    for (unsigned int i = 0; i < 6; i++)
    {
        accumulated += 1.0 / dist(input.Pos.xy, lights[i]);
    }
    accumulated *= input.Colour;
    return float4(accumulated, 1.0);
}
*/

float4 PS(PS_INPUT input) : SV_Target0
{
    return float4(abs(normalize(input.Normal)) * 0.9f, 1.0);
}