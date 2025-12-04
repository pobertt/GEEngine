// VSAnim.hlsl

cbuffer staticMeshBuffer : register(b0)
{
    float4x4 W;
    float4x4 VP;
    float4x4 bones[256]; // Array of bone matrices sent from C++
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
    uint4 BoneIDs : BONEIDS; // Must match C++ Layout name
    float4 Weights : BONEWEIGHTS; // Must match C++ Layout name
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoords : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // Calculate Skinned Position
    // Sum of (BoneMatrix * Weight) for all 4 influencing bones
    float4x4 boneTransform = bones[input.BoneIDs[0]] * input.Weights[0];
    boneTransform += bones[input.BoneIDs[1]] * input.Weights[1];
    boneTransform += bones[input.BoneIDs[2]] * input.Weights[2];
    boneTransform += bones[input.BoneIDs[3]] * input.Weights[3];

    // Apply Bone Transform to local position
    float4 posLocal = mul(float4(input.Pos, 1.0f), boneTransform);
    
    // Apply World and View-Projection
    float4 posWorld = mul(posLocal, W);
    output.Pos = mul(posWorld, VP);

    // Transform Normal (Rotate only)
    // Note: Technically should use InverseTranspose of World for non-uniform scale
    float3 normalLocal = mul(input.Normal, (float3x3) boneTransform);
    output.Normal = mul(normalLocal, (float3x3) W);

    output.TexCoords = input.TexCoords;

    return output;
}