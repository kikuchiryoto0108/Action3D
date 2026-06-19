#pragma once

#include "GameObject.h"

class Polygon2D : public GameObject {
private:
    ID3D11Buffer* m_VertexBuffer;   // 頂点バッファ
    
    ID3D11InputLayout* m_VertexLayout;  // 頂点レイアウト
    ID3D11VertexShader* m_VertexShader; // 頂点シェーダー
    ID3D11PixelShader* m_PixelShader;   // ピクセルシェーダー

    ID3D11ShaderResourceView* m_Texture;    // テクスチャ

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    // 2Dレイヤー指定
    Layer GetLayer() const override { return Layer::UI2D; }

    // 2Dなのでシャドウ生成は不要
    void DrawShadow() override {}
};
