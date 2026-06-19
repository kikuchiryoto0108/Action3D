#pragma once

#include "gameObject.h"

class Enemy : public GameObject {
private:
    ID3D11InputLayout* m_VertexLayout;  // 頂点レイアウト
    ID3D11VertexShader* m_VertexShader; // 頂点シェーダー
    ID3D11PixelShader* m_PixelShader;   // ピクセルシェーダー

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    float GetRadius() const { return 1.0f; }
};
