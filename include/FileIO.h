#ifndef _FILEIO_H_
#define _FILEIO_H_

#include "IOClass.h"

class IOReq;

/*
 * This is a generic I/O class to package a file ptr with its
 * associated routines, for passing to functions.
 */
class FuSYSTEM_API FileIO : public IOClass
{
	FuDeclareClass(FileIO, IOClass); // declare ClassID static member, etc.

	IOReq **IOReqs;
	int64 DiskPos, NextDiskPos, FilePos;
	int NextReq, CurrentReq;
	int NumBuffers;

	LockableObject IOReqLock;

	void QueueBuffers(int64 pos);
	IOReq *GetNextBuffer();

public:
	FileIO();
	FileIO(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~FileIO();

	virtual bool	Open (const char *name, int openmode = IOC_OPEN_READ);
	virtual void	Close (void);
	virtual int64	Read (void *buf, int64 bytes);
	virtual int64	Write (const void *buf, int64 bytes);
	virtual int64	Seek (int64 bytes, int seekmode);
	virtual bool	Flush (void);
	virtual void	SetStartPtr (void);
	virtual int64	GetFilePos  (void);
};

#endif // _FILEIO_H_
