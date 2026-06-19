#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct SKY_PS_IN {
    float4 Position : SV_POSITION;
    float3 WorldDir : TEXCOORD0;
};

static const float INV_PI = 1.0 / 3.14159265359;
static const float INV_2PI = 1.0 / (2.0 * 3.14159265359);

float4 main(SKY_PS_IN input) : SV_TARGET {
    float3 dir = normalize(input.WorldDir);
    float u = atan2(dir.z, dir.x) * INV_2PI + 0.5;
    float v = acos(clamp(dir.y, -1.0, 1.0)) * INV_PI;

    float3 col = g_Texture.Sample(g_Sampler, float2(u, v)).rgb;

    // トーンマッピング
    col = col / (col + 1.0);
    col = pow(col, 1.0 / 2.2);

    return float4(col, 1.0);
}
