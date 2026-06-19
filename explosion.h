#pragma once
#include "gameObject.h"
class BillboardRenderer;

class Explosion : public GameObject {
private:
    BillboardRenderer* m_Billboard = nullptr;
    float m_Timer = 0.0f;
    float m_Duration = 0.8f;
    int   m_FrameCountX = 4;
    int   m_FrameCountY = 4;
    float m_Size = 3.0f;
public:
    void Init() override;
    void Update() override;
    void DrawShadow() override {}
};