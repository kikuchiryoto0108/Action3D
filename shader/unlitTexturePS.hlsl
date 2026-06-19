#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

void main(in float4 inPosition : SV_POSITION,
          in float4 inDiffuse : COLOR,
          in float2 inTexCoord : TEXCOORD,
          out float4 outColor : SV_TARGET) {
    float4 texColor = float4(1.0, 1.0, 1.0, 1.0);
    if (Material.TextureEnable)
    {
        texColor = g_Texture.Sample(g_Sampler, inTexCoord);
    }
    outColor = texColor * inDiffuse * Material.Diffuse;
}
