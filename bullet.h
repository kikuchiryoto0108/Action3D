#pragma once
#include "gameObject.h"

class Bullet : public GameObject {
private:
    Vector3 m_Velocity{ 0.0f, 0.0f, 0.0f };
    float   m_LifeTime = 3.0f;  // 3ïbÇ≈è¡Ç¶ÇÈ

    ID3D11InputLayout* m_VertexLayout;
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;
    void OnCollision(GameObject* other) override;

    void SetVelocity(const Vector3& vel) { m_Velocity = vel; }

    float GetRadius() const { return 0.3f; }  // ìñÇΩÇËîªíËÇÃîºåa
};
