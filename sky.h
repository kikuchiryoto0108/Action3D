#pragma once
#include "gameObject.h"

class Sky : public GameObject {
private:
    ID3D11Buffer* m_VertexBuffer = nullptr;
    ID3D11Buffer* m_IndexBuffer = nullptr;
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
    ID3D11InputLayout* m_VertexLayout = nullptr;
    ID3D11ShaderResourceView* m_Texture = nullptr;
    UINT m_IndexCount = 0;

public:
    void Init() override;
    void Uninit() override;
    void Update() override {}
    void Draw() override;
    void DrawShadow() override {}  // ‰e‚Í•s—v
};
