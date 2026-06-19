//==============================================================================
//  File   : boxCollider.h
//==============================================================================
#pragma once
#include "collider.h"

class BoxCollider : public Collider {
private:
    Vector3 m_HalfSize{ 0.5f, 0.5f, 0.5f };

public:
    using Collider::Collider;
    ColliderType GetType() const override { return ColliderType::Box; }

    void SetHalfSize(const Vector3& s) { m_HalfSize = s; }
    void SetSize(const Vector3& s) {
        m_HalfSize = { s.x * 0.5f, s.y * 0.5f, s.z * 0.5f };
    }
    Vector3 GetHalfSize() const { return m_HalfSize; }

    Vector3 GetMin() const {
        Vector3 c = GetWorldPosition();
        return { c.x - m_HalfSize.x, c.y - m_HalfSize.y, c.z - m_HalfSize.z };
    }
    Vector3 GetMax() const {
        Vector3 c = GetWorldPosition();
        return { c.x + m_HalfSize.x, c.y + m_HalfSize.y, c.z + m_HalfSize.z };
    }
};
