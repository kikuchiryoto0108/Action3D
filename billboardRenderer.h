#pragma once
#include "component.h"

class BillboardRenderer : public Component {
private:
    static ID3D11Buffer* m_VertexBuffer;  // 共通の板ポリ
    static ID3D11VertexShader* m_VertexShader;
    static ID3D11PixelShader* m_PixelShader;
    static ID3D11InputLayout* m_VertexLayout;
    static int                       m_RefCount;      // 参照カウント

    ID3D11ShaderResourceView* m_Texture = nullptr;
    float                            m_Width = 1.0f;
    float                            m_Height = 1.0f;
    bool                             m_YLock = true;  // Y軸ロックするか

public:
    using Component::Component;

    void Init() override;
    void Uninit() override;
    void Draw() override;

    void LoadTexture(const wchar_t* fileName);
    void SetSize(float w, float h) { m_Width = w; m_Height = h; }
    void SetYLock(bool lock) { m_YLock = lock; }

    float GetWidth() const { return m_Width; }
    float GetHeight() const { return m_Height; }
    ID3D11ShaderResourceView* GetTexture() const { return m_Texture; }
    void DrawGeometry();  // 頂点バッファ描画のみ

private:
    float m_UVOffsetX = 0, m_UVOffsetY = 0;
    float m_UVScaleX = 1, m_UVScaleY = 1;
public:
    void SetUV(float ox, float oy, float sx, float sy) {
        m_UVOffsetX = ox; m_UVOffsetY = oy;
        m_UVScaleX = sx;  m_UVScaleY = sy;
    }
};
