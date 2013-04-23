#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#ifndef DFMANAGER

#include "Object.h"
#include "TagList.h"
#include "TimeExtent.h"
#include "ScriptVal.h"
//#include "Input.h"  Argh, stupid include problems

class Input;
class Request;

class FuSYSTEM_API Parameter : public Object
{
	FuDeclareClass(Parameter, Object); // declare ClassID static member, etc.

public:
	int32 CacheUseCount, UnmappedUseCount;
	int32 UseLockThread, UseLockNest;

	double TimeCost;
	TagList Attrs;
#ifndef USE_NOSCRIPTING
	ScriptVal m_CustomData;
#endif
	// Alignment forced this to 8 anyway
	bool CacheMe;
	bool ReadOnly;
	bool _flagpad[6];

	int32 WeakUseCount;

	uint8 pad[28];

	Parameter();
	Parameter(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Parameter(const Parameter &p); // Copy Constructor
	virtual ~Parameter();

	virtual bool Save(ScriptVal &table, const TagList &tags);

	virtual Parameter *Copy();
	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual Parameter *InterpolateWith(double weight, Parameter *rightvalue, TagList *tags = NULL);

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual bool UpdatePublish(void *data, Parameter *param, TagList *tags = NULL);

	virtual void Use();
	virtual void Recycle();

	virtual void CacheUse();
	virtual void CacheRecycle(bool allowswap = false);

	virtual void UnmappedUse();
	virtual void UnmappedRecycle();

	virtual void ObtainUseLock();
	virtual void ReleaseUseLock();
	
	bool IsReadOnly();

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Parameter);
#endif

protected:
	virtual bool _IsValid();
};




#define PARAMETER_BASE (OBJECT_BASE + TAGOFFSET)

enum ParameterAttrTags ENUM_TAGS
{
	PARAM_Obsolete = PARAMETER_BASE | TAG_INT,
	PARAM_Quality,
	PARAM_RealQuality,
	PARAM_MotionBlurQuality,
	PARAM_HasMotionBlur,
	PARAM_LimitFlags,
	PARAM_NoCache,
	PARAM_ProxyScale,
	PARAM_NoData,

	PARAM_AllowNoDataMismatch,									// Used with EventID_Param_IsComparable - mismatch of PARAM_NoData is OK
	PARAM_ForceSizeCompare,										// Used with EventID_Param_IsComparable - compare sizes even if its not a mask

	PARAMD_LeftHandleX = PARAMETER_BASE | TAG_DOUBLE,
	PARAMD_LeftHandleY,
	PARAMD_RightHandleX,
	PARAMD_RightHandleY,

	PARAMP_Compare = PARAMETER_BASE | TAG_PTR,			// Used with EventID_Param_IsComparable - what to compare against
	PARAMP_Owner,
	PARAM_MemorySize,												// Needs to be 64bit value on 64bit OS
	PARAMP_Request,												// Can optionally be passed to EventID_Param_Limit, but PARAM_LimitFlags overrides its flags

	PARAMO_ImageDoD = PARAMETER_BASE | TAG_OBJECT,
	PARAMO_ImageRoI,
};

class FuSYSTEM_API AutoParameter
{
protected:
	Parameter *Param;
public:
	AutoParameter()			{ Param = NULL; }
	AutoParameter(Input *in, TimeStamp time);
	AutoParameter(Input *in, Request *req, TimeStamp time = TIME_UNDEFINED);
	~AutoParameter()			{if (Param) Param->Recycle(); }
};

class AutoParameterLock
{
protected:
	Parameter *m_Param;

public:
	AutoParameterLock(Parameter &param)			{ m_Param = &param;					ObtainLock(); }
	AutoParameterLock(Parameter *param)			{ m_Param = param;					ObtainLock(); }
	AutoParameterLock(const Parameter &param)	{ m_Param = (Parameter *)&param; ObtainLock(); }
	AutoParameterLock(const Parameter *param)	{ m_Param = (Parameter *)param;	ObtainLock(); }
	~AutoParameterLock()								{ ReleaseUseLock(); }

	void ObtainLock()									{ if (m_Param) m_Param->ObtainUseLock(); }
	void ReleaseUseLock()							{ if (m_Param) { m_Param->ReleaseUseLock(); m_Param = NULL; } }
};

