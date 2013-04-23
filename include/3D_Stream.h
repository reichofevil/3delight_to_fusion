#ifndef _STREAM_3D_H_
#define _STREAM_3D_H_

#include "VectorMath.h"
#include "3D_Resource.h"
#include "3D_DataTypes.h"



// Stream Type Flags
enum
{
	STF_Material,		// this stream was declared by a material or a light or both ???????????????????????
	STF_Light,
};


class StreamType3D
{
	FuID Name;
	CS_CoordSpace CoordSpace;
	TT_TransformType TransType;
	uint32 Flags;
	uint16 ShortID;
};






//
//
// class IndexArray3D - an array of integers indexing per-vertex attributes.  IndexArray3D objects are immutable after they have been 
//   initially created since they can be shared among different objects and threads.  Indices should be specified counter clockwise
//   when looking at the front of a primitive.
//
//
//
class FuSYSTEM_API IndexArray3D : public Resource3D
{
public:
	uint32 NumIndices;
	uint32 *Data;

	volatile LONG VBO;

public:
	IndexArray3D();										// prefer IndexArray3D::Alloc
	IndexArray3D(const IndexArray3D &toCopy);		// nyi
	~IndexArray3D(); 

	static IndexArray3D *Alloc(int numIndices);	// throws CMemoryException

	IndexArray3D *Copy();								// Does a FULL copy - no reference counting is done

	bool DebugIsValid();

	virtual bool Upload();
	virtual void FreeData(TagList *tags = NULL);

	void BindGL();				// Binds this array's VBO for usage in GL and uploads it if it doesn't exist (the array should be Activate()d before calling this)
	void UnbindGL();

	virtual void Use();
	virtual void Recycle();

	void Fill(uint32 *i, int numIndices)
	{
		uassert(Data);
		memcpy(Data, i, sizeof(uint32) * numIndices);
	}

	uint64 EstimateMemoryUsage()
	{
		return sizeof(IndexArray3D) + NumIndices * sizeof(uint32);
	}
};






//
//
// class Stream3D - an array of per-vertex attributes.  eg. position (x, y, z), normal (nz, ny, nz), fog (f), texcoord (u, v).  Stream objects
//   are immutable after they have been initially created since they can be shared among different objects and threads.
//
class FuSYSTEM_API Stream3D : public Resource3D
{
public:
	float32 *Tuples;							// use counted, 16 byte aligned array for SSE
	
	StreamType3D *StreamType;

	FuID Name;
	CS_CoordSpace CoordSpace;
	TT_TransformType TransType;			// Alloc() will try to set this to a reasonable default based on the name, but you may have to set it manually
	
	uint32 TupleSize;							// number of float32 elements in each tuple - usually 1, 2, 3 or 4
	uint32 NumTuples;							// number of Tuples in the tuples array

	uint32 TexChannel;						// currently unused, defaults to 0, used to match textures to texture coordinate sets

	volatile LONG VBO;						// does it have an existing vertex buffer? (usually located in AGP/card memory)
	HW_Stream StreamNum;

public:
	Stream3D();									// prefer the static Alloc() method below
	Stream3D(const Stream3D &rhs);		// nyi
	virtual ~Stream3D();

	static Stream3D *Alloc(const FuID &name, CS_CoordSpace cs, uint32 numTuples, uint32 tupleSize);	// throws CMemoryException 

	Stream3D *Copy();							// Does a FULL copy - no reference counting is done

	bool DebugIsValid();

	int NumBytes() { return sizeof(float32) * TupleSize * NumTuples; }

	uint64 EstimateMemoryUsage()
	{
		return sizeof(Stream3D) + TupleSize * NumTuples * sizeof(float32);
	}

	void BindGL();				// Binds this Stream's VBO for usage in GL and uploads it if it doesn't exist (the stream should be Activate()d before calling this)
	void UnbindGL();

	virtual void Use();
	virtual void Recycle();

	//void BindCg();				// Binds this Stream's VBO for usage in Cg and uploads it if it doesn't exist (the stream should be Activate()d before calling this)
	//void UnbindCg();

	// Upload will upload the data into a VBO it isn't already so.  It will leave the VBO active in GL.  When calling Upload() you will want to 
	//  Activate() the resource beforehand, the reason being if you call Activate() after Upload(), the stream's vbo could be freed between the 
	//  Upload() and Activate() calls.
	virtual bool Upload();

	virtual void FreeData(TagList *tags = NULL);


	Stream3D *Flatten(const IndexArray3D *ia) const; // throws CMemoryException

