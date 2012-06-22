/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        server/precomp.h
*  PURPOSE:     Precompiled header
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef PRECOMP_H_INCLUDED
#define PRECOMP_H_INCLUDED

#define _USE_MATH_DEFINES // MSVC needs it

#include "config.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <ctime>
#include <climits>
#include <cctype>
#include <cmath>
#include <cassert>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <btBulletCollisionCommon.h>

#if LINUX
# include <csignal>
# include <errno.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <pthread.h>
#else // !LINUX
# include <winsock2.h> /* include it before windows.h! */
# include <windows.h>
#endif

#endif // PRECOMP_H_INCLUDED
