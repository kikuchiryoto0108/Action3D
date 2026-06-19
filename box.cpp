//==============================================================================
//  File   : box.cpp
//==============================================================================
#include "main.h"
#include "box.h"
#include "renderer.h"
#include "modelRenderer.h"
#include "boxCollider.h"

void Box::Init() {
    ModelRenderer* modelRenderer = AddComponent<ModelRenderer>();
    modelRenderer->Load("asset\\model\\box.obj");

    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
        "shader\\lightingVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader,
        "shader\\lightingPS.cso");

    BoxCollider* collider = AddComponent<BoxCollider>();
    collider->SetSize({ m_Scale.x * 2.0f, m_Scale.y * 2.0f, m_Scale.z * 2.0f });
    collider->SetOffset({ 0, m_Scale.y, 0 });  // 底面原点なので半分=Yスケール分上
    collider->SetTag(CollisionTag::Wall);
}

void Box::Uninit() {
    if (m_VertexLayout) m_VertexLayout->Release();
    if (m_VertexShader) m_VertexShader->Release();
    if (m_PixelShader)  m_PixelShader->Release();

    GameObject::Uninit();
}

void Box::Draw() {
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    Renderer::SetWorldMatrix(GetWorldMatrix());

    GameObject::Draw();
}
