#include "main.h"
#include "sky.h"
#include "renderer.h"
#include "camera.h"
#include "manager.h"
#include <vector>

void Sky::Init() {
    //---------------- 球メッシュ生成 ----------------
    const int latSegments = 20;   // 緯度方向の分割数
    const int lonSegments = 30;   // 経度方向の分割数
    const float radius = 500.0f;  // 十分大きく

    std::vector<VERTEX_3D> vertices;
    std::vector<unsigned short> indices;

    for (int lat = 0; lat <= latSegments; lat++) {
        float theta = (float)lat / latSegments * XM_PI; // 0 ~ PI
        for (int lon = 0; lon <= lonSegments; lon++) {
            float phi = (float)lon / lonSegments * XM_2PI; // 0 ~ 2PI
            VERTEX_3D v{};
            v.Position.x = radius * sinf(theta) * cosf(phi);
            v.Position.y = radius * cosf(theta);
            v.Position.z = radius * sinf(theta) * sinf(phi);
            // 法線は内側向き（使わないけど）
            v.Normal = XMFLOAT3(-v.Position.x, -v.Position.y, -v.Position.z);
            v.Diffuse = XMFLOAT4(1, 1, 1, 1);
            v.TexCoord = XMFLOAT2((float)lon / lonSegments, (float)lat / latSegments);
            vertices.push_back(v);
        }
    }

    // インデックス（内側を向くように三角形の順序を逆に）
    for (int lat = 0; lat < latSegments; lat++) {
        for (int lon = 0; lon < lonSegments; lon++) {
            unsigned short i0 = lat * (lonSegments + 1) + lon;
            unsigned short i1 = i0 + 1;
            unsigned short i2 = i0 + (lonSegments + 1);
            unsigned short i3 = i2 + 1;
            // 内側向き（CCWを反転）
            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
        }
    }
    m_IndexCount = (UINT)indices.size();

    // 頂点バッファ
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(VERTEX_3D) * (UINT)vertices.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = vertices.data();
    Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

    // インデックスバッファ
    bd.ByteWidth = sizeof(unsigned short) * m_IndexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    sd.pSysMem = indices.data();
    Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);

    //---------------- シェーダー ----------------
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
        "shader\\skyVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader,
        "shader\\skyPS.cso");

    //---------------- HDR テクスチャ読み込み ----------------
    DirectX::ScratchImage image;
    DirectX::TexMetadata metadata;
    HRESULT hr = DirectX::LoadFromHDRFile(
        L"asset\\texture\\sky.hdr", &metadata, image);
    assert(SUCCEEDED(hr));

    DirectX::CreateShaderResourceView(Renderer::GetDevice(),
        image.GetImages(), image.GetImageCount(), metadata, &m_Texture);
}

void Sky::Uninit() {
    if (m_VertexBuffer) m_VertexBuffer->Release();
    if (m_IndexBuffer) m_IndexBuffer->Release();
    if (m_VertexShader) m_VertexShader->Release();
    if (m_PixelShader) m_PixelShader->Release();
    if (m_VertexLayout) m_VertexLayout->Release();
    if (m_Texture) m_Texture->Release();
    GameObject::Uninit();
}

void Sky::Draw() {
    Camera* camera = Manager::GetGameObject<Camera>();
    if (!camera) return;
    Vector3 camPos = camera->GetPosition();

    // ★ カメラ位置に追従
    XMMATRIX world = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
    Renderer::SetWorldMatrix(world);

    // 深度書き込みOFF
    Renderer::SetDepthEnable(false);

    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, nullptr, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, nullptr, 0);
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

    MATERIAL mat{};
    mat.Diffuse = XMFLOAT4(1, 1, 1, 1);
    mat.TextureEnable = true;
    Renderer::SetMaterial(mat);

    UINT stride = sizeof(VERTEX_3D), offset = 0;
    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Renderer::GetDeviceContext()->DrawIndexed(m_IndexCount, 0, 0);

    Renderer::SetDepthEnable(true);
}
