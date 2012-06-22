/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        vppmgr/main.cpp
*  PURPOSE:     VPP archiver
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <vpp_format.h>
#include <utils.h>
#include <CFileList.h>

using namespace std;

#define ALIGN(offset, alignment) (((offset) + (alignment) - 1) & (~(alignment - 1)))

class CVpp
{
    struct SFileEntry
    {
        string strPath;
        unsigned cBytes;
        unsigned nOffset;
    };
    
    public:
        inline CVpp() {}
        
        inline CVpp(const char *pPath, bool bWrite = false)
        {
            Open(pPath, bWrite);
        }
        
        int Open(const char *pPath, bool bWrite = false)
        {
            m_Stream.open(pPath, (bWrite ? ios_base::out : ios_base::in) | ios_base::binary);
            m_bWrite = bWrite;
            
            if(!m_Stream.good())
                return -1;
            
            if(!m_bWrite)
            {
                vpp_header_t Header;
                m_Stream.read((char*)&Header, sizeof(Header));
                m_cFiles = Header.file_count;
            }
            
            return 0;
        }
        
        bool IsOpen() const
        {
            return m_Stream.is_open();
        }
        
        void Close()
        {
            m_Stream.close();
        }
        
        int Unpack(unsigned i, const char *pPath)
        {
            if(m_bWrite)
                return -1;
            
            if(!m_Stream.is_open())
                return -1;
            
            if(m_Files.empty())
            {
                int iStatus = ReadFileList();
                if(iStatus < 0)
                    return iStatus;
            }
            
            if(i >= m_Files.size())
                return -1;
            
            m_Stream.seekg(m_Files[i].nOffset);
            if(!m_Stream.good())
                return -1;
            
            stringstream ssPath;
            ssPath << pPath << "/" << m_Files[i].strPath;
            ofstream File(ssPath.str().c_str(), ios_base::out | ios_base::binary);
            if(!File.is_open())
                return -1;
            
            unsigned cBytes = m_Files[i].cBytes;
            char Buf[4096];
            while(cBytes > 0)
            {
                if(!m_Stream.good() || !File.good())
                    return -1;
                
                unsigned cbRead = min(cBytes, 4096u);
                m_Stream.read(Buf, cbRead);
                File.write(Buf, cbRead);
                cBytes -= cbRead;
            }
            
            File.close();
            return 0;
        }
        
        int UnpackAll(const char *pPath)
        {
            for(unsigned i = 0; i < m_cFiles; ++i)
            {
                int iStatus = Unpack(i, pPath);
                if(iStatus < 0)
                    return iStatus;
            }
            
            return 0;
        }
        
        int Add(const char *pPath, unsigned nSize = 0)
        {
            SFileEntry Entry;
            Entry.strPath = pPath;
            Entry.cBytes = nSize ? nSize : GetFileSize(pPath);
            if(!Entry.cBytes)
                return -1;
            
            m_Files.push_back(Entry);
            return 0;
        }
        
        int Save()
        {
            char Buf[max(4096, VPP_ALIGNMENT)];
            unsigned cbBuf;
            
            if(!m_bWrite)
                return -1;
            
            vpp_header_t Header;
            Header.signature = VPP_SIGNATURE;
            Header.version = 1;
            Header.file_count = m_Files.size();
            Header.archive_size = VPP_ALIGNMENT + ALIGN(m_Files.size() * sizeof(vpp_entry_t), VPP_ALIGNMENT);
            
            for(unsigned i = 0; i < m_Files.size(); ++i)
                Header.archive_size += ALIGN(m_Files[i].cBytes, VPP_ALIGNMENT);
            
            m_Stream.write((char*)&Header, sizeof(Header));
            
            cbBuf = ALIGN((unsigned)m_Stream.tellg(), VPP_ALIGNMENT) - (unsigned)m_Stream.tellg();
            memset(Buf, 0, cbBuf);
            m_Stream.write(Buf, cbBuf);
            
            for(unsigned i = 0; i < m_Files.size(); ++i)
            {
                size_t nPos = m_Files[i].strPath.find_last_of("/\\");
                if(nPos != m_Files[i].strPath.npos)
                    ++nPos;
                else
                    nPos = 0;
                
                vpp_entry_t Entry;
                strcpy(Entry.file_name, m_Files[i].strPath.c_str() + nPos);
                Entry.file_size = m_Files[i].cBytes;
                m_Stream.write((char*)&Entry, sizeof(Entry));
            }
            
            cbBuf = ALIGN((unsigned)m_Stream.tellg(), VPP_ALIGNMENT) - (unsigned)m_Stream.tellg();
            memset(Buf, 0, cbBuf);
            m_Stream.write(Buf, cbBuf);
            
            for(unsigned i = 0; i < m_Files.size(); ++i)
            {
                ifstream File(m_Files[i].strPath.c_str(), ios_base::in | ios_base::binary);
                if(!File.is_open())
                    return -1;
                
                unsigned cBytes = m_Files[i].cBytes;
                while(cBytes > 0)
                {
                    if(!m_Stream.good() || !File.good())
                        return -1;
                    
                    cbBuf = min(cBytes, sizeof(Buf));
                    File.read(Buf, cbBuf);
                    m_Stream.write(Buf, cbBuf);
                    cBytes -= cbBuf;
                }
                
                cbBuf = ALIGN((unsigned)m_Stream.tellg(), VPP_ALIGNMENT) - (unsigned)m_Stream.tellg();
                memset(Buf, 0, cbBuf);
                m_Stream.write(Buf, cbBuf);
            }
            
            return 0;
        }
        
