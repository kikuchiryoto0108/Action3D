#include "main.h"
#include "renderer.h"
#include "polygon2D.h"


void Polygon2D::Init() {

    VERTEX_3D vertex[4];

    vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

    vertex[1].Position = XMFLOAT3(200.0f, 0.0f, 0.0f);
    vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

    vertex[2].Position = XMFLOAT3(0.0f, 200.0f, 0.0f);
    vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

    vertex[3].Position = XMFLOAT3(200.0f, 200.0f, 0.0f);
    vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);


    // 頂点バッファ生成
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA sd;
    sd.pSysMem = vertex;

    Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);


    // シェーダー生成
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
        "shader\\unlitTextureVS.cso");

    Renderer::CreatePixelShader(&m_PixelShader,
        "shader\\unlitTexturePS.cso");

    // テクスチャ読み込み
    TexMetadata metadata;
    ScratchImage image;

    LoadFromWICFile(L"asset\\texture\\gati_kawaii.png", WIC_FLAGS_NONE, &metadata, image);

    // R8G8B8A8_UNORM_SRGB に強制変換
    ScratchImage converted;
    Convert(
        image.GetImages(), image.GetImageCount(), image.GetMetadata(),
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        TEX_FILTER_DEFAULT | TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT,
        TEX_THRESHOLD_DEFAULT,
        converted);

    CreateShaderResourceView(Renderer::GetDevice(),
        converted.GetImages(), converted.GetImageCount(),
        converted.GetMetadata(),
        &m_Texture);

    assert(m_Texture);
}


void Polygon2D::Uninit() {
    m_Texture->Release();

    m_VertexBuffer->Release();

    m_VertexLayout->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();
}


void Polygon2D::Update() {

}


void Polygon2D::Draw() {
    // 入力レイアウト設定
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    // シェーダー設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);


    // マトリクス設定
    Renderer::SetWorldViewProjection2D();

    // マトリクス設定
    XMMATRIX world, scale, rot, trans;
    scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    rot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
    trans = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    world = scale * rot * trans;

    Renderer::SetWorldMatrix(world);


    // マテリアル設定
    MATERIAL material;
    ZeroMemory(&material, sizeof(material));
    material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
    material.TextureEnable = true;
    Renderer::SetMaterial(material);

    // テクスチャ設定
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

    // 頂点バッファ設定
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // プリミティブトポロジ設定
    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // ポリゴン描画
    Renderer::GetDeviceContext()->Draw(4, 0);

}
