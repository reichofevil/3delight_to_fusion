#ifndef _3D_ALLOCATOR_H_
#define _3D_ALLOCATOR_H_


#include "3D_DataTypes.h"


// ClassAllocator3D - use this if you want the constructor and destructor called on T.  ClassAllocator3D isn't designed to offer a speed increase,
//   but rather to prevent memory fragmentation by allocating entire chunks of T at the same time.
// StructAllocator3D - use this if T has empty constructors and destructors.  This template is designed for speed memory allocation/deallocation
//   and to reduce memory fragmentation.  (eg. use ClassAllocator3D if your struct contains a std::vector<> so its dtor gets called)
// MemAllocator3D - For allocating a large number of variably sized small objects that all will be freed at the same time.  The initial allocation
//   is done on a 16-byte boundary so that if only 16-byte boundary objects are requested, they'll all be aligned.
// Vector4fAllocator3D - use this if you want to do a lot of frequent allocations of single Vector4f or small arrays of Vector4f and all those
//   allocations will have the same lifetime.  The 3D scanline renderer uses this a lot since it can't simply examine a mesh and know ahead of 
//   time how many Vector4f it needs, as primitives in the mesh can be rejected/culled in various ways.  This also keeps all the vectors together
//   in chunks to avoid memory fragmentation and improve cache locality.



#ifdef _DEBUG
#undef new									// MFC-STL-MS memory debugging conflicts
#endif

#include <new>

// Notes:
//  - the class T must be at least as big as a pointer (32/64 bits) + 32 bytes
//  - the ctor and dtor for T *will* get called
//  - be careful if T has a specific memory alignment, as objects are allocated many at a single time in arrays
//  - if the Allocator<> might be accessed by multiple threads, you should bracket all your allocations with a ObtainLock/ReleaseLock
template <class T>
class ClassAllocator3D : public LockableObject
{

public:
	/**
	 * @param sizeInc  the number of item by which the allocator will grows it pool when it is full
	 * @param initSize the number of items initially in the pool
	 */
	ClassAllocator3D(int sizeInc = 128, int initSize = 0) 
	{ 
		FreeList = NULL; 
		MemUsage = 0; 
		CreateNewChunk(initSize);
		SizeIncrement = sizeInc;
	}

	~ClassAllocator3D() 
	{ 
		FreeAll(); 
	}

	/**
	 * Reserves additional memory to store up to n items.  This is extra ontop of any existing unused memory.
	 */
	void Reserve(int n) 
	{
		CreateNewChunk(n);
	}

	/**
	 * Sets how much the pool will grow next time it is enlarged and thereafter.
	 */
	void SetSizeIncrement(int sizeInc) 
	{
		SizeIncrement = sizeInc;
	}

	/**
	 * Returns memory usage in bytes (allocated + unallocated).
	 */
	int GetMemUsage() 
	{ 
		return MemUsage; 
	}

	/**
	 * Allocates a new item from the pool.  The constructor for T will be called.  On failure a CMemoryException is thrown.
	 */
	T *Alloc() 
	{  
		if (!FreeList)
			CreateNewChunk(SizeIncrement);

		void *ret = FreeList;
		FreeList = Next(FreeList);

		return new (ret) T;
	}

	/**
	 * Returns an item back to the pool.  The destructor will be called.  The pointer must not be NULL - this check is avoided for performance reasons.
	 */
	void Free(T *toFree)
	{
		toFree->~T();
		SetNext(toFree, FreeList);
		FreeList = toFree;
	}

	/**
	 * Returns a vector of items back to the pool.  The destructors will be called.  The pointers may be NULL.
	 */
	void Free(std::vector<T*> &v)
	{
		int n = v.size();
		for (int i = 0; i < n; i++)
			if (v[i])
				Free(v[i]);
		v.clear();
	}

