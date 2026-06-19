#include "main.h"
#include "explosion.h"
#include "billboardRenderer.h"
#include "manager.h"

void Explosion::Init() {
    m_Billboard = AddComponent<BillboardRenderer>();
    m_Billboard->LoadTexture(L"asset\\texture\\explosion.png");
    m_Billboard->SetSize(m_Size, m_Size);
    m_Billboard->SetYLock(false);
}

void Explosion::Update() {
    m_Timer += Time::GetDeltaTime();

    if (m_Timer >= m_Duration) {
        Manager::DestroyGameObject(this);
        return;
    }

    int total = m_FrameCountX * m_FrameCountY;
    int frame = (int)((m_Timer / m_Duration) * total);
    if (frame >= total) frame = total - 1;
    int fx = frame % m_FrameCountX;
    int fy = frame / m_FrameCountX;

    float sx = 1.0f / m_FrameCountX;
    float sy = 1.0f / m_FrameCountY;
    m_Billboard->SetUV(fx * sx, fy * sy, sx, sy);

    GameObject::Update();
}