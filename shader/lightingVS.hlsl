#include "common.hlsl"

void main(in float4 inPosition : POSITION,
          in float4 inNormal : NORMAL,
          in float4 inDiffuse : COLOR,
          in float2 inTexCoord : TEXCOORD,
          out float4 outPosition : SV_POSITION,
          out float4 outWorldPos : POSITION1,
          out float4 outLightPos : POSITION2,
          out float4 outNormal : NORMAL,
          out float4 outDiffuse : COLOR,
          out float2 outTexCoord : TEXCOORD) {
    // ワールド座標
    float4 worldPos = mul(inPosition, World);
    outWorldPos = worldPos;

    // クリップ空間
    float4 viewPos = mul(worldPos, View);
    outPosition = mul(viewPos, Projection);
    
    // ライト空間座標（シャドウ判定用）
    outLightPos = mul(worldPos, LightViewProjection);
    
    // 法線（ワールド空間）
    float3 worldNormal = mul(inNormal.xyz, (float3x3) World);
    outNormal = float4(normalize(worldNormal), 0.0);

    outDiffuse = inDiffuse;
    outTexCoord = inTexCoord;
}
