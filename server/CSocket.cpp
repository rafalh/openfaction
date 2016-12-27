/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/CSocket.cpp
*  PURPOSE:     Static function used for packets debuging
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "util/CSocket.h"

#ifdef _DEBUG
void DumpBinData(const void *pData, unsigned cbData, FILE *File)
{
    unsigned i = 0, j;
    const unsigned char * const lpBytes = (unsigned char*)pData;

    for(i = 0; i < cbData; i += 16)
    {
        for(j = 0; j < 16 && i + j < cbData; ++j)
            fprintf(File, "%02X ", lpBytes[i+j]);

        for(; j < 16; ++j)
            fprintf(File, "   ");

        fprintf(File, "   ");

        for(j = 0; j < 16 && i + j < cbData; ++j)
            fputc(lpBytes[i+j] >= 0x20 && lpBytes[i+j] <= 0x7F ? lpBytes[i+j] : '.', File);

        if(i + 16 < cbData)
            fprintf(File, "\n");
    }

    fprintf(File, "\n");
}
#endif
