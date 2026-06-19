#include "main.h"
#include "renderer.h"
#include "grass.h"
#include "billboardRenderer.h"
#include "camera.h"
#include "manager.h"

void Grass::Init() {
    BillboardRenderer* bb = AddComponent<BillboardRenderer>();
    bb->LoadTexture(L"asset\\texture\\grass.png");
    bb->SetSize(0.8f, 0.6f);
}

void Grass::Draw() {
    GameObject::Draw();  // コンポーネント描画
}

void Grass::DrawShadow() {
    BillboardRenderer* bb = nullptr;
    for (Component* c : m_Components) {
        bb = dynamic_cast<BillboardRenderer*>(c);
        if (bb) break;
    }
    if (!bb) return;

    // カメラ向きのワールド行列
    Camera* camera = Manager::GetGameObject<Camera>();
    if (!camera) return;

    Vector3 camPos = camera->GetPosition();
    Vector3 objPos = GetPosition();

    float dx = objPos.x - camPos.x;
    float dz = objPos.z - camPos.z;
    float yaw = atan2f(dx, dz);

    float w = bb->GetWidth();
    float h = bb->GetHeight();

    XMMATRIX scale = XMMatrixScaling(w, h, 1.0f);
    XMMATRIX rot = XMMatrixRotationY(yaw);
    XMMATRIX trans = XMMatrixTranslation(objPos.x, objPos.y + h * 0.5f, objPos.z);
    XMMATRIX world = scale * rot * trans;

    Renderer::SetWorldMatrix(world);

    // ビルボード用シャドウシェーダーをバインド
    Renderer::GetDeviceContext()->IASetInputLayout(m_BillboardShadowLayout);
    Renderer::GetDeviceContext()->VSSetShader(m_BillboardShadowVS, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_BillboardShadowPS, NULL, 0);

    // テクスチャ（アルファクリップ用）
    ID3D11ShaderResourceView* tex = bb->GetTexture();
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

    bb->DrawGeometry();
}