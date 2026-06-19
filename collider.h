//==============================================================================
//  File   : collider.h
//  Brief  : 衝突判定コンポーネントの基底クラス
//==============================================================================
#pragma once
#include "main.h"
#include "component.h"

enum class CollisionTag {
    None,
    Player,
    Enemy,
    Bullet,
    Wall,
    Tree,
};

enum class ColliderType {
    Sphere,
    Box,
};

class Collider : public Component {
protected:
    Vector3 m_Offset{ 0.0f, 0.0f, 0.0f };
    CollisionTag m_Tag = CollisionTag::None;
    bool m_IsTrigger = false;

public:
    using Component::Component;
    void Init() override {}
    void Uninit() override {}
    void Update() override {}
    void Draw() override {}

    virtual ColliderType GetType() const = 0;

    Vector3 GetWorldPosition() const {
        Vector3 p = m_GameObject->GetPosition();
        return { p.x + m_Offset.x, p.y + m_Offset.y, p.z + m_Offset.z };
    }

    void SetOffset(const Vector3& o) { m_Offset = o; }
    void SetTag(CollisionTag t) { m_Tag = t; }
    CollisionTag GetTag() const { return m_Tag; }
    void SetTrigger(bool t) { m_IsTrigger = t; }
    bool IsTrigger() const { return m_IsTrigger; }
};
