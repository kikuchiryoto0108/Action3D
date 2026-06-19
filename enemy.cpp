#include "main.h"
#include "renderer.h"
#include "enemy.h"
#include "keyboard.h"
#include "modelRenderer.h"
#include "sphereCollider.h"

void Enemy::Init() {
    m_Position = { 5.0f, 0.0f, 0.0f };

    ModelRenderer* modelRenderer = AddComponent<ModelRenderer>();
    modelRenderer->Load("asset\\model\\player.obj");

    // シェーダー生成
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
        "shader\\unlitTextureVS.cso");

    Renderer::CreatePixelShader(&m_PixelShader,
        "shader\\unlitTexturePS.cso");

    auto* col = AddComponent<SphereCollider>();
    col->SetRadius(1.0f);
    col->SetTag(CollisionTag::Enemy);
}


void Enemy::Uninit() {
    m_VertexLayout->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();

    GameObject::Uninit();
}


void Enemy::Update() {
    GameObject::Update();
}


void Enemy::Draw() {
    // 入力レイアウト設定
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    // シェーダー設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    // マトリクス設定
    XMMATRIX world, scale, rot, trans;
    scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
    rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
    world = scale * rot * trans;

    Renderer::SetWorldMatrix(world);

    GameObject::Draw();
}
