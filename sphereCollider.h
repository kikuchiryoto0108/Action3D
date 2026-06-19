//==============================================================================
//  File   : sphereCollider.h
//==============================================================================
#pragma once
#include "collider.h"

class SphereCollider : public Collider {
private:
    float m_Radius = 1.0f;

public:
    using Collider::Collider;
    ColliderType GetType() const override { return ColliderType::Sphere; }

    void SetRadius(float r) { m_Radius = r; }
    float GetRadius() const { return m_Radius; }
};
