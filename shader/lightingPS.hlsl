#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

void main(in float4 inPosition : SV_POSITION,
          in float4 inWorldPos : POSITION1,
          in float4 inLightPos : POSITION2,
          in float4 inNormal : NORMAL,
          in float4 inDiffuse : COLOR,
          in float2 inTexCoord : TEXCOORD,
          out float4 outColor : SV_TARGET) {
    // ベース色
    float4 texColor = float4(1.0, 1.0, 1.0, 1.0);
    if (Material.TextureEnable)
    {
        texColor = g_Texture.Sample(g_Sampler, inTexCoord);
    }
    float4 baseColor = texColor * inDiffuse * Material.Diffuse;

    float3 finalColor;

    if (Light.Enable)
    {
        float3 N = normalize(inNormal.xyz);
        float3 L = normalize(-Light.Direction.xyz);
        float3 V = normalize(CameraPosition.xyz - inWorldPos.xyz);
        float3 H = normalize(L + V);

        float NdotL = saturate(dot(N, L));

        //----------------------------------------------------------------------
        // シャドウ判定
        //----------------------------------------------------------------------
        float shadow = 1.0;
        {
            // ライト空間 → UV
            float3 projCoord = inLightPos.xyz / inLightPos.w;
            float2 shadowUV;
            shadowUV.x = projCoord.x * 0.5 + 0.5;
            shadowUV.y = -projCoord.y * 0.5 + 0.5;

            // 深度バイアス（シャドウアクネ防止）
            float bias = 0.001;
            float currentDepth = projCoord.z - bias;

            // シャドウマップの範囲外は影なし
            if (shadowUV.x >= 0.0 && shadowUV.x <= 1.0 &&
                shadowUV.y >= 0.0 && shadowUV.y <= 1.0 &&
                projCoord.z <= 1.0)
            {
                shadow = g_ShadowMap.SampleCmpLevelZero(g_ShadowSampler, shadowUV, currentDepth);
            }
        }

        //----------------------------------------------------------------------
        // ライティング
        //----------------------------------------------------------------------
        // ディフューズ＆スペキュラには影を適用
        float3 diffuse = Light.Diffuse.rgb * NdotL * shadow;

        float NdotH = saturate(dot(N, H));
        float specPower = max(Material.Shininess, 1.0);
        float specTerm = pow(NdotH, specPower) * NdotL * shadow;
        float3 specular = Light.Specular.rgb * Material.Specular.rgb * specTerm;

        // アンビエントとリムは影の影響を受けない（暗くなりすぎ防止）
        float upDot = N.y * 0.5 + 0.5;
        float3 ambient = lerp(Light.Ambient.rgb, Light.AmbientSky.rgb, upDot);

        float NdotV = saturate(dot(N, V));
        float rim = pow(1.0 - NdotV, 3.0);
        float3 rimLight = float3(0.5, 0.7, 1.0) * 0.6 * rim;

        finalColor = baseColor.rgb * (ambient + diffuse) + specular + rimLight;
    }
    else
    {
        finalColor = baseColor.rgb;
    }

    finalColor += Material.Emission.rgb;

    outColor = float4(finalColor, baseColor.a);
}
