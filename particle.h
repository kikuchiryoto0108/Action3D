// particle.h
#pragma once

struct Particle {
    Vector3 position;
    Vector3 velocity;
    float   life;
    float   lifeMax;
    float   size;
    float   sizeStart, sizeEnd;
    XMFLOAT4 colorStart, colorEnd;
    bool    active;
};
