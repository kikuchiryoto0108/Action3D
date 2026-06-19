//==============================================================================
//  File   : manager.h
//  Brief  : 
// 
//  Author : Ryoto Kikuchi
//  Date   : 2026/4/21
//------------------------------------------------------------------------------
//
//==============================================================================
#pragma once

class GameObject;

class Manager {
private:
	static std::list<GameObject*> m_GameObjects;
	static std::list<GameObject*> m_DestroyList;

	static void CheckCollisions();

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	template<typename T>
	static T* AddGameObject() {
		T* gameObject = new T();
		gameObject->Init();
		m_GameObjects.push_back(gameObject);

		return gameObject;
	}

	template<typename T>
	static T* GetGameObject() {
		for (GameObject* gameObject : m_GameObjects) {
			T* casted = dynamic_cast<T*>(gameObject);	// RTTI（実行時型情報）
			if (casted) {
				return casted;
			}
		}
		return nullptr;
	}

	template<typename T>
	static std::list<T*> GetGameObjects() {
		std::list<T*> result;
		for (GameObject* obj : m_GameObjects) {
			T* casted = dynamic_cast<T*>(obj);
			if (casted) result.push_back(casted);
		}
		return result;
	}

	// 動的追加（既存インスタンスを登録）
	static void AddGameObject(GameObject* gameObject);

	// 削除フラグを立てる（次フレームに削除される）
	static void DestroyGameObject(GameObject* gameObject);
};