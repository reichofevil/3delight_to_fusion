#ifndef _DF_SAFE_PTR_H_
#define _DF_SAFE_PTR_H_

#include "Parameter.h"


#define DFSAFEPTR_BUSY		((ObjClass *)-1)

// Reference-Counted, multithread-safe Pointer to any RefObject derived class
template <class ObjClass> class FuSafePtr
{
protected:
	RefObject *m_Obj;

	ObjClass *Lock() const		// well, not strictly const perhaps
	{
		ObjClass *obj;
		while ((obj = (ObjClass *)InterlockedExchangePointer((void **)&m_Obj, DFSAFEPTR_BUSY)) == DFSAFEPTR_BUSY)		// spinlock until we get the ptr
			Sleep(0);		// might be faster not to Sleep() for multiCPU systems, on average
		return obj;
	}
	void Release(RefObject *obj) const
	{
		ASSERT(m_Obj == DFSAFEPTR_BUSY);
		InterlockedExchangePointer((void **)&m_Obj, obj);
	}
	ObjClass *Obj() const
	{
		ObjClass *obj = Lock();
		Release(obj);

		return obj;				// may no longer be valid
	}

public:
	FuSafePtr(ObjClass *obj = NULL)
	{
		m_Obj = obj;

		if (m_Obj)
			m_Obj->Use();
	}

	FuSafePtr(ObjClass *obj, bool addref)
	{
		m_Obj = obj;

		if (m_Obj && addref)
			m_Obj->Use();
	}

	FuSafePtr(FuSafePtr &p)
	{
		m_Obj = p.Lock();

		if (m_Obj)
			m_Obj->Use();

		p.Release(m_Obj);
	}

	~FuSafePtr()
	{
		ASSERT(m_Obj != DFSAFEPTR_BUSY);
		if (m_Obj)
			m_Obj->Recycle();
	}

	ObjClass *operator = (ObjClass *obj)
	{
		ObjClass *t = Lock();

		if (obj != t)
		{
			if (t)
				t->Recycle();

			t = obj;

			if (t)
				t->Use();
		}
		Release(t);

		return obj;
	}

	FuSafePtr &operator = (FuSafePtr<ObjClass> &p)
	{
		ObjClass *t = p.Lock();		// could still deadlock, if you assigned two DFSafePtrs to each other in different threads;

		*this = t;

		p.Release(t);

		return *this;
	}

	ObjClass *UseGet() const		// returns a Use()d object (or NULL) - don't forget to Recycle() it
	{
		ObjClass *obj = Lock();

		if (obj)
			obj->Use();

		Release(obj);
		return obj;
	}

	// none of the below are guaranteed
	bool operator ==(FuSafePtr &p) const { return (Obj() == p.Obj()); }
	bool operator !=(FuSafePtr &p) const { return (Obj() != p.Obj()); }

	operator ObjClass *() const    { return Obj(); }

	ObjClass &operator *() const   { return *Obj(); }
	ObjClass * operator ->() const { return Obj(); }
};


// Reference-Counted, multithread-safe Unmapped Pointer to any Parameter derived class
template <class ObjClass> class FuSafeUnmappedPtr
{
protected:
	Parameter *m_Obj;

	ObjClass *Lock() const		// well, not strictly const perhaps
	{
		ObjClass *obj;
		while ((obj = (ObjClass *)InterlockedExchangePointer((void **)&m_Obj, DFSAFEPTR_BUSY)) == DFSAFEPTR_BUSY)		// spinlock until we get the ptr
			Sleep(0);		// might be faster not to Sleep() for multiCPU systems, on average
		return obj;
	}
	void Release(RefObject *obj) const
	{
		ASSERT(m_Obj == DFSAFEPTR_BUSY);
		InterlockedExchangePointer((void **)&m_Obj, obj);
	}
	ObjClass *Obj() const
	{
		ObjClass *obj = Lock();
		Release(obj);

		return obj;				// may no longer be valid
	}

public:
	FuSafeUnmappedPtr(ObjClass *obj = NULL)
	{
		m_Obj = obj;

		if (m_Obj)
			m_Obj->UnmappedUse();
	}

	FuSafeUnmappedPtr(ObjClass *obj, bool addref)
	{
		m_Obj = obj;

		if (m_Obj && addref)
			m_Obj->UnmappedUse();
	}

	FuSafeUnmappedPtr(FuSafeUnmappedPtr &p)
	{
		m_Obj = p.Lock();

		if (m_Obj)
			m_Obj->UnmappedUse();

		p.Release(m_Obj);
	}

	~FuSafeUnmappedPtr()
	{
		ASSERT(m_Obj != DFSAFEPTR_BUSY);
		if (m_Obj)
			m_Obj->UnmappedRecycle();
	}

	ObjClass *operator = (ObjClass *obj)
	{
		ObjClass *t = Lock();

		if (obj != t)
		{
			if (t)
				t->UnmappedRecycle();

			t = obj;

			if (t)
				t->UnmappedUse();
		}
		Release(t);

		return obj;
	}

	FuSafeUnmappedPtr &operator = (FuSafeUnmappedPtr<ObjClass> &p)
	{
		ObjClass *t = p.Lock();		// could still deadlock, if you assigned two DFSafePtrs to each other in different threads;

		*this = t;

		p.Release(t);

		return *this;
	}

	ObjClass *UseGet() const		// returns a UnmappedUse()d param (or NULL) - don't forget to UnmappedRecycle() it
	{
		ObjClass *obj = Lock();

		if (obj)
			obj->UnmappedUse();

		Release(obj);
		return obj;
	}

	// none of the below are guaranteed
	bool operator ==(FuSafeUnmappedPtr &p) const { return (Obj() == p.Obj()); }
	bool operator !=(FuSafeUnmappedPtr &p) const { return (Obj() != p.Obj()); }

	operator ObjClass *() const    { return Obj(); }

	ObjClass &operator *() const   { return *Obj(); }
	ObjClass * operator ->() const { return Obj(); }
};



