#include "main.h"
#include "renderer.h"
#include "manager.h"
#include "camera.h"
#include "gameObject.h"
#include "billboardRenderer.h"

ID3D11Buffer* BillboardRenderer::m_VertexBuffer = nullptr;
ID3D11VertexShader* BillboardRenderer::m_VertexShader = nullptr;
ID3D11PixelShader* BillboardRenderer::m_PixelShader = nullptr;
ID3D11InputLayout* BillboardRenderer::m_VertexLayout = nullptr;
int                 BillboardRenderer::m_RefCount = 0;

void BillboardRenderer::Init() {
    m_RefCount++;
    if (m_RefCount > 1) return;  // 既に初期化済み

    // 共通の板ポリ（中心原点、サイズは描画時にスケーリング）
    VERTEX_3D vertex[4];
    vertex[0].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);  // 左上
    vertex[1].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);  // 右上
    vertex[2].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);  // 左下
    vertex[3].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);  // 右下

    for (int i = 0; i < 4; i++) {
        vertex[i].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
        vertex[i].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
    vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
    vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
    vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = vertex;

    Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

    // シェーダー
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
        "shader\\billboardVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader,
        "shader\\billboardPS.cso");
}

void BillboardRenderer::Uninit() {
    if (m_Texture) {
        m_Texture->Release();
        m_Texture = nullptr;
    }

    m_RefCount--;
    if (m_RefCount > 0) return;

    if (m_VertexBuffer) { m_VertexBuffer->Release(); m_VertexBuffer = nullptr; }
    if (m_VertexShader) { m_VertexShader->Release(); m_VertexShader = nullptr; }
    if (m_PixelShader) { m_PixelShader->Release();  m_PixelShader = nullptr; }
    if (m_VertexLayout) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
}

void BillboardRenderer::LoadTexture(const wchar_t* fileName) {
    TexMetadata metadata;
    ScratchImage image;
    LoadFromWICFile(fileName, WIC_FLAGS_NONE, &metadata, image);

    ScratchImage converted;
    Convert(image.GetImages(), image.GetImageCount(), image.GetMetadata(),
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        TEX_FILTER_DEFAULT | TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT,
        TEX_THRESHOLD_DEFAULT, converted);

    CreateShaderResourceView(Renderer::GetDevice(),
        converted.GetImages(), converted.GetImageCount(),
        converted.GetMetadata(), &m_Texture);
}

void BillboardRenderer::Draw() {
    // カメラ位置を取得
    Camera* camera = Manager::GetGameObject<Camera>();
    if (!camera) return;

    Vector3 camPos = camera->GetPosition();
    Vector3 objPos = m_GameObject->GetPosition();

    // カメラからオブジェクトへのベクトル
    float dx = objPos.x - camPos.x;
    float dz = objPos.z - camPos.z;
    float yaw = atan2f(dx, dz);

    // ワールド行列
    XMMATRIX scale = XMMatrixScaling(m_Width, m_Height, 1.0f);
    XMMATRIX rot;
    if (m_YLock) {
        // 木・草用: Y軸のみ回転
        float dx = objPos.x - camPos.x;
        float dz = objPos.z - camPos.z;
        float yaw = atan2f(dx, dz);
        rot = XMMatrixRotationY(yaw);
    } else {
        // 爆発用: 完全にカメラに正対
        float dx = objPos.x - camPos.x;
        float dy = objPos.y - camPos.y;
        float dz = objPos.z - camPos.z;
        float yaw = atan2f(dx, dz);
        float dist = sqrtf(dx * dx + dz * dz);
        float pitch = -atan2f(dy, dist);
        rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0);
    }
    float yOffset = m_YLock ? m_Height * 0.5f : 0.0f;
    XMMATRIX trans = XMMatrixTranslation(objPos.x, objPos.y + yOffset, objPos.z);
    XMMATRIX world = scale * rot * trans;

    Renderer::SetWorldMatrix(world);

    // UV パラメータを設定
    Renderer::SetSpriteUV(m_UVOffsetX, m_UVOffsetY, m_UVScaleX, m_UVScaleY);

    // 描画パイプライン設定
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    // テクスチャ
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

    // マテリアル
    MATERIAL material{};
    material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    material.TextureEnable = true;
    Renderer::SetMaterial(material);

    // 頂点バッファ
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // アルファテスト（葉っぱの透明部分を抜く）
    Renderer::SetATCEnable(true);
	DrawGeometry();
    Renderer::SetATCEnable(false);
}

void BillboardRenderer::DrawGeometry() {
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Renderer::GetDeviceContext()->Draw(4, 0);
}
