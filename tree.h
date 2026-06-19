#pragma once
#include "gameObject.h"

class Tree : public GameObject {
public:
    void Init() override;
    void Update() override {}
    void Draw() override;
    void DrawShadow() override;
};
