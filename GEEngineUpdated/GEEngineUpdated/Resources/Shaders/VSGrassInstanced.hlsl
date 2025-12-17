cbuffer GrassBuffer : register(b0)
{
    float4x4 VP;
    float4x4 W; // Base world matrix (usually Identity)
    float Time;
    float3 Padding;
};

struct VS_INPUT
{
    // PER-VERTEX DATA (The shape of 1 blade)
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;
    
    // PER-INSTANCE DATA (Where to put this specific blade)
    // Your InstancedMesh class likely sets this up in Slot 1
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
    
    // 1. Start with the local vertex position
    float4 worldPos = input.Pos;

    // 2. Add the Instance Position (Move the blade to its spot)
    worldPos.xyz += input.InstancePos;

    // 3. Apply Wind (Same as before)
    if (input.UV.y < 0.1) // Only move the tip
    {
        float wind = sin(Time * 2.0 + input.InstancePos.x + input.InstancePos.z);
        worldPos.x += wind * 0.2f;
    }

    // 4. Final Transform
    output.Pos = mul(worldPos, VP);
    output.UV = input.UV;
    
    return output;
}