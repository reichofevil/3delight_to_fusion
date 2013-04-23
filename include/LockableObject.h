#ifndef _LOCKABLEOBJECT_H_
#define _LOCKABLEOBJECT_H_

#include "Object.h"


class _FuLock;

class FuSYSTEM_API LockableObject : public Object
{
	FuDeclareClass(LockableObject, Object); // declare ClassID static member, etc.

protected:
	void *m_Pad1, *m_Pad2;
	int32 m_Pad3, m_Pad4, m_Pad5;
	
	_FuLock *m_Lock;

public:
	LockableObject();
	LockableObject(const LockableObject &toCopy);
	LockableObject(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~LockableObject();
	
	void ObtainLock();
	void ReleaseLock();
	bool AttemptLock(uint32 timeout = 0);

	void ObtainReadLock();
	void ReleaseReadLock();

	bool IsReadLocked();
	bool IsWriteLocked();
	bool IsLocked();

	int32 GetWriteLockCount();

#ifndef RELEASE_VERSION
	void AssertWriteLock();
	void AssertReadLock();
#else
	void AssertWriteLock()	{}
	void AssertReadLock()	{}
#endif

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(LockableObject);
#endif
};


#define LOCKABLEOBJECT_BASE OBJECT_BASE + TAGOFFSET


class AutoLock
{
	LockableObject *lock;

	void ObtainLock()							{ if (lock) lock->ObtainLock(); }

public:
	AutoLock(LockableObject &_lock)		{ lock = &_lock; ObtainLock(); }
	AutoLock(LockableObject *_lock)		{ lock = _lock; ObtainLock(); }
	AutoLock(const LockableObject &_lock)		{ lock = (LockableObject *)&_lock; ObtainLock(); }		// should be safe when used in scope, since it's always left as it was found
	AutoLock(const LockableObject *_lock)		{ lock = (LockableObject *)_lock; ObtainLock(); }
	~AutoLock()									{ ReleaseLock(); }

	void ReleaseLock()						{ if (lock) { lock->ReleaseLock(); lock = NULL; } }
};

class AutoReadLock
{
	LockableObject *lock;

	void ObtainReadLock()					{ if (lock) lock->ObtainReadLock(); }

public:
	AutoReadLock(LockableObject &_lock)	{ lock = &_lock; ObtainReadLock(); }
	AutoReadLock(LockableObject *_lock)	{ lock = _lock; ObtainReadLock(); }
	AutoReadLock(const LockableObject &_lock)	{ lock = (LockableObject *)&_lock; ObtainReadLock(); }
	AutoReadLock(const LockableObject *_lock)	{ lock = (LockableObject *)_lock; ObtainReadLock(); }
	~AutoReadLock()							{ ReleaseReadLock(); }

	void ReleaseReadLock()					{ if (lock) { lock->ReleaseReadLock(); lock = NULL; } }
	void ReleaseLock()						{ ReleaseReadLock(); }
};





class FuSYSTEM_API FuLock
{
	_FuLock *m_Lock;

public:
	FuLock();
	virtual ~FuLock();
	
	void ObtainLock();
	void ReleaseLock();
	bool AttemptLock(uint32 timeout = 0);

	void ObtainReadLock();
	void ReleaseReadLock();

	bool IsReadLocked();
	bool IsWriteLocked();
	bool IsLocked();

	int32 GetWriteLockCount();

#ifndef RELEASE_VERSION
	void AssertWriteLock();
	void AssertReadLock();
#else
	void AssertWriteLock()	{}
	void AssertReadLock()	{}
#endif
};


class AutoFuLock
{
	FuLock *lock;

	void ObtainLock()							{ if (lock) lock->ObtainLock(); }

public:
	AutoFuLock(FuLock &_lock)				{ lock = &_lock; ObtainLock(); }
	AutoFuLock(FuLock *_lock)				{ lock = _lock; ObtainLock(); }
	AutoFuLock(const FuLock &_lock)		{ lock = (FuLock *)&_lock; ObtainLock(); }		// should be safe when used in scope, since it's always unlocked
	AutoFuLock(const FuLock *_lock)		{ lock = (FuLock *)_lock; ObtainLock(); }
	~AutoFuLock()									{ ReleaseLock(); }

	void ReleaseLock()						{ if (lock) { lock->ReleaseLock(); lock = NULL; } }
};

class AutoReadFuLock
{
	FuLock *lock;

	void ObtainReadLock()					{ if (lock) lock->ObtainReadLock(); }

public:
	AutoReadFuLock(FuLock &_lock)			{ lock = &_lock; ObtainReadLock(); }
	AutoReadFuLock(FuLock *_lock)			{ lock = _lock; ObtainReadLock(); }
	AutoReadFuLock(const FuLock &_lock)	{ lock = (FuLock *)&_lock; ObtainReadLock(); }
	AutoReadFuLock(const FuLock *_lock)	{ lock = (FuLock *)_lock; ObtainReadLock(); }
	~AutoReadFuLock()							{ ReleaseReadLock(); }

	void ReleaseReadLock()					{ if (lock) { lock->ReleaseReadLock(); lock = NULL; } }
	void ReleaseLock()						{ ReleaseReadLock(); }
};


#endif
