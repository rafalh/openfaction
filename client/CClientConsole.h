/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CClientConsole.h
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CCLIENTCONSOLE_H
#define CCLIENTCONSOLE_H

#include <irrlicht.h>
#include <vector>
#include "CString.h"
#include <cstdarg>
#include "CTimer.h"
#include "CConsole.h"

class CClientConsole: public irr::IEventReceiver, public CConsole
{
    public:
        CClientConsole(irr::gui::IGUIEnvironment *pGuiEnv);
        ~CClientConsole();
        void VPrint(const char *pszFormat, std::va_list ap);
        void Render();
        bool OnEvent(const irr::SEvent &Event);
        
        void SetFont(irr::gui::IGUIFont *pFont)
        {
            if(m_pFont)
                m_pFont->drop();
            m_pFont = pFont;
            if(m_pFont)
                m_pFont->grab();
        }
        
        void Toggle()
        {
            if(m_State == CS_VISIBLE || m_State == CS_SHOWING)
                m_State = CS_HIDING;
            else
                m_State = CS_SHOWING;
            m_ToggleTimer.Reset();
        }
        
        void Clear()
        {
            m_Lines.clear();
        }
    
    private:
        static const int LINES_LIMIT;
        static const int TOGGLE_TIME;
        static const int HEIGHT;
        static const irr::video::SColor BACKGROUND_COLOR;
        static const irr::video::SColor TEXT_COLOR;
        
        irr::gui::IGUIEnvironment *m_pGuiEnv;
        irr::gui::IGUIFont *m_pFont;
        std::vector<irr::core::stringw> m_Lines;
        irr::core::stringw m_strInputLine;
        
        enum {CS_HIDDEN, CS_VISIBLE, CS_HIDING, CS_SHOWING} m_State;
        
        CTimer m_Timer;
        CTimer m_ToggleTimer;
        
        void AddLine(const irr::core::stringw &strLine);
};

#endif // CCLIENTCONSOLE_H
