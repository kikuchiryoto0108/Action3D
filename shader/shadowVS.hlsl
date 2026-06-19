cbuffer WorldBuffer : register(b0) {
    matrix World;
}

cbuffer ShadowBuffer : register(b6) {
    matrix LightViewProjection;
}

void main(in float4 inPosition : POSITION,
          in float4 inNormal : NORMAL,
          in float4 inDiffuse : COLOR,
          in float2 inTexCoord : TEXCOORD,
          out float4 outPosition : SV_POSITION) {
    float4 worldPos = mul(inPosition, World);
    outPosition = mul(worldPos, LightViewProjection);
}
