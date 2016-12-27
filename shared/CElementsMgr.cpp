/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CElementsMgr.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include <cassert>
#include "CElementsMgr.h"
#include "util/CLogger.h"

using namespace std;

void CElementsMgr::Add(CElement *pElement)
{
    if(pElement->GetUid() != OFE_INVALID_UID)
    {
        pair<TElementsMap::iterator, bool> Ret = m_ElementsMap.insert(pair<unsigned, CElement*>(pElement->GetUid(), pElement));
        if(!Ret.second)
            CLogger::GetInst().PrintStatus("UID %u is not unique!\n", pElement->GetUid());
    }
    
    assert(pElement->GetType() < OFET_MAX);
    m_Elements[pElement->GetType()].insert(pElement);
}

void CElementsMgr::Remove(CElement *pElement)
{
    if(pElement->GetUid() != OFE_INVALID_UID)
        m_ElementsMap.erase(pElement->GetUid());
    
    assert(pElement->GetType() < OFET_MAX);
    m_Elements[pElement->GetType()].erase(pElement);
}
