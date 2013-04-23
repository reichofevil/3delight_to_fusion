#ifndef _MemoryManager_h_
#define _MemoryManager_h_

#include "LockableObject.h"
#include "List.h"

#ifdef _DEBUG
#undef new
#endif
#include <fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Memory flags.

// Low 16 bits are requested attributes
#define MEMF_Clear        (1 << 0)  // Clear memory on allocation
#define MEMF_Contiguous   (1 << 2)  // Allocated Memory must be contiguous
#define MEMF_BottomUp     (1 << 3)  // Chunk ptr array must be bottom-up

// High 16 bits are state flags
#define MEMF_SwapPending  (1 << 16)  // Swap out when convenient...
#define MEMF_ReadOnly     (1 << 17)  // Read Only
#define MEMF_IsContiguous (1 << 18)  // Currently mapped in as contiguous

struct Memory;
class MemPointer;

class FuSYSTEM_API MemoryManager : public LockableObject
{
	FuDeclareClass(MemoryManager, LockableObject); // declare ClassID static member, etc.

	void Recycle(Memory *mem, bool force);

protected:
	uint64 m_TotalMemory;
	uint64 m_PeakTotalMemory;
	uint64 m_MappedMemory;
	uint64 m_PeakMappedMemory;

	uint32 m_TotalBlocks;
	uint32 m_PeakTotalBlocks;
	uint32 m_MappedBlocks;
	uint32 m_PeakMappedBlocks;

	uint32 m_MemoryMapSize;
	Memory *m_MemoryMap;

	uint32 m_FreeMemoryMapSize;
	Memory *m_FreeMemoryMap;

	uint32 m_PageSize;

	ListObj<Object> m_HandlerList;

protected:
	typedef uint32 MemHandle;

	void Use(MemHandle mh);
	void Recycle(MemHandle mh);

	bool MapIn(MemHandle mh, void **&ptr);
	void MapOut(MemHandle mh, void **&ptr);

	bool SwapOut(MemHandle mh);

	void SetReadOnly(MemHandle mh, bool ro);

	bool AllocChunks(MemHandle mh);

	bool IsContiguous(MemHandle mh);
	bool IsSwappedOut(MemHandle mh);

	friend class MemPointer;

public:
	MemoryManager();
	~MemoryManager();

	void UseManager();

	const MemPointer Alloc(uint32 size, uint32 count = 1, const char *name = "Mem", uint32 flags = 0, uint32 lastextrasize = 0);
	const MemPointer Alloc(uint32 size, uint32 count = 1, int32 start = 0, int32 offset = 0, const char *name = "Mem", uint32 flags = 0, uint32 lastextrasize = 0);

	bool AddMemHandler(Object *obj, int32 pri);
	bool RemoveMemHandler(Object *obj);

	bool FreeMemory(uint_ptr amount);

	static bool DumpMemoryMap(std::ostream &buffer);
};

extern FuSYSTEM_API MemoryManager *g_MemoryManager;

class FuSYSTEM_API MemPointer
{
protected:
	MemoryManager            *m_Manager;
	MemoryManager::MemHandle  m_Handle;

	MemPointer(MemoryManager *manager, MemoryManager::MemHandle handle);

public:
	MemPointer();
	MemPointer(const MemPointer &mp);

	~MemPointer();

	const MemPointer &operator = (const MemPointer &mp);

	inline operator bool() const { return (m_Handle != NULL); }

	inline bool operator == (const MemPointer &mp) const { return (m_Handle == mp.m_Handle) && (m_Manager == mp.m_Manager); }
	inline bool operator != (const MemPointer &mp) const { return !((m_Handle == mp.m_Handle) && (m_Manager == mp.m_Manager)); }

	inline bool MapIn(void **&ptr) const { if (m_Handle) return m_Manager->MapIn(m_Handle, ptr); else return false; }
	inline void MapOut(void **&ptr) const { if (m_Handle) m_Manager->MapOut(m_Handle, ptr); }

	inline bool SwapOut() const { if (m_Handle) return m_Manager->SwapOut(m_Handle); else return false; }
	inline void SetReadOnly(bool ro) const { if (m_Handle) m_Manager->SetReadOnly(m_Handle, ro); }

	inline bool IsContiguous() const { if (m_Handle) return m_Manager->IsContiguous(m_Handle); else return false; }
	inline bool IsSwappedOut() const { if (m_Handle) return m_Manager->IsSwappedOut(m_Handle); else return false; }

	friend class MemoryManager;
};

#endif // _MemoryManager_h_
