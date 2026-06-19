//==============================================================================
//  File   : manager.cpp
//  Brief  : 
// 
//  Author : Ryoto Kikuchi
//  Date   : 2026/4/21
//------------------------------------------------------------------------------
//
//==============================================================================
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "polygon2D.h"
#include "field.h"
#include "camera.h"
#include "player.h"
#include "enemy.h"
#include "mouse.h"
#include "tree.h"
#include "grass.h"
#include "collider.h"
#include "sphereCollider.h"
#include "boxCollider.h"
#include <time.h>
#include "sky.h"
#include "box.h"

std::list<GameObject*> Manager::m_GameObjects;	// staticメンバ変数はCPPファイル内で定義する必要がある
std::list<GameObject*> Manager::m_DestroyList;

//==============================================================================
// 衝突判定用ヘルパー（無名namespace）
//==============================================================================
namespace {
    bool SphereVsSphere(SphereCollider* a, SphereCollider* b) {
        Vector3 pa = a->GetWorldPosition();
        Vector3 pb = b->GetWorldPosition();
        float dx = pa.x - pb.x, dy = pa.y - pb.y, dz = pa.z - pb.z;
        float distSq = dx * dx + dy * dy + dz * dz;
        float r = a->GetRadius() + b->GetRadius();
        return distSq < r * r;
    }

    bool BoxVsBox(BoxCollider* a, BoxCollider* b) {
        Vector3 amin = a->GetMin(), amax = a->GetMax();
        Vector3 bmin = b->GetMin(), bmax = b->GetMax();
        return amin.x <= bmax.x && amax.x >= bmin.x
            && amin.y <= bmax.y && amax.y >= bmin.y
            && amin.z <= bmax.z && amax.z >= bmin.z;
    }

    bool SphereVsBox(SphereCollider* s, BoxCollider* b) {
        Vector3 sc = s->GetWorldPosition();
        Vector3 bmin = b->GetMin(), bmax = b->GetMax();
        float cx = std::max(bmin.x, std::min(sc.x, bmax.x));
        float cy = std::max(bmin.y, std::min(sc.y, bmax.y));
        float cz = std::max(bmin.z, std::min(sc.z, bmax.z));
        float dx = sc.x - cx, dy = sc.y - cy, dz = sc.z - cz;
        float r = s->GetRadius();
        return (dx * dx + dy * dy + dz * dz) < r * r;
    }

    bool Intersect(Collider* a, Collider* b) {
        ColliderType ta = a->GetType();
        ColliderType tb = b->GetType();
        if (ta == ColliderType::Sphere && tb == ColliderType::Sphere)
            return SphereVsSphere((SphereCollider*)a, (SphereCollider*)b);
        if (ta == ColliderType::Box && tb == ColliderType::Box)
            return BoxVsBox((BoxCollider*)a, (BoxCollider*)b);
        if (ta == ColliderType::Sphere && tb == ColliderType::Box)
            return SphereVsBox((SphereCollider*)a, (BoxCollider*)b);
        if (ta == ColliderType::Box && tb == ColliderType::Sphere)
            return SphereVsBox((SphereCollider*)b, (BoxCollider*)a);
        return false;
    }
}

static void DrawDebugGui();

void Manager::Init() {
    srand((unsigned)time(nullptr));

	Renderer::Init();

	GameObject* gameObject;
    GameObject::InitShadowShader();

    AddGameObject<Sky>();
	AddGameObject<Camera>();
	AddGameObject<Field>();
	AddGameObject<Player>();
	AddGameObject<Enemy>();

    for (int i = 0; i < 10; i++) {
        Tree* tree = new Tree();
        float x = (rand() % 40 - 20);
        float z = (rand() % 40 - 20);
        tree->SetPosition({ x, 0.0f, z });
        Manager::AddGameObject(tree);
    }

    for (int i = 0; i < 30; i++) {
        Grass* grass = new Grass();
        float x = (rand() % 40 - 20);
        float z = (rand() % 40 - 20);
        grass->SetPosition({ x, 0.0f, z });
        Manager::AddGameObject(grass);
    }

    Box* box1 = new Box();
    box1->SetPosition({ 5.0f, 0.0f, 5.0f });
    box1->SetSize({ 3.0f, 1.0f, 3.0f });
    Manager::AddGameObject(box1);

    Box* box2 = new Box();
    box2->SetPosition({ -5.0f, 0.0f, 8.0f });
    box2->SetSize({ 4.0f, 2.0f, 2.0f });
    Manager::AddGameObject(box2);

    Box* box3 = new Box();
    box3->SetPosition({ 0.0f, 0.0f, -10.0f });
    box3->SetSize({ 6.0f, 1.0f, 1.0f });
    Manager::AddGameObject(box3);

	AddGameObject<Polygon2D>();
}


