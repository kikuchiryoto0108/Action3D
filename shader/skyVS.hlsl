#include "common.hlsl"

struct SKY_PS_IN {
    float4 Position : SV_POSITION;
    float3 WorldDir : TEXCOORD0;
};

SKY_PS_IN main(VS_IN input) {
    SKY_PS_IN output;

    float4 worldPos = mul(input.Position, World);
    float4 viewPos = mul(worldPos, View);
    output.Position = mul(viewPos, Projection);

    output.WorldDir = input.Position.xyz;
    return output;
}
