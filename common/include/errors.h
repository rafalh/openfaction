/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        include/errors.h
*  PURPOSE:     Error identifier used by engine
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

enum EOfStatus
{
    OFS_SUCCESS                  = 0,
    
    OFE_UNKNOWN_ERROR           = -1,
    
    OFE_CANNOT_START_WSA        = -10,
    OFE_CANNOT_BIND_SOCKET      = -11,
    OFE_CANNOT_CREATE_SOCKET    = -12,
    
    OFE_FILE_NOT_FOUND          = -10,
    OFE_FILE_READ_ERROR         = -11,
    OFE_FILE_SEEK_ERROR         = -12,
    OFE_INVALID_SIGNATURE       = -13,
    OFE_INVALID_LEVEL           = -14,
};

#endif // ERRORS_H_INCLUDED
