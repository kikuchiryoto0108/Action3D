//==============================================================================
//  File   : renderer.h
//  Brief  : 
// 
//  Author : Ryoto Kikuchi
//  Date   : 2026/4/21
//------------------------------------------------------------------------------
//
//==============================================================================
#pragma once

struct VERTEX_3D
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Diffuse;
	XMFLOAT2 TexCoord;
};

struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	BOOL		TextureEnable;
	float		Dummy[2];
};

struct LIGHT
{
	BOOL		Enable;
	BOOL		Dummy[3];
	XMFLOAT4	Direction;
	XMFLOAT4	Diffuse;
	XMFLOAT4 Ambient;     // 地面側
	XMFLOAT4 AmbientSky;  // 空側
	XMFLOAT4 Specular;
};

class Renderer
{
private:

	static D3D_FEATURE_LEVEL       m_FeatureLevel;

	static ID3D11Device*           m_Device;
	static ID3D11DeviceContext*    m_DeviceContext;
	static IDXGISwapChain*         m_SwapChain;
	static ID3D11RenderTargetView* m_RenderTargetView;
	static ID3D11DepthStencilView* m_DepthStencilView;

	static ID3D11Buffer*			m_WorldBuffer;
	static ID3D11Buffer*			m_ViewBuffer;
	static ID3D11Buffer*			m_ProjectionBuffer;
	static ID3D11Buffer*			m_MaterialBuffer;
	static ID3D11Buffer*			m_LightBuffer;


	static ID3D11DepthStencilState* m_DepthStateEnable;
	static ID3D11DepthStencilState* m_DepthStateDisable;

	static ID3D11BlendState*		m_BlendState;
	static ID3D11BlendState*		m_BlendStateATC;

	static ID3D11Buffer* m_CameraBuffer;

	// シャドウマップ用
	static const UINT SHADOW_MAP_SIZE = 2048;

	static ID3D11Texture2D* m_ShadowMapTexture;
	static ID3D11DepthStencilView* m_ShadowMapDSV;
	static ID3D11ShaderResourceView* m_ShadowMapSRV;
	static ID3D11Buffer* m_ShadowBuffer;
	static D3D11_VIEWPORT m_DefaultViewport;

	static ID3D11SamplerState* m_ShadowSampler;

	static ID3D11Buffer* m_SpriteBuffer;
	static XMMATRIX m_ViewMatrixCache;

public:
	static void Init();
	static void Uninit();
	static void Begin();
	static void End();

	static void SetDepthEnable(bool Enable);
	static void SetATCEnable(bool Enable);
	static void SetWorldViewProjection2D();
	static void SetWorldMatrix(XMMATRIX WorldMatrix);
	static void SetViewMatrix(XMMATRIX ViewMatrix);
	static void SetProjectionMatrix(XMMATRIX ProjectionMatrix);
	static void SetMaterial(MATERIAL Material);
	static void SetLight(LIGHT Light);

	static ID3D11Device* GetDevice( void ){ return m_Device; }
	static ID3D11DeviceContext* GetDeviceContext( void ){ return m_DeviceContext; }

	static XMMATRIX GetViewMatrix() { return m_ViewMatrixCache; }

	static void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	static void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);

	static void SetCameraPosition(const Vector3& pos);

	static void SetSpriteUV(float offsetX, float offsetY, float scaleX, float scaleY);

	static void SetBlendAdditive();
	static void SetBlendAlpha();

	// シャドウマップ
	static void BeginShadowPass();
	static void EndShadowPass();
	static void SetLightViewProjection(XMMATRIX view, XMMATRIX proj);
	static ID3D11ShaderResourceView* GetShadowMapSRV() { return m_ShadowMapSRV; }
};
