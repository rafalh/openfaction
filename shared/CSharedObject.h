/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CSharedObject.h
*  PURPOSE:     Base class for reference-counted objects
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CSHAREDOBJECT_H
#define CSHAREDOBJECT_H

class CSharedObject
{
    public:
        CSharedObject():
            m_cRefs(1) {}
        virtual ~CSharedObject() {}
        
        unsigned AddRef()
        {
            ++m_cRefs;
        }
        
        unsigned Release()
        {
            --m_cRefs;
            if(m_cRefs == 0)
            {
                delete this;
                return 0;
            }
            return m_cRefs;
        }
        
    protected:
        unsigned m_cRefs;
};

#endif // CSHAREDOBJECT_H
