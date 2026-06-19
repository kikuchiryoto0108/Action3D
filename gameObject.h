#pragma once
#include "component.h"
#include "renderer.h"

class GameObject {
public:
	enum class Layer { Default3D, UI2D };

protected:
	Vector3 m_Position{ 0.0f, 0.0f, 0.0f };
	Vector3 m_Rotation{ 0.0f, 0.0f, 0.0f };
	Vector3 m_Scale{ 1.0f, 1.0f, 1.0f };

	std::list<Component*> m_Components; // コンポーネントのリスト

	// シャドウ用シェーダー（全オブジェクト共通）
	static ID3D11VertexShader* m_ShadowVS;
	static ID3D11PixelShader* m_ShadowPS;
	static ID3D11InputLayout* m_ShadowLayout;

	static ID3D11VertexShader* m_BillboardShadowVS;
	static ID3D11PixelShader* m_BillboardShadowPS;
	static ID3D11InputLayout* m_BillboardShadowLayout;

	bool m_Destroy = false;

public:
	void SetPosition(const Vector3& position) { m_Position = position; }
	Vector3 GetPosition() const { return m_Position; }

	static void InitShadowShader();
	static void UninitShadowShader();

	virtual void Init() {}
	virtual void Uninit() {
		for (Component* component : m_Components) {
			component->Uninit();
			delete component;
		}
	}
	virtual void Update() {
		for (Component* component : m_Components) {
			component->Update();
		}
	}
	virtual void Draw() {
		for (Component* component : m_Components) {
			component->Draw();
		}
	}

	// どのレイヤーで描画するか（派生クラスで override 可能）
	virtual Layer GetLayer() const { return Layer::Default3D; }

	// ワールド行列を計算
	virtual XMMATRIX GetWorldMatrix() const {
		XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		XMMATRIX rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		XMMATRIX trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		return scale * rot * trans;
	}


	// デフォルトのシャドウ描画（ワールド行列をセットしてコンポーネント描画）
	virtual void DrawShadow() {
		// シャドウ用シェーダーをバインド
		Renderer::GetDeviceContext()->IASetInputLayout(m_ShadowLayout);
		Renderer::GetDeviceContext()->VSSetShader(m_ShadowVS, NULL, 0);
		Renderer::GetDeviceContext()->PSSetShader(m_ShadowPS, NULL, 0);

		// ワールド行列
		Renderer::SetWorldMatrix(GetWorldMatrix());

		// コンポーネント（ModelRenderer など）の Draw を呼ぶ
		for (Component* component : m_Components) {
			component->Draw();
		}
	}

	virtual void OnCollision(GameObject* other) {}

	bool IsDestroyed() const { return m_Destroy; }
	void Destroy() { m_Destroy = true; }

	template<typename T>
	T* AddComponent() {
		T* component = new T(this);
		component->Init();
		m_Components.push_back(component);

		return component;
	}

	template<typename T>
	T* GetComponent() {
		for (Component* c : m_Components) {
			T* casted = dynamic_cast<T*>(c);
			if (casted) return casted;
		}
		return nullptr;
	}
};
