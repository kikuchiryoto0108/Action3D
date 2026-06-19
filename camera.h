#pragma once

#include "gameObject.h"

class Camera : public GameObject {
private:
	Vector3 m_Target{ 0.0f, 0.0f, 0.0f };

	float m_Yaw = 0.0f;       // Y軸まわり回転 (ラジアン)
	float m_Pitch = 0.0f;       // X軸まわり回転 (ラジアン)
	float m_Fov = 1.0f;       // 視野角 (ラジアン) 

	float m_Distance = 5.0f;   // プレイヤーからの距離
	float m_Height = 2.0f;   // 注視点の高さオフセット

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	float GetYaw() const { return m_Rotation.y; }
};