    private:
        fstream m_Stream;
        vector<SFileEntry> m_Files;
        unsigned m_cFiles;
        bool m_bWrite;
        
        unsigned GetFileSize(const char *pPath)
        {
            ifstream File(pPath, ios_base::in | ios_base::binary);
            if(!File.is_open())
                return 0;
            
            File.seekg(0, ios::end);
            return File.tellg();
        }
        
        int ReadFileList()
        {
            if(!m_Stream.is_open())
                return -1;
            
            m_Stream.seekg(VPP_ALIGNMENT);
            if(!m_Stream.good())
                return -1;
            
            unsigned nOffset = ALIGN(VPP_ALIGNMENT + m_cFiles * sizeof(vpp_entry_t), VPP_ALIGNMENT);
            for(unsigned i = 0; i < m_cFiles; ++i)
            {
                if(!m_Stream.good())
                    return -1;
                
                vpp_entry_t Entry;
                m_Stream.read((char*)&Entry, sizeof(Entry));
                
                SFileEntry File;
                File.strPath = Entry.file_name;
                File.cBytes = Entry.file_size;
                File.nOffset = nOffset;
                
                m_Files.push_back(File);
                
                nOffset += ALIGN(File.cBytes, VPP_ALIGNMENT);
            }
            
            return 0;
        }
};

int main(int argc, const char * argv[])
{
    const char *pPath = NULL;
    bool bCreate = false;
    bool bHelp = (argc < 2);
    CVpp Vpp;
    
    for(int i = 1; i < argc; ++i)
    {
        if(argv[i][0] == '/' || argv[i][0] == '-')
        {
            if(!StrCmpI(argv[i] + 1, "h") || !StrCmpI(argv[i] + 1, "help"))
                bHelp = true;
            else if(!StrCmpI(argv[i] + 1, "u"))
            {
                if(i + 1 < argc)
                {
                    if(bCreate)
                    {
                        cout << "Saving " << pPath << "...";
                        int iStatus = Vpp.Save();
                        if(iStatus >= 0)
                            cout << "ok\n";
                        else
                            cout << "failed (" << iStatus << "\n";
                        
                    }
                    
                    pPath = argv[++i];
                    bCreate = false;
                }
            }
            else if(!StrCmpI(argv[i] + 1, "c"))
            {
                if(i + 1 < argc)
                {
                    pPath = argv[++i];
                    bCreate = true;
                    Vpp.Open(pPath, true);
                }
            }
            else
                cerr << "Invalid option: " << argv[i] << "\n";
        }
        else
        {
            CFileList FileList(argv[i]);
            
            string strFileName;
            while(FileList.GetNextFile(strFileName) >= 0)
            {
                string strPath = argv[i];
                size_t nPos = strPath.find_last_of("/\\");
                if(nPos != strPath.npos)
                    strPath.resize(nPos + 1);
                else
                    strPath.resize(0);
                strPath += strFileName;
                
                if(!bCreate)
                {
                    
                    cout << "Unpacking " << strPath << "...";
                    int iStatus = Vpp.Open(strPath.c_str());
                    if(iStatus >= 0)
                    {
                        iStatus = Vpp.UnpackAll(pPath);
                        
                        if(iStatus >= 0)
                            cout << "ok\n";
                        else
                            cout << "failed (" << iStatus << ")\n";
                    }
                    else
                        cerr << "failed (" << iStatus << ")\n";
                    Vpp.Close();
                }
                else
                {
                    cout << "Adding " << strPath << "...";
                    int iStatus = Vpp.Add(strPath.c_str());
                    if(iStatus >= 0)
                        cout << "ok\n";
                    else
                        cerr << "failed (" << iStatus << ")\n";
                }
            }
        }
    }
    
    if(bCreate)
    {
        cout << "Saving " << pPath << "...";
        int iStatus = Vpp.Save();
        if(iStatus >= 0)
            cout << "ok\n";
        else
            cout << "failed (" << iStatus << "\n";
    }
    
    if(bHelp)
        cout << "Usage: " << argv[0] << " [-c vpp_path] [-u output_path] file1 file2 ...\n";
    
    return 0;
}
