/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/COutputBinaryStream.h
*  PURPOSE:     Write-only binary stream
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CNETOUTPUTSTREAM_H
#define CNETOUTPUTSTREAM_H

#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif

class COutputBinaryStream: public std::ostream
{
    public:
        inline COutputBinaryStream(std::streambuf *pStreamBuf):
            std::ostream(pStreamBuf) {}
        
        inline void WriteUInt8(uint8_t n)
        {
            write((char*)&n, sizeof(n));
        }
        
        inline void WriteUInt16(uint16_t n)
        {
            write((char*)&n, sizeof(n));
        }
        
        inline void WriteUInt32(uint32_t n)
        {
            write((char*)&n, sizeof(n));
        }
        
        inline void WriteInt8(int8_t n)
        {
            write((char*)&n, sizeof(n));
        }
        
        inline void WriteInt16(int16_t n)
        {
            write((char*)&n, sizeof(n));
        }
        
        inline void WriteInt32(int32_t n)
        {
            write((char*)&n, sizeof(n));
        }
        
        inline void WriteFloat(float f)
        {
            write((char*)&f, sizeof(f));
        }
        
        inline void WriteString(const char *pStr)
        {
            write(pStr, strlen(pStr) + 1);
        }
        
        inline void WriteString(const std::string &Str)
        {
            write(Str.c_str(), Str.size() + 1);
        }
        
        inline void WriteVector(const btVector3 &Vec)
        {
            WriteFloat(Vec.x());
            WriteFloat(Vec.y());
            WriteFloat(Vec.z());
        }
        
        inline void WriteVector(const btQuaternion &q)
        {
            WriteFloat(q.x());
            WriteFloat(q.y());
            WriteFloat(q.z());
            WriteFloat(q.w());
        }
        
        inline void WriteBinary(const void *pData, unsigned cbData)
        {
            write((char*)pData, cbData);
        }
        
        inline void WriteBinary(const std::string &strData)
        {
            write((char*)strData.data(), strData.size());
        }
};

class COutputBinaryStringStream: private std::stringbuf, public COutputBinaryStream
{
    public:
        inline COutputBinaryStringStream():
            std::stringbuf(std::ios_base::out), COutputBinaryStream(this) {}
        
        inline const std::string str() const
        {
            return std::stringbuf::str();
        }
        
        inline void str(const std::string &Str)
        {
            std::stringbuf::str(Str);
        }
};

#endif // CNETOUTPUTSTREAM_H