	/**
	 * Frees all items allocated from this pool.  This is much more efficient than calling Free() on each item, however the destructor
	 * for each item is not called.
	 */
	void FreeAll()
	{
		FreeList = NULL;
		MemUsage = 0;

		std::vector<T*>::iterator i = Chunks.begin();
		std::vector<T*>::iterator end = Chunks.end();
		while (i != end)
		{
			_mm_free(*i);
			++i;
		}

		Chunks.clear();
	}

protected:
	class Dummy 
	{ 
	public:
		T *Next;
	};

	std::vector<T*> Chunks;
	T *FreeList;
	uint32 SizeIncrement;
	int64 MemUsage;

protected:
	T *Next(T *current)						{ return ((Dummy *)current)->Next; }
	void SetNext(T *current, T *next)	{ ((Dummy *)current)->Next = next; }


	void CreateNewChunk(int size)
	{
		if (size > 0)
		{
			int32 bytes = sizeof(T) * size;
			T *chunk = (T *) _mm_malloc(bytes, 16);
			if (!chunk)
				AfxThrowMemoryException();

			Chunks.push_back(chunk);
			MemUsage += bytes;

			T *c = chunk;
			for (int32 i = 0; i < size - 1; i++)
			{
				SetNext(c, c + 1);
				c++;
			}

			SetNext(c, FreeList);
			FreeList = chunk;
		}
	}
};




#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// Notes:
//  - the class T must be at least as big as a pointer (32/64 bits)
//  - the ctor and dtor for T will *not* get called
//  - be careful if T has a specific memory alignment, as objects are allocated many at a single time in arrays
//  - if the Allocator<> might be accessed by multiple threads, you should bracket all your allocations with a ObtainLock/ReleaseLock
template <class T>
class StructAllocator3D : public LockableObject
{
public:
	/**
	 * @param sizeInc  the number of item by which the allocator will grows it pool when it is full
	 * @param initSize the number of items initially in the pool
	 */
	StructAllocator3D(int sizeInc = 128, int initSize = 0) 
	{ 
		FreeList = NULL; 
		MemUsage = 0;
		CreateNewChunk(initSize);
		SizeIncrement = sizeInc;
	}

	~StructAllocator3D() 
	{ 
		FreeAll(); 
	}

	/**
	 * Reserves additional memory to store up to n items.  This is extra ontop of any existing unused memory.
	 */
	void Reserve(int n) 
	{
		CreateNewChunk(n);
	}

	/**
	 * Sets how much the pool will grow next time it is enlarged and thereafter.
	 */
	void SetSizeIncrement(int sizeInc) 
	{
		SizeIncrement = sizeInc;
	}

	/**
	 * Returns memory usage in bytes (allocated + unallocated).
	 */
	int GetMemUsage() 
	{ 
		return MemUsage; 
	}

	/**
	 * Allocates a new item from the pool.  Note that the constructor for T is not called.  On failure a CMemoryException is thrown.
	 */
	T *Alloc()
	{  
		if (!FreeList)
			CreateNewChunk(SizeIncrement);

		void *ret = FreeList;
		FreeList = Next(FreeList);

		return (T *) ret;
	}

	/**
	 * Returns an item back to the pool.  The pointer must not be NULL.
	 */
	void Free(T *toFree)
	{
		SetNext(toFree, FreeList);
		FreeList = toFree;
	}

	/**
	 * Frees all items allocated from this pool.  This is much more efficient than calling Free() on each item.
	 */
	void FreeAll()
	{
		FreeList = NULL;
		MemUsage = 0;

		std::vector<T*>::iterator i = Chunks.begin();
		std::vector<T*>::iterator end = Chunks.end();
		while (i != end)
		{
			_mm_free(*i);
			++i;
		}

		Chunks.clear();
	}

protected:
	class Dummy 
	{ 
	public:
		T *Next; 
	};

	std::vector<T*> Chunks;
	T *FreeList;
	int32 SizeIncrement;
	int64 MemUsage;

protected:
	T *Next(T *current)						{ return ((Dummy *)current)->Next; }
	void SetNext(T *current, T *next)	{ ((Dummy *)current)->Next = next; }