void Manager::Uninit()
{
	for (GameObject* gameObject : m_GameObjects) {
		gameObject->Uninit();
		delete gameObject;
	}

    GameObject::UninitShadowShader();

	Renderer::Uninit();
}

void Manager::Update() {
	Mouse::Instance().Update();


	for (GameObject* gameObject : m_GameObjects) {
		gameObject->Update();
	}

    CheckCollisions();

    // 削除処理
    for (GameObject* obj : m_DestroyList) {
        obj->Uninit();
        m_GameObjects.remove(obj);
        delete obj;
    }
    m_DestroyList.clear();
}

//==============================================================================
// CheckCollisions
//==============================================================================
void Manager::CheckCollisions() {
    // すべてのコライダーを収集
    std::vector<std::pair<GameObject*, Collider*>> colliders;
    for (GameObject* obj : m_GameObjects) {
        if (obj->IsDestroyed()) continue;
        Collider* col = obj->GetComponent<Collider>();
        if (col) colliders.push_back({ obj, col });
    }

    // 総当たり判定
    for (size_t i = 0; i < colliders.size(); i++) {
        for (size_t j = i + 1; j < colliders.size(); j++) {
            GameObject* objA = colliders[i].first;
            GameObject* objB = colliders[j].first;
            Collider* colA = colliders[i].second;
            Collider* colB = colliders[j].second;

            if (objA->IsDestroyed() || objB->IsDestroyed()) continue;

            if (Intersect(colA, colB)) {
                objA->OnCollision(objB);
                objB->OnCollision(objA);
            }
        }
    }
}

void Manager::Draw()
{
    //--------------------------------------------------------------------------
    // パス1: シャドウマップ生成
    //--------------------------------------------------------------------------
    Renderer::BeginShadowPass();

    // 太陽光の向き（renderer.cpp の Init で設定したのと同じ向き）
    XMVECTOR lightDir = XMVector3Normalize(XMVectorSet(0.5f, -1.0f, 0.5f, 0.0f));
    XMVECTOR lightPos = XMVectorScale(lightDir, -30.0f);
    XMVECTOR lightTarget = XMVectorZero();
    XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX lightView = XMMatrixLookAtLH(lightPos, lightTarget, lightUp);
    XMMATRIX lightProj = XMMatrixOrthographicLH(40.0f, 40.0f, 0.1f, 100.0f);

    Renderer::SetLightViewProjection(lightView, lightProj);

    for (GameObject* gameObject : m_GameObjects) {
        gameObject->DrawShadow();
    }

    Renderer::EndShadowPass();

    //--------------------------------------------------------------------------
    // パス2: 通常描画
    //--------------------------------------------------------------------------
    Renderer::Begin();

    // Sky を最初に描画
    if (Sky* sky = Manager::GetGameObject<Sky>()) {
        sky->Draw();
    }


    // ---- ImGui フレーム開始 ----
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 3D描画
    for (GameObject* obj : m_GameObjects) {
        if (obj->GetLayer() == GameObject::Layer::Default3D) {
            obj->Draw();
        }
    }

    // 2D描画（UIなど、必ず最後に描画）
    for (GameObject* obj : m_GameObjects) {
        if (obj->GetLayer() == GameObject::Layer::UI2D) {
            obj->Draw();
        }
    }

    // ---- ImGui のデバッグウィンドウを描画 ----
    DrawDebugGui();

    // ---- ImGui レンダリング ----
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    Renderer::End();
}

void Manager::AddGameObject(GameObject* gameObject) {
    gameObject->Init();
    m_GameObjects.push_back(gameObject);
}

void Manager::DestroyGameObject(GameObject* gameObject) {
    m_DestroyList.push_back(gameObject);
}

static void DrawDebugGui() {
    // FPS表示
    ImGui::Begin("Debug");

    ImGui::Text("FPS: %.1f", 1.0f / Time::GetUnscaledDeltaTime());
    ImGui::Text("DeltaTime: %.4f ms", Time::GetUnscaledDeltaTime() * 1000.0f);

    // タイムスケール
    float ts = Time::GetTimeScale();
    if (ImGui::SliderFloat("TimeScale", &ts, 0.0f, 2.0f)) {
        Time::SetTimeScale(ts);
    }

    ImGui::Separator();

    // プレイヤー情報
    if (Player* player = Manager::GetGameObject<Player>()) {
        Vector3 pos = player->GetPosition();
        ImGui::Text("Player Pos: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
    }

    // オブジェクト数
    ImGui::Text("GameObjects: %zu", Manager::GetGameObjects<GameObject>().size());

    ImGui::End();
}

