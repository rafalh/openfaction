/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CElementsMgr.h
*  PURPOSE:     Keeps list of all elements
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CELEMENTSMGR_H
#define CELEMENTSMGR_H

#include <set>
#include <map>
#include "CElement.h"

class CElementsMgr
{
    public:
        void Add(CElement *pElement);
        void Remove(CElement *pElement);
        
        inline CElement *Get(unsigned nUid) const
        {
            TElementsMap::const_iterator it = m_ElementsMap.find(nUid);
            return it != m_ElementsMap.end() ? it->second : NULL;
        }
        
        inline const std::set<CElement*> &GetByType(EElementType Type) const
        {
            return m_Elements[Type];
        }
        
        inline void Clear()
        {
            m_ElementsMap.clear();
            for(unsigned i = 0; i < OFET_MAX; ++i)
                m_Elements[i].clear();
        }
        
    private:
        typedef std::map<unsigned, CElement*> TElementsMap;
        
        TElementsMap m_ElementsMap;
        std::set<CElement*> m_Elements[OFET_MAX];
};

#endif // CELEMENTSMGR_H