	void CreateNewChunk(int size)
	{
		if (size > 0)
		{
			uint32 bytes = sizeof(T) * size;
			T *chunk = (T *) _mm_malloc(bytes, 16);
			if (!chunk)
				AfxThrowMemoryException();

			Chunks.push_back(chunk);
			MemUsage += bytes;

			T *c = chunk;
			for (int i = 0; i < size - 1; i++)
			{
				SetNext(c, c + 1);
				c++;
			}

			SetNext(c, FreeList);
			FreeList = chunk;
		}
	}
};






class MemAllocator3D : public LockableObject
{
public:
	std::vector<uint8*> Chunks;
	uint8 *Current;
	int Remaining;
	int SizeIncrement;
	int64 MemUsage;		// in bytes

protected:
	bool CreateNewChunk(int bytes)
	{
		uint8 *chunk = (uint8 *) _mm_malloc(bytes, 16);
		if (!chunk)
			return false;

		Chunks.push_back(chunk);
		Remaining = bytes;
		Current = chunk;
		MemUsage += bytes;

		return true;
	}

public:
	MemAllocator3D() {}
	MemAllocator3D(int initSize, int sizeIncrement) { Init(initSize, sizeIncrement); }
	~MemAllocator3D() { FreeAll(); }

	bool Init(int initSize = 4 * 4096, int sizeIncrement = 4 * 4096)
	{
		FreeAll();
		SizeIncrement = sizeIncrement;
		return CreateNewChunk(initSize);
	}

	void *Alloc(int bytes) 
	{  
		if (bytes > Remaining)
			if (!CreateNewChunk(SizeIncrement))
				return NULL;

		Remaining -= bytes;
		void *ret = Current;
		Current += bytes;				

		return ret;
	}

	void FreeAll()
	{
		int nChunks = Chunks.size();
		for (int i = 0; i < nChunks; i++)
			_mm_free(Chunks[i]);
		Chunks.clear();
		MemUsage = 0;
	}
};






class Vector4fAllocator3D : public LockableObject
{
public:
	std::vector<Vector4f*> Chunks;
	Vector4f *Current;
	int Remaining;
	int SizeIncrement;
	int64 MemUsage;

public:
	/**
	 * @param  sizeInc the number of Vector4f's to allocate when enlarging the pool (should be >= 1024)
	 */
	Vector4fAllocator3D(int sizeInc = 4096, int initSize = 0) 
	{
		Current = NULL; 
		Remaining = 0; 
		MemUsage = 0;
		CreateNewChunk(initSize);
		SizeIncrement = sizeInc;
	}

	~Vector4fAllocator3D()
	{
		FreeAll();
	}

	/**
	 * Sets how much the pool will grow next time it is enlarged and thereafter.
	 */
	void SetSizeIncrement(int sizeInc) 
	{
		SizeIncrement = sizeInc;
	}

	/**
	 * Returns memory usage in bytes (allocated + unallocated).
	 */
	int GetMemUsage() 
	{ 
		return MemUsage; 
	}

	/**
	 * Allocates a block of 'n' Vector4fs 
	 */
	Vector4f *Alloc(int n)
	{
		if (Remaining < n)
			CreateNewChunk(SizeIncrement);

      Vector4f *ret = Current;
		Current += n;
		Remaining -= n;

		return ret;
	}

	void FreeAll()
	{
		int nChunks = Chunks.size();
		for (int i = 0; i < nChunks; i++)
			_mm_free(Chunks[i]);
		Chunks.clear();
	}

protected:
	void CreateNewChunk(int n)
	{
		Current = (Vector4f *) _mm_malloc(n * sizeof(Vector4f), 16);
		if (!Current)
			AfxThrowMemoryException();

		Chunks.push_back(Current);
		Remaining = n;
		MemUsage += n << 4;
	}
};



typedef StructAllocator3D<Matrix4f> M4Pool;







#endif