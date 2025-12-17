// 1. UPDATE CBUFFER
// Add the global variables here. These apply to ALL instances.
cbuffer staticMeshBuffer : register(b0)
{
    // float4x4 W; // Not needed, you have it in VS_INPUT
    float4x4 VP;
    
    // --- NEW VARIABLES ---
    float Time;
    float3 WindDirection;
    float WindStrength;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
    float4x4 World : WORLD; // Per-instance matrix
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // 2. CREATE A COPY OF THE POSITION
    // We need a variable we can modify before transforming it.
    float4 animatedLocalPos = input.Pos;

    // 3. CALCULATE ANIMATION LOGIC
    // ---------------------------------------------------------
    
    // A. Height Threshold: Only move vertices higher up (like tree branches)
    // Adjust "1.0f" to match the height of your mesh's trunk.
    float heightThreshold = 200.0f;
    float movementFactor = max(0.0f, animatedLocalPos.y - heightThreshold) * 0.025;
    
    // B. Randomness (CRITICAL FOR INSTANCING)
    // If we only use input.Pos (local space), every tree moves in perfect sync.
    // We use the Instance's World Position (stored in the World Matrix) as a seed.
    // input.World._m30 is the X translation, _m32 is the Z translation.
    float instanceSeed = input.World._m30 + input.World._m32;

    // C. The Wave Formula
    // Combine global time, the instance seed, and the local vertex position
    float wave = sin(Time + instanceSeed + animatedLocalPos.x);
    
    // Calculate the final offset vector
    float3 offset = WindDirection * wave * WindStrength * movementFactor;

    // Apply the offset to our local position copy
    animatedLocalPos.xyz += offset;

    // ---------------------------------------------------------

    // 4. TRANSFORMATIONS
    // IMPORTANT: Use 'animatedLocalPos' instead of 'input.Pos' here!
    
    // Transform the ANIMATED local position by the instance's World Matrix
    output.Pos = mul(animatedLocalPos, input.World);
    
    // Then multiply by View-Projection
    output.Pos = mul(output.Pos, VP);

    // Pass through other data
    output.Normal = mul(input.Normal, (float3x3) input.World); // Cast to 3x3 to remove translation
    output.Tangent = mul(input.Tangent, (float3x3) input.World);
    output.TexCoords = input.TexCoords;

    return output;
}