#include "common.hlsl"

PS_IN main(VS_IN vi) {
    PS_IN po;
    float4 wp = float4(vi.Position.xyz, 1.0);
    float4 vp = mul(wp, View);
    po.Position = mul(vp, Projection);
    po.Diffuse = vi.Diffuse; // Å© ñºëOÇçáÇÌÇπÇÈ
    po.TexCoord = vi.TexCoord;
    return po;
}
