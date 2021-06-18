#pragma once

namespace Khan
{
    class Timer
    {
    public:
        Timer();

        double GetElapsedTimeInMs();
        float GetElapsedTime();
        double GetDeltaTimeInMs();
        float GetDeltaTime();

        void Start();
        void Stop();
        void Pause();
        void Resume();
        void Reset();

    private:
        double m_SecPerCount;
        int64_t m_StartTime;
        int64_t m_ElapsedTime;
        int64_t m_PreviousTime;
        bool m_Stopped;
    };
}