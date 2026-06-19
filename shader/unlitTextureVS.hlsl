#include "common.hlsl"

void main(in float4 inPosition : POSITION,
          in float4 inNormal : NORMAL,
          in float4 inDiffuse : COLOR,
          in float2 inTexCoord : TEXCOORD,
          out float4 outPosition : SV_POSITION,
          out float4 outDiffuse : COLOR,
          out float2 outTexCoord : TEXCOORD) {
    matrix wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    outPosition = mul(inPosition, wvp);

    outDiffuse = inDiffuse;
    outTexCoord = inTexCoord;
}
