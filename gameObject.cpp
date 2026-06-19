#include "main.h"
#include "renderer.h"
#include "gameObject.h"

// static ƒƒ“ƒo‚ÌŽÀ‘Ì
ID3D11VertexShader* GameObject::m_ShadowVS = nullptr;
ID3D11PixelShader* GameObject::m_ShadowPS = nullptr;
ID3D11InputLayout* GameObject::m_ShadowLayout = nullptr;

ID3D11VertexShader* GameObject::m_BillboardShadowVS = nullptr;
ID3D11PixelShader* GameObject::m_BillboardShadowPS = nullptr;
ID3D11InputLayout* GameObject::m_BillboardShadowLayout = nullptr;

void GameObject::InitShadowShader() {
    Renderer::CreateVertexShader(&m_ShadowVS, &m_ShadowLayout, "shader\\shadowVS.cso");
    Renderer::CreatePixelShader(&m_ShadowPS, "shader\\shadowPS.cso");

    Renderer::CreateVertexShader(&m_BillboardShadowVS, &m_BillboardShadowLayout,
        "shader\\billboardShadowVS.cso");
    Renderer::CreatePixelShader(&m_BillboardShadowPS,
        "shader\\billboardShadowPS.cso");
}

void GameObject::UninitShadowShader() {
    if (m_ShadowLayout) m_ShadowLayout->Release();
    if (m_ShadowVS)     m_ShadowVS->Release();
    if (m_ShadowPS)     m_ShadowPS->Release();

    if (m_BillboardShadowLayout) m_BillboardShadowLayout->Release();
    if (m_BillboardShadowVS)     m_BillboardShadowVS->Release();
    if (m_BillboardShadowPS)     m_BillboardShadowPS->Release();
}
