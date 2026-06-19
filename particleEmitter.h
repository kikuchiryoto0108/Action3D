// particleEmitter.h
#pragma once
#include "component.h"
#include "particle.h"
#include <vector>

class ParticleEmitter : public Component {
private:
    ID3D11ShaderResourceView* m_Texture = nullptr;
    ID3D11VertexShader* m_VS = nullptr;
    ID3D11PixelShader* m_PS = nullptr;
    ID3D11InputLayout* m_Layout = nullptr;
    ID3D11Buffer* m_VertexBuffer = nullptr;

    std::vector<Particle> m_Particles;
    int   m_MaxParticles = 256;

    float m_EmitRate = 30.0f;
    float m_EmitTimer = 0.0f;
    bool  m_Playing = true;
    bool  m_Loop = true;

    Vector3  m_Offset{ 0, 0, 0 };
    float    m_LifeMin = 0.5f, m_LifeMax = 1.0f;
    float    m_SpeedMin = 1.0f, m_SpeedMax = 3.0f;
    Vector3  m_Direction{ 0, 1, 0 };
    float    m_ConeAngle = 30.0f;
    float    m_SizeStart = 0.4f, m_SizeEnd = 0.0f;
    XMFLOAT4 m_ColorStart{ 1.0f, 0.8f, 0.4f, 1.0f };
    XMFLOAT4 m_ColorEnd{ 1.0f, 0.2f, 0.0f, 0.0f };
    Vector3  m_Gravity{ 0, 0, 0 };

public:
    using Component::Component;

    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    void SetTexture(const char* path);
    void SetEmitRate(float r) { m_EmitRate = r; }
    void SetLifeRange(float mn, float mx) { m_LifeMin = mn; m_LifeMax = mx; }
    void SetSpeedRange(float mn, float mx) { m_SpeedMin = mn; m_SpeedMax = mx; }
    void SetDirection(const Vector3& d) { m_Direction = d; }
    void SetConeAngle(float deg) { m_ConeAngle = deg; }
    void SetSizeRange(float s, float e) { m_SizeStart = s; m_SizeEnd = e; }
    void SetColorRange(const XMFLOAT4& s, const XMFLOAT4& e) { m_ColorStart = s; m_ColorEnd = e; }
    void SetGravity(const Vector3& g) { m_Gravity = g; }
    void SetOffset(const Vector3& o) { m_Offset = o; }
    void SetLoop(bool b) { m_Loop = b; }

    void Burst(int count);
    void Play() { m_Playing = true; }
    void Stop() { m_Playing = false; }

private:
    void Emit(int count);
    int  FindDead();
};
