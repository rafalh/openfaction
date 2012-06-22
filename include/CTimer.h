/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/CTimer.h
*  PURPOSE:     Cross-platform milisecond timer
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CTIMER_H
#define CTIMER_H

#if !LINUX
# include <windows.h>
#else
#include <ctime>
#include <unistd.h>
#endif // !LINUX

/* need to add an intialisation routine that checks resolution via clock_getres() */

class CTimer
{
    public:
        inline CTimer()
        {
            Reset();
        }
        
        inline static void Sleep(unsigned nMs)
        {
#if !LINUX
            ::Sleep(nMs);
#else
            usleep(nMs);
#endif
        }
        
        inline void Reset()
        {
#if !LINUX
            m_nStartTicks = GetTickCount();
#else
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            m_nStartTicks = (now.tv_sec * 1000) + (now.tv_nsec / 1000000);
#endif
        }
        
        inline unsigned GetValue() const
        {
#if !LINUX
            return GetTickCount() - m_nStartTicks;
#else
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            return (now.tv_sec * 1000) + (now.tv_nsec / 1000000) - m_nStartTicks;
#endif
        }
        
    private:
        unsigned m_nStartTicks;
};

#endif // CTIMER_H
