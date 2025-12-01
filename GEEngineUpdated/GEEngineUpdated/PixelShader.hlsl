struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_Target
{
    // Return Green (R=0, G=1, B=0, A=1)
    return float4(0.0f, 1.0f, 0.0f, 1.0f);
}