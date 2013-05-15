/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/CClientConsole.cpp
*  PURPOSE:     
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "CClientConsole.h"
#include "CTimer.h"
#include <cstdarg>
#include <cstdio>
#include <cctype>

using namespace irr;
using namespace std;

const int CClientConsole::LINES_LIMIT = 10;
const int CClientConsole::TOGGLE_TIME = 150;
const int CClientConsole::HEIGHT = 160;
const video::SColor CClientConsole::BACKGROUND_COLOR(160, 0, 0, 0);
const video::SColor CClientConsole::TEXT_COLOR(160, 255, 255, 255);

CClientConsole::CClientConsole(irr::gui::IGUIEnvironment *pGuiEnv):
    m_pGuiEnv(pGuiEnv), m_pFont(NULL), m_State(CS_HIDDEN) {}

CClientConsole::~CClientConsole()
{
    if(m_pFont)
        m_pFont->drop();
}

void CClientConsole::VPrint(const char *pszFormat, va_list ap)
{
    char szBuf[256];
    
    vsnprintf(szBuf, sizeof(szBuf) - 1, pszFormat, ap);
    szBuf[sizeof(szBuf) - 1] = 0;
    
    core::stringw strLine;
    for(const char *Ptr = szBuf; *Ptr; ++Ptr)
        strLine += (wchar_t)*Ptr;
    
    AddLine(strLine);
    
#ifdef DEBUG
    printf("%s", szBuf);
#endif // DEBUG
}

void CClientConsole::Render()
{
    if(m_State == CS_HIDDEN)
        return;
    
    core::dimension2du ScrSize = m_pGuiEnv->getVideoDriver()->getScreenSize();
    core::dimension2du ConsoleSize(ScrSize.Width, HEIGHT);
    if(m_State == CS_SHOWING)
    {
        ConsoleSize.Height = min((int)m_ToggleTimer.GetValue() * HEIGHT / TOGGLE_TIME, HEIGHT);
        if((int)ConsoleSize.Height == HEIGHT)
            m_State = CS_VISIBLE;
    }
    else if(m_State == CS_HIDING)
    {
        ConsoleSize.Height = max((TOGGLE_TIME - (int)m_ToggleTimer.GetValue()) * HEIGHT / TOGGLE_TIME, 0);
        if((int)ConsoleSize.Height == HEIGHT)
            m_State = CS_HIDDEN;
    }
    
    core::rect<s32> rcConsole(0, 0, ConsoleSize.Width, ConsoleSize.Height);
    m_pGuiEnv->getVideoDriver()->draw2DRectangle(BACKGROUND_COLOR, rcConsole);
    
    gui::IGUIFont *pFont = m_pFont ? m_pFont : m_pGuiEnv->getBuiltInFont();
    unsigned cyFont = pFont->getDimension(L"").Height;
    if(ConsoleSize.Height < cyFont)
        return;
    
    unsigned cLinesVisible = ConsoleSize.Height / cyFont - 1;
    if(cLinesVisible > m_Lines.size())
        cLinesVisible = m_Lines.size();
    
    unsigned y = 0;
    for(unsigned i = m_Lines.size() - cLinesVisible; i < m_Lines.size(); ++i)
    {
        core::recti rcLine(2, y, ConsoleSize.Width - 2, y + cyFont);
        pFont->draw(m_Lines[i], rcLine, TEXT_COLOR);
        y += cyFont;
    }
    
    core::recti rcLine(2, ConsoleSize.Height - cyFont, ConsoleSize.Width - 2, ConsoleSize.Height);
    pFont->draw(m_strInputLine, rcLine, TEXT_COLOR);
    
    if(m_Timer.GetValue() % 1000 < 500)
    {
        rcLine.UpperLeftCorner.X += pFont->getDimension(m_strInputLine.c_str()).Width;
        pFont->draw(L"_", rcLine, TEXT_COLOR);
    }
}

bool CClientConsole::OnEvent(const SEvent &Event)
{
    if(Event.EventType == EET_KEY_INPUT_EVENT)
    {
        if(Event.KeyInput.PressedDown)
        {
            if(Event.KeyInput.Char == '~' || Event.KeyInput.Char == '`')
            {
                Toggle();
                return true;
            }
            else if(m_State == CS_VISIBLE)
            {
                if(Event.KeyInput.Key == KEY_BACK)
                {
                    if(m_strInputLine.size() > 0)
                    m_strInputLine.erase(m_strInputLine.size() - 1);
                }
                else if(Event.KeyInput.Key == KEY_RETURN)
                {
                    if(m_strInputLine.size() > 0)
                    {
                        AddLine(m_strInputLine);
                        // TODO: Execute command
                        m_strInputLine = L"";
                    }
                }
                else if(isprint(Event.KeyInput.Char))
                    m_strInputLine += Event.KeyInput.Char;
                else
                    return false;
                
                m_Timer.Reset();
                return true;
            }
        }
    }
    
    return false;
}

void CClientConsole::AddLine(const irr::core::stringw &strLine)
{
    m_Lines.push_back(strLine);
    
    if((int)m_Lines.size() > LINES_LIMIT)
        m_Lines.erase(m_Lines.begin());
}
