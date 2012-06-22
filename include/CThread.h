/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/CThread.h
*  PURPOSE:     Cross-platform threading
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CTHREAD_H
#define CTHREAD_H

#if !LINUX

#define DECLARE_THREAD_FUNC(func_name, param_name) DWORD WINAPI func_name (void * param_name) // 

class CThread
{
    public:
        inline CThread(): hThread(NULL) {}
        
        inline ~CThread()
        {
            if(hThread)
                Terminate();
        }
        
        inline int Start(DWORD (WINAPI *pFunc)(void*), void *pParam = 0)
        {
            if(hThread)
                Terminate();
            
            hThread = CreateThread(NULL, 0, pFunc, pParam, 0, NULL);
            
            return hThread ? 0 : -1;
        }
        
        inline void Terminate()
        {
            TerminateThread(hThread, -1);
            CloseHandle(hThread);
            hThread = NULL;
        }
        
        inline bool IsRunning()
        {
            return hThread ? true : false; /* FIXME */
        }
    
    private:
        HANDLE hThread;
};

#else /* LINUX */

#define DECLARE_THREAD_FUNC(func_name, param_name) void * func_name (void * param_name)

class CThread
{
    public:
        inline CThread(): bValid(false) {}

        inline ~CThread()
        {
            if(bValid)
                Terminate();
        }
        inline int Start(void *(*pFunc)(void*), void *pParam = 0)
        {
            if(bValid)
                Terminate();
            
            bValid = (pthread_create(&Thread, NULL, pFunc, pParam) == 0);
            return bValid ? 0 : -1;
        }
        inline void Terminate()
        {
            pthread_cancel(Thread);
            pthread_detach(Thread);
            bValid = false;
        }

        inline bool IsRunning()
        {
            return bValid;
        }
    
    private:
        bool      bValid;
        pthread_t Thread;
};

#endif

#endif // CTHREAD_H
