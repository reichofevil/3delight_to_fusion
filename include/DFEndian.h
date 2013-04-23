#ifndef __ENDIAN_H__
#define __ENDIAN_H__

/*---------------------------------------------------------
// ENDIAN.H : Byte order compatibility functions
//
// Written by Will McGovern
//
// Copyright (c)1993 by New York Production & Design
//---------------------------------------------------------
//
// HISTORY :
// ---------
//
// 13-MAR-93  WMC  Created file
//
//---------------------------------------------------------
*/

#include "types.h"

/*---------------------------------------------------------
// Endian type flags
*/

#define BIG_ENDIAN		TRUE
#define LITTLE_ENDIAN   FALSE

/*---------------------------------------------------------
// Name : uint32 d = GetLong( uint8 *p, bool m )
//
// Desc : Fetch unsigned long according to endian flag.
//
// Parm : p = pointer to unsigned long data.
//        m = TRUE  - big endian mode.
//            FALSE - little endian mode.
//
// Retn : d = unsigned long data value
//
// Glob : None
//
// Note : 32-bit long values are assumed.
*/

DLLExtern uint32 GetLong( void *, bool );

/*---------------------------------------------------------
// Name : uint16 d = GetShort( uint8 *p, bool m )
//
// Desc : Fetch unsigned short according to endian flag.
//
// Parm : p = pointer to unsigned short data.
//        m = TRUE  - big endian mode.
//            FALSE - little endian mode.
//
// Retn : d = unsigned long data value
//
// Glob : None
//
// Note : 16-bit short values are assumed.
*/

DLLExtern uint16 GetShort( void *, bool );

/*---------------------------------------------------------
// Name : void PutLong( uint8 *p, uint32 d, bool m )
//
// Desc : Put unsigned long according to endian flag.
//
// Parm : p = pointer to 4-byte area for long data
//        d = long to write into data area.
//        m = TRUE  - big endian mode.
//            FALSE - little endian mode.
//
// Retn : None
//
// Glob : None
//
// Note : 32-bit long values are assumed.
*/

DLLExtern void PutLong( void *, uint32 , bool );

/*---------------------------------------------------------
// Name : void PutShort( uint8 *p, uint16 d, bool m )
//
// Desc : Put unsigned short according to endian flag.
//
// Parm : p = pointer to 2-byte area for short data.
//        d = short to write into data area.
//        m = TRUE  - big endian mode.
//            FALSE - little endian mode.
//
// Retn : None
//
// Glob : None
//
// Note : 16-bit short values are assumed.
*/

DLLExtern void PutShort( void *, uint16 , bool );

/* like above, but for floats */
DLLExtern float	GetFloat( void *, bool );
DLLExtern void 	PutFloat( void *, float, bool );

/* Byte swaps a buffer */
DLLExtern void ByteSwap(char *buf, int size);

#endif
