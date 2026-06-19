#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "player.h"
#include "camera.h"
#include "keyboard.h"
#include "modelRenderer.h"
#include "mouse.h"
#include "bullet.h"
#include "sphereCollider.h"
#include "boxCollider.h"
#include "particleEmitter.h"

void Player::Init() {
	m_Position = { 0.0f, 0.0f, 0.0f };

    ModelRenderer* modelRenderer = AddComponent<ModelRenderer>();
	modelRenderer->Load("asset\\model\\player.obj");

    // シェーダー生成
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
        "shader\\unlitTextureVS.cso");

    Renderer::CreatePixelShader(&m_PixelShader,
        "shader\\unlitTexturePS.cso");

    auto* col = AddComponent<SphereCollider>();
    col->SetRadius(0.8f);
    col->SetOffset({ 0.0f, 0.8f, 0.0f }); // 腰くらいの高さに中心
    col->SetTag(CollisionTag::Player);

    auto* em = AddComponent<ParticleEmitter>();
    em->SetTexture("asset\\texture\\particle.png");
    em->SetOffset({ 0, 2.5f, 0 });
    em->SetEmitRate(40.0f);
    em->SetDirection({ 0, 1, 0 });
    em->SetConeAngle(15.0f);
    em->SetSpeedRange(0.5f, 1.5f);
    em->SetLifeRange(0.4f, 0.8f);
    em->SetSizeRange(0.4f, 0.0f);
    em->SetColorRange({ 1.0f, 0.8f, 0.3f, 1.0f }, { 1.0f, 0.2f, 0.0f, 0.0f });
    em->SetGravity({ 0, 1.0f, 0 });
    em->SetLoop(true);
}


void Player::Uninit() {
    m_VertexLayout->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();

	GameObject::Uninit();
}

void Player::Update() {
    float dt = Time::GetDeltaTime();

    //--------------------------------------------------------------------------
    // カメラの向きを取得（Yawのみ使用、Pitchは無視）
    //--------------------------------------------------------------------------
    Camera* camera = Manager::GetGameObject<Camera>();
    float yaw = camera ? camera->GetYaw() : 0.0f;

    // Yawのみで forward / right を計算（地面に水平な移動にする）
    XMMATRIX rotY = XMMatrixRotationY(yaw);
    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotY);
    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotY);

    //--------------------------------------------------------------------------
    // 入力で移動方向を決定
    //--------------------------------------------------------------------------
    XMVECTOR moveDir = XMVectorZero();
    if (Keyboard_IsKeyDown(KK_W)) moveDir = XMVectorAdd(moveDir, forward);
    if (Keyboard_IsKeyDown(KK_S)) moveDir = XMVectorSubtract(moveDir, forward);
    if (Keyboard_IsKeyDown(KK_D)) moveDir = XMVectorAdd(moveDir, right);
    if (Keyboard_IsKeyDown(KK_A)) moveDir = XMVectorSubtract(moveDir, right);

    bool moving = XMVectorGetX(XMVector3LengthSq(moveDir)) > 0.0001f;

    const float moveSpeed = 8.0f; // ニーアくらいのキビキビ感

    if (moving) {
        moveDir = XMVector3Normalize(moveDir);

        // 目標速度
        float vx = XMVectorGetX(moveDir) * moveSpeed;
        float vz = XMVectorGetZ(moveDir) * moveSpeed;

        // 速度を素早く目標値に寄せる（加速）
        const float accelRate = 15.0f;
        m_Velocity.x += (vx - m_Velocity.x) * accelRate * dt;
        m_Velocity.z += (vz - m_Velocity.z) * accelRate * dt;

        //----------------------------------------------------------------------
        // プレイヤーの向きを移動方向にスムーズに回転
        //----------------------------------------------------------------------
        float targetYaw = atan2f(XMVectorGetX(moveDir), XMVectorGetZ(moveDir));

        // 角度差を -π π に正規化
        float diff = targetYaw - m_Rotation.y;
        while (diff > XM_PI) diff -= XM_2PI;
        while (diff < -XM_PI) diff += XM_2PI;

        const float turnSpeed = 15.0f; // 体の向きの追従の速さ
        m_Rotation.y += diff * turnSpeed * dt;
    } else {
        // 入力なしのときは減速（地上でのみ）
        if (m_Position.y <= 0.01f) {
            const float decelRate = 20.0f;
            m_Velocity.x += -m_Velocity.x * decelRate * dt;
            m_Velocity.z += -m_Velocity.z * decelRate * dt;
        }
    }

    //--------------------------------------------------------------------------
    // ジャンプ
    //--------------------------------------------------------------------------
    if (Keyboard_IsKeyDown(KK_SPACE) && m_Position.y <= 0.01f) {
        m_Velocity.y = 15.0f;
        m_Scale.y = 2.0f;
        m_Scale.x = 0.5f;
		m_Scale.z = 0.5f;
    }

	m_Scale.x += (1.0f - m_Scale.x) * 0.1f;
	m_Scale.y += (1.0f - m_Scale.y) * 0.1f;
	m_Scale.z += (1.0f - m_Scale.z) * 0.1f;

    //--------------------------------------------------------------------------
    // 重力
    //--------------------------------------------------------------------------
    m_Velocity.y += -40.0f * dt;

    //--------------------------------------------------------------------------
    // 位置更新
    //--------------------------------------------------------------------------
    m_Position += m_Velocity * dt;

	bool oldGround = m_Ground;
    m_Ground = false;

    // 地面衝突
    if (m_Position.y < 0.0f) {
        m_Position.y = 0.0f;
        m_Velocity.y = 0.0f;
		m_Ground = true;
    }

    if(!oldGround && m_Ground) {
        m_Scale.y = 0.5f;
		m_Scale.x = 2.0f;
		m_Scale.z = 2.0f;
	}

    if (m_Ground) {
        m_MoveAnimation += m_Velocity.length() + dt;
        m_Scale.y += sinf(m_MoveAnimation * 3.0f) * 0.03f;
    }

    //--------------------------------------------------------------------------
    // 弾の発射
    //--------------------------------------------------------------------------
    m_ShotCooldown -= dt;
    if (m_ShotCooldown <= 0.0f && Mouse::Instance().IsPressed(MouseButton::Left)) {
        m_ShotCooldown = 0.15f;  // 0.15秒に1発

        Bullet* bullet = new Bullet();

        // 発射位置：プレイヤーの少し上＆前
        Vector3 spawnPos = m_Position;
        spawnPos.y += 1.0f;

        // 発射方向：プレイヤーの向きに沿って前方
        float bodyYaw = m_Rotation.y;
        Vector3 forward = {
            sinf(bodyYaw),
            0.0f,
            cosf(bodyYaw)
        };

        spawnPos.x += forward.x * 0.5f;
        spawnPos.z += forward.z * 0.5f;

        bullet->SetPosition(spawnPos);

        const float bulletSpeed = 30.0f;
        Vector3 vel = { forward.x * bulletSpeed, 0.0f, forward.z * bulletSpeed };
        bullet->SetVelocity(vel);

        Manager::AddGameObject(bullet);
    }

    GameObject::Update();
}

