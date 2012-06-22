/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/CInputBinaryStream.h
*  PURPOSE:     Read-only binary stream
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CINPUTBINARYSTREAM_H
#define CINPUTBINARYSTREAM_H

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <btBulletCollisionCommon.h>

#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif

class CInputBinaryStream: public std::istream
{
    public:
        inline CInputBinaryStream(std::streambuf *pStreamBuf):
            std::istream(pStreamBuf) {}
        
        inline uint32_t ReadUInt32()
        {
            uint32_t Ret = 0;
            read((char*)&Ret, sizeof(Ret));
            return Ret;
        }
        
        inline uint16_t ReadUInt16()
        {
            uint16_t Ret = 0;
            read((char*)&Ret, sizeof(Ret));
            return Ret;
        }
        
        inline uint8_t ReadUInt8()
        {
            uint8_t Ret = 0;
            read((char*)&Ret, sizeof(Ret));
            return Ret;
        }
        
        inline int32_t ReadInt32()
        {
            int32_t Ret = 0;
            read((char*)&Ret, sizeof(Ret));
            return Ret;
        }
        
        inline int16_t ReadInt16()
        {
            int16_t Ret = 0;
            read((char*)&Ret, sizeof(Ret));
            return Ret;
        }
        
        inline int8_t ReadInt8()
        {
            int8_t Ret = 0;
            read((char*)&Ret, sizeof(Ret));
            return Ret;
        }
        
        inline float ReadFloat()
        {
            float Ret = 0.0f;
            read((char*)&Ret, sizeof(Ret));
            return Ret;
        }
        
        inline std::string ReadString()
        {
            char Char;
            std::string Ret;
            
            while((Char = (char)ReadUInt8()))
                Ret += Char;
            
            return Ret;
        }
        
        inline std::string ReadString2()
        {
            unsigned nLen = ReadUInt16();
            
            return ReadBinary(nLen);
        }
        
        inline btVector3 ReadVector()
        {
            btVector3 Vec;
            Vec.setX(ReadFloat());
            Vec.setY(ReadFloat());
            Vec.setZ(ReadFloat());
            return Vec;
        }
        
        inline btQuaternion ReadQuaternion()
        {
            btQuaternion q;
            q.setX(ReadFloat());
            q.setY(ReadFloat());
            q.setZ(ReadFloat());
            q.setW(ReadFloat());
            return q;
        }
        
        inline void ReadBinary(void *pBuffer, unsigned cbBuffer)
        {
            read((char*)pBuffer, cbBuffer);
        }
        
        inline std::string ReadBinary(unsigned cBytes)
        {
            char *pBuf = new char[cBytes];
            read(pBuf, cBytes);
            std::string StrResult = std::string(pBuf, cBytes);
			delete pBuf;
			return StrResult;
        }
};

class CInputBinaryStringStream: private std::stringbuf, public CInputBinaryStream
{
    public:
        CInputBinaryStringStream(const std::string &Str, std::ios_base::openmode Which = std::ios_base::in):
            std::stringbuf(Str, Which), CInputBinaryStream(this) {}
        
        CInputBinaryStringStream(std::ios_base::openmode Which = std::ios_base::in):
            std::stringbuf(Which), CInputBinaryStream(this) {}
};

#endif // CINPUTBINARYSTREAM_H
