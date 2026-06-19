#pragma once
#include "gameObject.h"

class Grass : public GameObject {
public:
    void Init() override;
    void Update() override {}
    void Draw() override;
    void DrawShadow() override;
};
