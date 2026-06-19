#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

void main(in float4 inPosition : SV_POSITION,
          in float4 inDiffuse : COLOR,
          in float2 inTexCoord : TEXCOORD,
          out float4 outColor : SV_TARGET) {
    // UVを変換
    float2 uv = inTexCoord * SpriteUV.zw + SpriteUV.xy;
    float4 texColor = g_Texture.Sample(g_Sampler, uv);

    clip(texColor.a - 0.1); // 爆発用にしきい値下げる（0.5→0.1）

    outColor = texColor * inDiffuse * Material.Diffuse;
}
