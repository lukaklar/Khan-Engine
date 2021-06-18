#include "system/precomp.h"
#include "system/timer.hpp"

namespace Khan
{
    Timer::Timer()
        : m_StartTime(0)
        , m_ElapsedTime(0)
        , m_PreviousTime(0)
        , m_Stopped(true)
    {
        int64_t countsPerSec;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
        m_SecPerCount = 1.0 / static_cast<double>(countsPerSec);
    }

    double Timer::GetElapsedTimeInMs()
    {
        if (m_Stopped)
        {
            return 0.0;
        }
        else
        {
            int64_t currentTime;
            QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));
            m_ElapsedTime += currentTime - m_StartTime;
            return static_cast<double>(m_ElapsedTime) * m_SecPerCount * 1000.0;
        }
    }

    float Timer::GetElapsedTime()
    {
        if (m_Stopped)
        {
            return 0.0f;
        }
        else
        {
            int64_t currentTime;
            QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));
            m_ElapsedTime += currentTime - m_StartTime;
            return static_cast<float>(m_ElapsedTime * m_SecPerCount);
        }
    }

    double Timer::GetDeltaTimeInMs()
    {
        if (m_Stopped)
        {
            return 0.0;
        }
        else
        {
            int64_t currentTime;
            QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));
            int64_t deltaTime = currentTime - m_PreviousTime;
            m_PreviousTime = currentTime;
            return static_cast<double>(deltaTime) * m_SecPerCount * 1000.0;

        }
    }

    float Timer::GetDeltaTime()
    {
        if (m_Stopped)
        {
            return 0.0f;
        }
        else
        {
            int64_t currentTime;
            QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));
            int64_t deltaTime = currentTime - m_PreviousTime;
            m_PreviousTime = currentTime;
            return static_cast<float>(deltaTime * m_SecPerCount);

        }
    }

    void Timer::Start()
    {
        m_Stopped = false;
        m_ElapsedTime = 0;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_StartTime));
    }

    inline void Timer::Stop()
    {
        m_Stopped = true;
    }

    inline void Timer::Pause()
    {
        int64_t currentTime;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));
        m_ElapsedTime += currentTime - m_StartTime;
        m_Stopped = true;
    }

    void Timer::Resume()
    {
        m_Stopped = false;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_StartTime));
    }

    void Timer::Reset()
    {
        m_ElapsedTime = 0;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_StartTime));
    }
}