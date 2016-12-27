/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CHandleManager.h
*  PURPOSE:     Allocates handles for objects
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CHANDLEMANAGER_H
#define CHANDLEMANAGER_H

class CHandleManager
{
    public:
        inline CHandleManager():
            m_LastHandle(0) {}
        
        inline unsigned AllocHandle()
        {
            ++m_LastHandle;
            if(m_LastHandle == 0xFFFF)
                m_LastHandle = 0;
            return m_LastHandle;
        }
        
        static inline CHandleManager &GetInst()
        {
            static CHandleManager HandleMgr;
            return HandleMgr;
        }
        
    private:
        unsigned m_LastHandle;
};

#endif // CHANDLEMANAGER_H
