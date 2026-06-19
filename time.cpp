#include "main.h"
#include "time.h"

float Time::m_DeltaTime = 0.0f;
float Time::m_TimeScale = 1.0f;
float Time::m_TotalTime = 0.0f;
LARGE_INTEGER Time::m_Frequency = {};
LARGE_INTEGER Time::m_LastTime = {};

void Time::Init() {
    QueryPerformanceFrequency(&m_Frequency);
    QueryPerformanceCounter(&m_LastTime);
    m_DeltaTime = 0.0f;
    m_TotalTime = 0.0f;
}

void Time::Update() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    LONGLONG elapsed = now.QuadPart - m_LastTime.QuadPart;
    m_DeltaTime = (float)elapsed / (float)m_Frequency.QuadPart;
    m_LastTime = now;

    // 異常値防止（ブレークポイントで止まったあとの巨大DeltaTime対策）
    if (m_DeltaTime > 0.1f) m_DeltaTime = 0.1f;

    m_TotalTime += m_DeltaTime * m_TimeScale;
}
