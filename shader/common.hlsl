cbuffer WorldBuffer : register(b0) { matrix World; }
cbuffer ViewBuffer  : register(b1) { matrix View; }
cbuffer ProjectionBuffer : register(b2) { matrix Projection; }

cbuffer CameraBuffer : register(b5) {
    float4 CameraPosition;
}

struct MATERIAL {
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float  Shininess;
    bool   TextureEnable;
    float2 Dummy;
};

cbuffer MaterialBuffer : register(b3) {
    MATERIAL Material;
}

struct LIGHT {
    bool   Enable;
    bool3  Dummy;
    float4 Direction;   // 太陽光の向き（ライトから物体への方向）
    float4 Diffuse;     // 太陽光の色
    float4 Ambient;     // 環境光（地面側）
    float4 AmbientSky;  // 環境光（空側） 半球ライト用
    float4 Specular;    // 太陽光のスペキュラ色
};

cbuffer LightBuffer : register(b4) {
    LIGHT Light;
}

cbuffer ShadowBuffer : register(b6) {
    matrix LightViewProjection;
}

cbuffer SpriteBuffer : register(b7) {
    float4 SpriteUV; // xy=offset, zw=scale
}

Texture2D g_ShadowMap : register(t1);
SamplerComparisonState g_ShadowSampler : register(s1);

struct VS_IN {
    float4 Position : POSITION0;
    float4 Normal   : NORMAL0;
    float4 Diffuse  : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct PS_IN {
    float4 Position : SV_POSITION;
    float4 Diffuse  : COLOR0;
    float2 TexCoord : TEXCOORD0;
};
