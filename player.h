#pragma once

#include "gameObject.h"

class Player : public GameObject {
private:
	Vector3 m_Velocity{ 0.0f, 0.0f, 0.0f }; // 移動速度

    ID3D11InputLayout* m_VertexLayout;  // 頂点レイアウト
    ID3D11VertexShader* m_VertexShader; // 頂点シェーダー
    ID3D11PixelShader* m_PixelShader;   // ピクセルシェーダー

    float m_ShotCooldown = 0.0f;  // 連射防止

    bool m_Ground = true;
    float m_MoveAnimation = 0.0f;

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;
    void OnCollision(GameObject* other) override;

    XMMATRIX GetWorldMatrix() const override {
        XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
        XMMATRIX rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y + XM_PI, m_Rotation.z);
        XMMATRIX trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
        return scale * rot * trans;
    }
};