	/**
	 * Transforms each tuple by the supplied matrix.  The TransformType affects how the transform is done.  Can return NULL if the transform
	 * doesn't make sense.  The coordinate system of the return stream needs to be set.
	 */
	Stream3D *TransformAsPoint(const Matrix4f &m, CS_CoordSpace cs); // throws CMemoryException

	Stream3D *TransformAsVector(const Matrix4f &m, CS_CoordSpace cs); // throws CMemoryException

	/**
	 * Transforms normals or other unit vectors by the supplied matrix.  The supplied matrix must be the inverse transpose of the transform
	 * matrix.  The resulting vectors are renormalized afterwards.  The coordinate system of the return stream needs to be set.
	 * @param m the inverse transpose
	 */
	Stream3D *TransformAsUnitVector(const Matrix4f &invTr, CS_CoordSpace cs); // throws CMemoryException

	// Gets the i'th tuple
	void GetTuple(int i, Vector4f &val)
	{
		i *= TupleSize;
		for (int j = 0; j < TupleSize; j++)
			val[j] = Tuples[i + j];
	}

	// Computes the bounding box of this stream's data
	void GetMinMax(float32 minValue[4], float32 maxValue[4]);

	// Various Fill() functions for filling of the attribute array from a memory array of various types or filling the array with copies 
	// of an individual object.
	void Fill(Vector4f *v)
	{
		uassert(TupleSize == 4 && Tuples);
		memcpy(Tuples, v, sizeof(float32) * 4 * NumTuples);
	}

	void Fill(Vector3f *v)
	{
		uassert(TupleSize == 3 && Tuples);
		memcpy(Tuples, v, sizeof(float32) * 3 * NumTuples);
	}

	void Fill(Vector2f *v)
	{
		uassert(TupleSize == 2 && Tuples);
		memcpy(Tuples, v, sizeof(float32) * 2 * NumTuples);
	}

	void Fill(Vector4f &v)
	{
		uassert(TupleSize == 4 && Tuples);
		int maxi = 4 * NumTuples;
		for (int i = 0; i < maxi; i += 4)
			(Vector4f &) Tuples[i] = v;
	}

	void Fill(Vector3f &v)
	{
		uassert(TupleSize == 3 && Tuples);
		int maxi = 3 * NumTuples;
		for (int i = 0; i < maxi; i += 3)
			(Vector3f &) Tuples[i] = v;
	}

	void Fill(Vector2f &v)
	{
		uassert(TupleSize == 2 && Tuples);
		int maxi = 2 * NumTuples;
		for (int i = 0; i < maxi; i += 2)
			(Vector2f &) Tuples[i] = v;
	}

	Vector2f &Vec2(uint32 idx) 
	{ 
		uassert(TupleSize == 2 && Tuples);
		return (Vector2f &) Tuples[2 * idx];
	}

	Vector3f &Vec3(uint32 idx) 
	{ 
		uassert(TupleSize == 3 && Tuples);
		return (Vector3f &) Tuples[3 * idx];
	}

	Vector3f &Pt3(uint32 idx) 
	{ 
		uassert(TupleSize == 3 && Tuples);
		return (Vector3f &) Tuples[3 * idx];
	}

	Color4f &Col4(uint32 idx)
	{ 
		uassert(TupleSize == 4 && Tuples);
		return (Color4f &) Tuples[4 * idx];
	}

	Vector4f &Vec4(uint32 idx) 
	{ 
		uassert(TupleSize == 4 && Tuples);
		return (Vector4f &) Tuples[4 * idx];
	}

	//float32 *PickWeldEpsilon() 
	//{
	//	float32 minVal[4], maxVal[4];
	//	GetMinMax(minVal, maxVal);
	//}

	/*
	// Welds together vertices.  When a duplicate vertex is found it is replaced with the previously encountered vertex.  This doesn't use
	// the midpt, so it gives unintuitive results for large epsilon.
	Stream3D *Weld(float32 epsilon) 
	{
		FuASSERT(DebugIsValid(), (""));

		//if (epsilon == -1.0f)
		//	epsilon = PickWeldEpsilon();

		hash_map<> hmap;


		Stream3D *ret = Stream3D::Alloc();
		IndexArray3D *ind = IndexArray3D::Alloc();
		if (!ret || !ind)
		{
			SAFE_RECYCLE(ret)
			SAFE_RECYCLE(ind);
			return NULL;
		}

		ret->Indices = ind;
		return ret;
	}
	*/
};







#endif