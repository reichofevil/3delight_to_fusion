#ifndef _IOCLASS_H_
#define _IOCLASS_H_

#include "RegistryLists.h"
#include "Object.h"

enum IOCOpenModes
{
	IOC_OPEN_READ        = 1,
	IOC_OPEN_WRITE       = 2,
	IOC_OPEN_READWRITE   = (IOC_OPEN_READ | IOC_OPEN_WRITE),
	IOC_OPEN_UNBUFFERED  = 4,    // combine with others for unbuffered I/O
	IOC_OPEN_SHARED      = 8,    // combine with others for shared access,
                                 // except IOC_OPEN_READ which gets shared access anyway
};

enum IOCSeekModes
{
	IOC_SEEK_START = 1,
	IOC_SEEK_CUR,
	IOC_SEEK_END
};

/*
 * This is a generic I/O class to package a file ptr with its
 * associated routines, for passing to functions.
 */

class FuSYSTEM_API IOClass : public Object
{
	FuDeclareClass(IOClass, Object); // declare ClassID static member, etc.

protected:
	void    *Fp;            /* ptr to FILE, TapeFILE, etc */
	int64    StartOfFile;   /* For IOC_SEEK_STARTs - offset from real SOF */
	int64    FileSize;
	bool     LastDidRead;

public:
	char    *Filename;   /* Optional */
	uint32   Flags;      /* see below */

public:
	IOClass(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~IOClass();

	virtual bool    Open (const char *name, int openmode = IOC_OPEN_READ);
	virtual void    Close(void);
	virtual int64   Read (void *buf, int64 bytes);
	virtual int64   Write(const void *buf, int64 bytes);
	virtual int64   Seek (int64 bytes, int seekmode);
	virtual bool    Flush(void);
	virtual void    SetStartPtr(void);
	virtual int64   GetFilePos (void);
	virtual int64   GetFileSize(void);
	virtual void    SetFileSize(int64 size);

	virtual int     ReadLine(char *buf, uint32 maxlen);
	virtual int     WriteLine(const char *buf);

	virtual uint32  GetBlockSize();
};

/* Flags for above */
#define	IOCF_BYTESWAPPED        1L      /* Byteswap all data when reading    */
#define	IOCF_WRITE              2L      /* File opened for writing           */
#define	IOCF_OPEN               4L      /* File opened.                      */
#define	IOCF_READWRITE          8L      /* File opened for writing, but can read (needs IOCF_WRITE) */
#define IOCF_UNBUFFERED        16L      /* Opened for unbuffered I/O. Use GetBlockSize() to find your alignment needs */

// Tags to use in an IOClass's Registry Node.
enum IOClassReg_Tags
{
	IOREG_Wrapper = TAG_INT + REG_USER,         // Default: 0      (eg. TapeIO wraps everything in '.TAR')
	
	IOREG_IsThisProtocol = TAG_PTR + REG_USER,	// Default: NULL
};

typedef bool (*IsThisProtocolPtr) (const char *filename);

#endif
