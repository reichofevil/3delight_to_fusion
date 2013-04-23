#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include "Link.h"
#include "TimeExtentList.h"

#define CLASS_OUTPUT CLSID_Output

class Operator;
class Clip;

class FuSYSTEM_API Output : public Link
{
	FuDeclareClass(Output, Link); // declare ClassID static member, etc.

	int NumActiveConnects;

public:
	List ConnectionList;

	//Disk caching
	Clip *DiskCache;
	int32 NoDiskWriteCount, NoDiskReadCount;
	bool DiskCacheEnabled, DiskCacheLocked;

	FuID InstanceOut;
	Output *RealOutput;

	TimeExtentList m_DoDList;
	MsgPort *m_pPreCalcMsgPort;

	uint8 pad[28];

public:
	Output(const FuID &name, /*uint32 id, */const FuID &dt, Operator *op);
	Output(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~Output();

	static void RegCleanup(Registry *regnode);

	virtual bool Save(ScriptVal &table, const TagList &tags);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual void SetRequestData(Parameter *param, TimeStamp time, const TimeExtent& valid, uint32 flags = 0, TimeStamp basetime = TIME_UNDEFINED, TagList *tags = NULL);		// see Link.h for SRQ_ flags
	virtual bool SetSource(Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual bool PutMsg(Message *msg);

	virtual bool AddLink(Input *in, TagList *tags = NULL);
	virtual bool RemoveLink(Input *in, TagList *tags = NULL);
	virtual void InvalidateCache(TagList *tags = NULL);
	virtual void InvalidateCache(const TimeExtent& te, TagList *tags = NULL);
	virtual bool CheckRecursiveConnect(Link *lnk, TagList *tags = NULL);
	bool DoCheckRecursiveConnect(Link *lnk, TagList *tags = NULL);
	virtual bool OnConnected(Link *link, Link *oldsrc, Link *newsrc, TagList *tags = NULL);
	bool DoOnConnected(Link *link, Link *oldsrc, Link *newsrc, TagList *tags = NULL);

	virtual void Dump();

	virtual bool IsUsed(TagList *tags = NULL) const;
	virtual bool IsConnected() const;
	virtual bool IsConnectedTo(Link *link, TagList *tags = NULL);
	virtual int32 GetNumConnects() const;

	void Set(Request *req, Parameter *param, TagList *tags = NULL) { req->SetOutputData(this, param); }
	Parameter *Get(Request *req, TagList *tags = NULL) { return req->GetOutputData(this); }

	virtual void CachedExtent(const TimeExtent *te, bool skipthis = FALSE, TagList *tags = NULL);

	bool DoDiskCaching(TagList *tags = NULL);
	bool EnableDiskCache(bool enable = true, const char *filename = NULL, bool lockcache = false, bool lockbranch = false, bool deletefiles = false, bool prerender = false, bool usenetwork = false, TagList *tags = NULL);
	bool ReadDiskCache(TimeStamp time, uint32 minflags = NULL, TagList *tags = NULL);
	bool WriteDiskCache(Parameter *param, const TimeExtent& valid, TimeStamp time, TagList *tags = NULL);
	bool ClearDiskCache(const TimeExtent& te, TagList *tags = NULL);
	bool DeleteDiskCache(const char *cachepath, TagList *tags = NULL);

	Parameter *GetValue(TimeExtent &valid, TimeStamp time = TIME_UNDEFINED, uint32 flags = 0, int proxy = 1, TagList *tags = NULL);
	bool GetValueScriptVal(ScriptVal &result, ScriptVal &attrs, TimeStamp time = TIME_UNDEFINED, uint32 flags = 0, int proxy = 1, TagList *tags = NULL);

	virtual void RestoreState(UndoState *us, TagList *tags = NULL);
	virtual UndoState *SaveState(TagList *tags = NULL);

#if !defined(USE_NOSCRIPTING)
	DECLARE_LUATYPE(Output);
#endif
};

#define OUTPUT_BASE	LINK_BASE + TAGOFFSET

enum OUT_DiskCache_Tags ENUM_TAGS
{
	OUTB_ConfirmCacheDelete = TAG_INT | OUTPUT_BASE,		// accepted by DeleteDiskCache()
};

#endif