// 4-byte multithread-safe pointer to any class (no reference counting)
template <class ObjClass> class ThreadSafePtr
{
protected:
	ObjClass *m_Obj;

	ObjClass *Obj() const
	{
		ObjClass *obj = Lock();
		Release(obj);

		return obj;				// may no longer be valid
	}

public:
	ThreadSafePtr(ObjClass *obj = NULL)
	{
		m_Obj = obj;
	}

	ThreadSafePtr(ThreadSafePtr &p)
	{
		m_Obj = p.Lock();

		p.Release(m_Obj);
	}

	~ThreadSafePtr()
	{
		ASSERT(m_Obj != DFSAFEPTR_BUSY);
	}

	ObjClass *Lock() const		// well, not strictly const perhaps
	{
		ObjClass *obj;
		while ((obj = (ObjClass *)InterlockedExchangePointer((void **)&m_Obj, DFSAFEPTR_BUSY)) == DFSAFEPTR_BUSY)		// spinlock until we get the ptr
			Sleep(0);		// might be faster not to Sleep() for multiCPU systems, on average
		return obj;
	}
	void Release(ObjClass *obj) const
	{
		ASSERT(m_Obj == DFSAFEPTR_BUSY);
		InterlockedExchangePointer((void **)&m_Obj, obj);
	}

	ObjClass *operator = (ObjClass *obj)
	{
		ObjClass *t = Lock();

		t = obj;

		Release(t);

		return obj;
	}

	ThreadSafePtr &operator = (ThreadSafePtr<ObjClass> &p)
	{
		ObjClass *t = p.Lock();		// could still deadlock, if you assigned two DFSafePtrs to each other in different threads;

		*this = t;

		p.Release(t);

		return *this;
	}

	// none of the below are guaranteed
	bool operator ==(ThreadSafePtr &p) { return (Obj() == p.Obj()); }
	bool operator !=(ThreadSafePtr &p) { return (Obj() != p.Obj()); }

	operator ObjClass *() const   { return Obj(); }

	ObjClass &operator *() const  { return *Obj(); }
	ObjClass *operator ->() const { return Obj(); }
};

// Auto lock/release class for use with ThreadSafePtr
// Usage:
//		ThreadSafePtr<void> MemTSPtr = (void *)malloc(memsize);
//		AutoLockPtr<void> mem(MemTSPtr);
template <class ObjClass> class AutoLockPtr
{
protected:
	ThreadSafePtr<ObjClass>& m_Ptr;
	ObjClass *m_Obj;

public:
	AutoLockPtr(ThreadSafePtr<ObjClass>& ptr)	: m_Ptr(ptr)	{ m_Obj = m_Ptr.Lock(); }
	~AutoLockPtr()															{ m_Ptr.Release(m_Obj); }
	
	ObjClass *operator = (ObjClass *obj)
	{
		m_Obj = obj;
		return m_Obj;
	}

	// none of the below are guaranteed
	bool operator ==(ObjClass *p) { return (m_Obj == p); }
	bool operator !=(ObjClass *p) { return (m_Obj != p); }

	operator ObjClass *() const   { return m_Obj; }

	ObjClass &operator *() const  { return *m_Obj; }
	ObjClass *operator ->() const { return m_Obj; }
};

#endif