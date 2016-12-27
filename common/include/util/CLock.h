/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/CLock.h
*  PURPOSE:     Cross-platform critical section
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CLOCK_H
#define CLOCK_H

#if !LINUX

#include <windows.h>

class CLock
{
    public:
        inline CLock()
        {
            InitializeCriticalSection(&m_Cs);
        }
        
        inline ~CLock()
        {
            DeleteCriticalSection(&m_Cs);
        }
        
        inline void Acquire()
        {
            EnterCriticalSection(&m_Cs);
        }
        
        inline void Release()
        {
            LeaveCriticalSection(&m_Cs);
        }
    
    private:
        CRITICAL_SECTION m_Cs;
};

#else /* LINUX */

#include <pthread.h>

class CLock
{
    public:
        inline CLock()
        {
            pthread_mutex_init(&m_Mutex, NULL);
        }
        
        inline ~CLock()
        {
            pthread_mutex_destroy(&m_Mutex);
        }
        
        inline void Acquire()
        {
            pthread_mutex_lock(&m_Mutex);
        }
        
        inline void Release()
        {
            pthread_mutex_unlock(&m_Mutex);
        }
    
    private:
        pthread_mutex_t m_Mutex;
};

#endif

#endif // CLOCK_H
