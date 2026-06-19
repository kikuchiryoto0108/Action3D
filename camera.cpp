// Todo : 線形補完でカメラの動きを滑らかにする
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "player.h"
#include "keyboard.h"
#include "mouse.h"

void Camera::Init() {
    m_Position = { 0.0f, 1.0f, -5.0f };
    m_Target = { 0.0f, 0.0f,  0.0f };
    m_Yaw = 0.0f;
    m_Pitch = 0.0f;
    m_Fov = XMConvertToRadians(60.0f);
    m_Distance = 5.0f;
    m_Height = 1.5f;

    m_Rotation.x = XMConvertToRadians(15.0f);
    m_Rotation.y = 0.0f;

    // 初期化時に一度だけ Relative モードに
    Mouse::Instance().SetMode(MouseMode::Relative);
}

void Camera::Uninit() {
}

void Camera::Update() {
    float rotSpeed = 0.005f;

    Mouse& mouse = Mouse::Instance();

    // Mキーでマウスモードをトグル
    if (Keyboard_IsKeyDown(KK_M)) {
        if (mouse.GetMode() == MouseMode::Relative) {
            mouse.SetMode(MouseMode::Absolute);
            mouse.SetVisible(true);
        } else {
            mouse.SetMode(MouseMode::Relative);
            mouse.SetVisible(false);
        }
    }

    //--------------------------------------------------------------------------
    // マウスでカメラ回転
    //--------------------------------------------------------------------------
    if (mouse.GetMode() == MouseMode::Relative) {
        float rotSpeed = 0.004f;
        m_Rotation.y += mouse.GetX() * rotSpeed;
        m_Rotation.x += mouse.GetY() * rotSpeed;

        const float pitchMin = XMConvertToRadians(-10.0f);
        const float pitchMax = XMConvertToRadians(50.0f);
        if (m_Rotation.x < pitchMin) m_Rotation.x = pitchMin;
        if (m_Rotation.x > pitchMax) m_Rotation.x = pitchMax;
    }

    //--------------------------------------------------------------------------
    // プレイヤーを Manager から取得して追従
    //--------------------------------------------------------------------------
    Player* player = Manager::GetGameObject<Player>();
    if (player) {
        Vector3 playerPos = player->GetPosition();

        // 注視点 = プレイヤー位置 + 少し上
        XMVECTOR target = XMVectorSet(playerPos.x, playerPos.y + m_Height, playerPos.z, 0.0f);

        // 回転を適用した後方ベクトル
        XMMATRIX rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, 0.0f);
        XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rot);

        // カメラ位置 = 注視点 + 後方 * 距離
        XMVECTOR camPos = XMVectorSubtract(target, XMVectorScale(forward, m_Distance));

        // 地面より下に潜らないようにクランプ
        float camY = XMVectorGetY(camPos);
        if (camY < 0.2f) {
            camPos = XMVectorSetY(camPos, 0.3f);
        }

        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&m_Position), camPos);
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&m_Target), target);
    }
}

void Camera::Draw() {
    // プロジェクション
    XMMATRIX projection = XMMatrixPerspectiveFovLH(m_Fov,
        (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
    Renderer::SetProjectionMatrix(projection);

    // ビュー（m_Target は Update で更新済み）
    XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(
        XMLoadFloat3((XMFLOAT3*)&m_Position),
        XMLoadFloat3((XMFLOAT3*)&m_Target),
        XMLoadFloat3(&up));
    Renderer::SetViewMatrix(view);
    Renderer::SetCameraPosition(m_Position);
}
