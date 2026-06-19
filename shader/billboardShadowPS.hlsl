Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

void main(in float4 inPosition : SV_POSITION,
          in float2 inTexCoord : TEXCOORD) {
    float4 texColor = g_Texture.Sample(g_Sampler, inTexCoord);
    clip(texColor.a - 0.5);
}
