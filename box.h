//==============================================================================
//  File   : box.h
//  Brief  : 足場用のボックス
//==============================================================================
#pragma once
#include "gameObject.h"

class Box : public GameObject {
private:
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
    ID3D11InputLayout* m_VertexLayout = nullptr;

public:
    void Init() override;
    void Uninit() override;
    void Draw() override;

    void SetSize(const Vector3& size) { m_Scale = size; }
};