void Player::Draw() {
    // 入力レイアウト設定
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    // シェーダー設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    Renderer::SetWorldMatrix(GetWorldMatrix());

	GameObject::Draw();
}

void Player::OnCollision(GameObject* other) {
    auto* boxCol = other->GetComponent<BoxCollider>();
    if (!boxCol) return;

    auto* sphereCol = GetComponent<SphereCollider>();
    if (!sphereCol) return;

    Vector3 sc = sphereCol->GetWorldPosition();
    Vector3 bmin = boxCol->GetMin();
    Vector3 bmax = boxCol->GetMax();

    // 球の中心から最も近い箱内の点
    float cx = std::max(bmin.x, std::min(sc.x, bmax.x));
    float cy = std::max(bmin.y, std::min(sc.y, bmax.y));
    float cz = std::max(bmin.z, std::min(sc.z, bmax.z));

    // その点から球の中心への方向と距離
    float dx = sc.x - cx;
    float dy = sc.y - cy;
    float dz = sc.z - cz;
    float distSq = dx * dx + dy * dy + dz * dz;
    float r = sphereCol->GetRadius();

    if (distSq >= r * r) return; // 念のため
    if (distSq < 0.00001f) return; // 完全に内部、別処理が必要

    float dist = sqrtf(distSq);
    float penetration = r - dist;

    // めり込み方向（正規化）
    float nx = dx / dist;
    float ny = dy / dist;
    float nz = dz / dist;

    // 上から接触している場合 → 着地として処理（Yで戻す）
    if (ny > 0.7f) {
        m_Position.y += penetration;
        m_Velocity.y = 0.0f;
		m_Ground = true;
        return;
    }

    // 横からの場合 → XZ方向のみ押し戻し
    m_Position.x += nx * penetration;
    m_Position.z += nz * penetration;
}

