/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        shared/CElement.cpp
*  PURPOSE:     Base class for all elements (level objects)
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CElement.h"
#include "CLevel.h"
#include "CElementsMgr.h"

using namespace std;

CElement::CElement(EElementType Type, CLevel *pLevel, unsigned nUid):
    m_Type(Type), m_nUid(nUid), m_pLevel(pLevel)
{
    m_pLevel->GetElementsMgr()->Add(this);
}
        
CElement::~CElement()
{
    m_pLevel->GetElementsMgr()->Remove(this);
}
