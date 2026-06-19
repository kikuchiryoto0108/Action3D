#include "main.h"
#include "particleEmitter.h"
#include "gameObject.h"
#include "renderer.h"
using namespace DirectX;

void ParticleEmitter::Init() {
    m_Particles.resize(m_MaxParticles);
    for (auto& p : m_Particles) p.active = false;

    Renderer::CreateVertexShader(&m_VS, &m_Layout, "shader\\particleVS.cso");
    Renderer::CreatePixelShader(&m_PS, "shader\\particlePS.cso");

    // 動的頂点バッファ（1粒子 = 6頂点）
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(VERTEX_3D) * 6 * m_MaxParticles;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Renderer::GetDevice()->CreateBuffer(&bd, nullptr, &m_VertexBuffer);
}

void ParticleEmitter::Uninit() {
    if (m_Texture)      m_Texture->Release();
    if (m_VertexBuffer) m_VertexBuffer->Release();
    if (m_Layout)       m_Layout->Release();
    if (m_VS)           m_VS->Release();
    if (m_PS)           m_PS->Release();
}

void ParticleEmitter::SetTexture(const char* path) {
    if (m_Texture) { m_Texture->Release(); m_Texture = nullptr; }

    wchar_t wpath[256];
    MultiByteToWideChar(CP_ACP, 0, path, -1, wpath, 256);

    ScratchImage image;
    LoadFromWICFile(wpath, WIC_FLAGS_NONE, nullptr, image);
    CreateShaderResourceView(Renderer::GetDevice(),
        image.GetImages(), image.GetImageCount(),
        image.GetMetadata(), &m_Texture);
}

void ParticleEmitter::Emit(int count) {
    for (int i = 0; i < count; i++) {
        int idx = FindDead();
        if (idx < 0) return;

        Particle& p = m_Particles[idx];
        p.active = true;

        p.lifeMax = m_LifeMin + (rand() / (float)RAND_MAX) * (m_LifeMax - m_LifeMin);
        p.life = p.lifeMax;
        p.sizeStart = m_SizeStart;
        p.sizeEnd = m_SizeEnd;
        p.size = m_SizeStart;
        p.colorStart = m_ColorStart;
        p.colorEnd = m_ColorEnd;

        Vector3 base = m_GameObject->GetPosition() + m_Offset;
        p.position = base;

        // Cone 状の方向（軸が上向き(0,1,0)前提の簡易版）
        float speed = m_SpeedMin + (rand() / (float)RAND_MAX) * (m_SpeedMax - m_SpeedMin);
        float angle = m_ConeAngle * 3.14159265f / 180.0f;
        float theta = (rand() / (float)RAND_MAX) * angle;
        float phi = (rand() / (float)RAND_MAX) * 6.28318f;
        Vector3 dir{
            sinf(theta) * cosf(phi),
            cosf(theta),
            sinf(theta) * sinf(phi)
        };
        p.velocity = dir * speed;
    }
}

int ParticleEmitter::FindDead() {
    for (int i = 0; i < (int)m_Particles.size(); i++)
        if (!m_Particles[i].active) return i;
    return -1;
}

void ParticleEmitter::Burst(int count) { Emit(count); }

void ParticleEmitter::Update() {
    float dt = Time::GetDeltaTime();

    if (m_Playing && m_Loop) {
        m_EmitTimer += dt;
        float interval = 1.0f / m_EmitRate;
        while (m_EmitTimer >= interval) {
            Emit(1);
            m_EmitTimer -= interval;
        }
    }

    for (auto& p : m_Particles) {
        if (!p.active) continue;
        p.life -= dt;
        if (p.life <= 0.0f) { p.active = false; continue; }

        p.velocity += m_Gravity * dt;
        p.position += p.velocity * dt;

        float t = 1.0f - (p.life / p.lifeMax);
        p.size = p.sizeStart + (p.sizeEnd - p.sizeStart) * t;
    }
}

void ParticleEmitter::Draw() {
    if (!m_Texture) return;

    // カメラの右・上ベクトルを取り出してビルボード
    XMMATRIX view = Renderer::GetViewMatrix();
    XMVECTOR det;
    XMMATRIX invView = XMMatrixInverse(&det, view);
    XMFLOAT4X4 m;
    XMStoreFloat4x4(&m, invView);
    Vector3 camRight{ m._11, m._12, m._13 };
    Vector3 camUp{ m._21, m._22, m._23 };

    // 頂点バッファに書き込み
    D3D11_MAPPED_SUBRESOURCE mapped;
    Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    VERTEX_3D* vtx = (VERTEX_3D*)mapped.pData;

    int liveCount = 0;
    for (auto& p : m_Particles) {
        if (!p.active) continue;

        // 色を補間
        float t = 1.0f - (p.life / p.lifeMax);
        XMFLOAT4 col{
            p.colorStart.x + (p.colorEnd.x - p.colorStart.x) * t,
            p.colorStart.y + (p.colorEnd.y - p.colorStart.y) * t,
            p.colorStart.z + (p.colorEnd.z - p.colorStart.z) * t,
            p.colorStart.w + (p.colorEnd.w - p.colorStart.w) * t
        };

        float hs = p.size * 0.5f;
        Vector3 r = camRight * hs;
        Vector3 u = camUp * hs;

        Vector3 v0 = p.position - r + u; // 左上
        Vector3 v1 = p.position + r + u; // 右上
        Vector3 v2 = p.position - r - u; // 左下
        Vector3 v3 = p.position + r - u; // 右下

        // 三角形2枚（v0,v1,v2 と v1,v3,v2）
        auto set = [&](int i, const Vector3& pos, float u, float v) {
            vtx[i].Position = XMFLOAT3(pos.x, pos.y, pos.z);
            vtx[i].Normal = XMFLOAT3(0, 0, -1);
            vtx[i].Diffuse = col;
            vtx[i].TexCoord = XMFLOAT2(u, v);
            };
        int base = liveCount * 6;
        set(base + 0, v0, 0, 0);
        set(base + 1, v1, 1, 0);
        set(base + 2, v2, 0, 1);
        set(base + 3, v1, 1, 0);
        set(base + 4, v3, 1, 1);
        set(base + 5, v2, 0, 1);

        liveCount++;
    }
    Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

    if (liveCount == 0) return;

    // 描画設定
    Renderer::SetDepthEnable(false);  // 深度書き込みOFFが望ましい（深度テストはON維持）
    // ★加算ブレンドに切り替える機能が Renderer にあれば呼ぶ。なければ後述。

    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Renderer::GetDeviceContext()->IASetInputLayout(m_Layout);
    Renderer::GetDeviceContext()->VSSetShader(m_VS, nullptr, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PS, nullptr, 0);
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

    // ワールド行列は単位（頂点は既にワールド座標）
    Renderer::SetWorldMatrix(XMMatrixIdentity());

    Renderer::GetDeviceContext()->Draw(liveCount * 6, 0);

    Renderer::SetDepthEnable(true);
}
