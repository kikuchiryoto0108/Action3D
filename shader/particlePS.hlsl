#include "common.hlsl"

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN pi) : SV_TARGET {
    float4 c = tex.Sample(samp, pi.TexCoord);
    return c * pi.Diffuse; // Å© Ç±Ç¡ÇøÇ‡
}