template <class ObjClass> class FuDumbPointer : public FuPointerBase
{
	FuDeclareClass(FuDumbPointer, FuPointerBase); // declare ClassID static member, etc.

protected:
	Parameter *m_Obj;

public:
	FuDumbPointer(ObjClass *obj = NULL)
	{
		m_Obj = obj;
	}

	FuDumbPointer(const FuDumbPointer &p)
	{
		m_Obj = p.m_Obj;
	}

	~FuDumbPointer()
	{
		m_Obj = NULL;
	}

	ObjClass *operator = (ObjClass *obj)
	{
		m_Obj = obj;

		return obj;
	}

	const FuDumbPointer &operator = (const FuDumbPointer &p)
	{
		if (m_Obj != p.m_Obj)
			*this = (ObjClass *)p.m_Obj;

		return *this;
	}

	bool operator ==(const FuDumbPointer &p) const { return (m_Obj == p.m_Obj); }
	bool operator !=(const FuDumbPointer &p) const { return (m_Obj != p.m_Obj); }

	bool operator ==(FuDumbPointer &p) const { return (m_Obj == p.m_Obj); }
	bool operator !=(FuDumbPointer &p) const { return (m_Obj != p.m_Obj); }

	bool operator ==(const ObjClass *p) const { return (m_Obj == p); }
	bool operator !=(const ObjClass *p) const { return (m_Obj != p); }

	bool operator ==(ObjClass *p) const { return (m_Obj == p); }
	bool operator !=(ObjClass *p) const { return (m_Obj != p); }

	ObjClass * operator ->()                 { return (ObjClass *)m_Obj; }
	const ObjClass * operator ->() const     { return (const ObjClass *)m_Obj; }

	//TODO: Ideally we wouldn't need these...
	operator ObjClass *() const  { return (ObjClass *)m_Obj; }
	ObjClass &operator *() const { return *(ObjClass *)m_Obj; }
};

// Reference-Counted Pointer to any Parameter derived class, that only obtains a Cache use on the param
template <class ObjClass> class FuCachePointer : public FuDumbPointer<ObjClass>
{
	FuDeclareClass(FuCachePointer, FuDumbPointer<ObjClass>); // declare ClassID static member, etc.

public:
	FuCachePointer(ObjClass *obj = NULL) : FuDumbPointer<ObjClass> (obj)
	{
		if (m_Obj)
			m_Obj->CacheUse();
	}

	FuCachePointer(ObjClass *obj, bool addref) : FuDumbPointer<ObjClass> (obj)
	{
		if (m_Obj && addref)
			m_Obj->CacheUse();
	}

	FuCachePointer(const FuCachePointer &p)
	{
		m_Obj = p.m_Obj;

		if (m_Obj)
			m_Obj->CacheUse();
	}

	~FuCachePointer()
	{
		if (m_Obj)
			m_Obj->CacheRecycle();
	}

	ObjClass *operator = (ObjClass *obj)
	{
		if (obj != (ObjClass *)m_Obj)
		{
			if (m_Obj)
				m_Obj->CacheRecycle();

			m_Obj = obj;

			if (m_Obj)
				m_Obj->CacheUse();
		}

		return obj;
	}

	const FuCachePointer &operator = (const FuCachePointer &p)
	{
		if (m_Obj != p.m_Obj)
			*this = (ObjClass *)p.m_Obj;

		return *this;
	}

	bool operator ==(const FuCachePointer &p) const { return (m_Obj == p.m_Obj); }
	bool operator !=(const FuCachePointer &p) const { return (m_Obj != p.m_Obj); }

	ObjClass * operator ->()                 { return (ObjClass *)m_Obj; }
	const ObjClass * operator ->() const     { return (const ObjClass *)m_Obj; }

	//TODO: Ideally we wouldn't need these...
	operator ObjClass *() const  { return (ObjClass *)m_Obj; }
	ObjClass &operator *() const { return *(ObjClass *)m_Obj; }
};


// Reference-Counted Pointer to any Parameter derived class, that only obtains an Unmapped use on the param
template <class ObjClass> class FuUnmappedPointer : public FuDumbPointer<ObjClass>
{
	FuDeclareClass(FuUnmappedPointer, FuDumbPointer<ObjClass>); // declare ClassID static member, etc.

public:
	FuUnmappedPointer(ObjClass *obj = NULL) : FuDumbPointer<ObjClass> (obj)
	{
		if (m_Obj)
			m_Obj->UnmappedUse();
	}

	FuUnmappedPointer(ObjClass *obj, bool addref) : FuDumbPointer<ObjClass> (obj)
	{
		if (m_Obj && addref)
			m_Obj->UnmappedUse();
	}

	FuUnmappedPointer(const FuUnmappedPointer &p)
	{
		m_Obj = p.m_Obj;

		if (m_Obj)
			m_Obj->UnmappedUse();
	}

	~FuUnmappedPointer()
	{
		if (m_Obj)
			m_Obj->UnmappedRecycle();
	}

	ObjClass *operator = (ObjClass *obj)
	{
		if (obj != (ObjClass *)m_Obj)
		{
			if (m_Obj)
				m_Obj->UnmappedRecycle();

			m_Obj = obj;

			if (m_Obj)
				m_Obj->UnmappedUse();
		}

		return obj;
	}

	const FuUnmappedPointer &operator = (const FuUnmappedPointer &p)
	{
		if (m_Obj != p.m_Obj)
			*this = (ObjClass *)p.m_Obj;

		return *this;
	}

	bool operator ==(const FuUnmappedPointer &p) const { return (m_Obj == p.m_Obj); }
	bool operator !=(const FuUnmappedPointer &p) const { return (m_Obj != p.m_Obj); }

	ObjClass * operator ->()                 { return (ObjClass *)m_Obj; }
	const ObjClass * operator ->() const     { return (const ObjClass *)m_Obj; }

	//TODO: Ideally we wouldn't need these...
	operator ObjClass *() const  { return (ObjClass *)m_Obj; }
	ObjClass &operator *() const { return *(ObjClass *)m_Obj; }
};

#endif
#endif
