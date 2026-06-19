#pragma once

class Time {
private:
    static float m_DeltaTime;
    static float m_TimeScale;
    static float m_TotalTime;
    static LARGE_INTEGER m_Frequency;
    static LARGE_INTEGER m_LastTime;

public:
    static void Init();
    static void Update();

    // 1フレームの経過時間（秒）。TimeScale適用済み。
    static float GetDeltaTime() { return m_DeltaTime * m_TimeScale; }

    // TimeScale を無視した実時間の経過時間（UIアニメなどに使う）
    static float GetUnscaledDeltaTime() { return m_DeltaTime; }

    // ゲーム開始からの累計時間（秒、TimeScale適用済み）
    static float GetTotalTime() { return m_TotalTime; }

    // タイムスケール（1.0=通常、0.0=停止、0.5=半速）
    static void  SetTimeScale(float s) { m_TimeScale = s; }
    static float GetTimeScale() { return m_TimeScale; }
};
