// 1. INPUT: Use Texture2D (Standard Image), not TextureCube
Texture2D SkyMap : register(t0);
SamplerState samplerLinear : register(s0);

// 2. CONSTANTS: PI is needed for the math
static const float PI = 3.14159265359;
static const float TWO_PI = 6.28318530718;

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    
    // We still need the 3D position from the Vertex Shader!
    // This represents the direction the pixel is "looking" at.
    float3 LocalPos : TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
    // 3. NORMALIZE: Get the direction vector
    float3 v = normalize(input.LocalPos);

    // 4. MATH: Cartesian (XYZ) -> Spherical (UV)
    // atan2(z, x) gives the longitude angle (around the equator)
    // asin(y) gives the latitude angle (up and down)
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));

    // 5. SCALE: Convert radians to 0.0 - 1.0 range
    // longitude is -PI to PI -> divide by 2PI -> -0.5 to 0.5
    // latitude is -PI/2 to PI/2 -> divide by PI -> -0.5 to 0.5
    uv *= float2(1.0 / TWO_PI, 1.0 / PI);

    // 6. SHIFT: Move from (-0.5, 0.5) to (0.0, 1.0)
    uv += 0.5;
    
    // IMPORTANT: Fix the vertical flip if your image looks upside down
    // uv.y = 1.0 - uv.y; 

    return SkyMap.Sample(samplerLinear, uv);
}