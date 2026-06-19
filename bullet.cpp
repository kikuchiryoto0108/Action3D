#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "bullet.h"
#include "enemy.h"
#include "modelRenderer.h"
#include "explosion.h"
#include "sphereCollider.h"
#include "tree.h"

void Bullet::Init() {
    m_Scale = { 2.0f, 2.0f, 2.0f };

    ModelRenderer* modelRenderer = AddComponent<ModelRenderer>();
    modelRenderer->Load("asset\\model\\bullet.obj");

    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
        "shader\\unlitTextureVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader,
        "shader\\unlitTexturePS.cso");

    auto* col = AddComponent<SphereCollider>();
    col->SetRadius(0.3f);
    col->SetTag(CollisionTag::Bullet);
}

void Bullet::Uninit() {
    m_VertexLayout->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();

    GameObject::Uninit();
}

void Bullet::Update() {
    float dt = Time::GetDeltaTime();

    // à⁄ìÆ
    m_Position += m_Velocity * dt;

    // éıñΩ
    m_LifeTime -= dt;
    if (m_LifeTime <= 0.0f) {
        Manager::DestroyGameObject(this);
        return;
    }

    GameObject::Update();
}

void Bullet::Draw() {
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    Renderer::SetWorldMatrix(GetWorldMatrix());

    GameObject::Draw();
}

void Bullet::OnCollision(GameObject* other) {
    if (IsDestroyed()) return;

    if (Enemy* enemy = dynamic_cast<Enemy*>(other)) {
        Explosion* exp = new Explosion();
        exp->SetPosition(enemy->GetPosition());
        Manager::AddGameObject(exp);

        Manager::DestroyGameObject(enemy);
        Manager::DestroyGameObject(this);
    } else if (dynamic_cast<Tree*>(other)) {
        // ñÿÇ…ìñÇΩÇ¡ÇΩÇÁíeÇæÇØè¡Ç¶ÇÈ
        Manager::DestroyGameObject(this);
    }
}