#ifndef _STREAM_SET_3D_H_
#define _STREAM_SET_3D_H_

#include "3D_Stream.h"


const int NUM_SW_CUST_STREAMS = 1;	

const int CUST_STREAM_SCREEN_POS = 0;		// the first stream number is reserved for (ssp.x, ssp.y, clip.z, 1/w)
//const int CUST_STREAM_EYE_POS = 1;



// Wrapper class around the stream list that handles the use/recycles
class FuSYSTEM_API StreamSet3D
{
public:
	std::vector<Stream3D*> StreamList;		// anything in here is reference counted and owned by this StreamSet

public:
	StreamSet3D() {}

	StreamSet3D(const StreamSet3D &toCopy) 
	{
		*this = toCopy;
	}

	~StreamSet3D()
	{
		Clear();
	}

	StreamSet3D &operator=(const StreamSet3D &rhs);

	void Clear()
	{
		int nStreams = StreamList.size();
		for (int i = 0; i < nStreams; i++)
			SAFE_RECYCLE(StreamList[i]);
		StreamList.clear();
	}

	// Performs full copy, no refcnting
	void Copy(StreamSet3D &ss, bool shallow = false)
	{
		int nStreams = ss.StreamList.size();

		if (shallow)
		{
			for (int i = 0; i < nStreams; i++)
			{
				Stream3D *stream = ss.StreamList[i];
				SAFE_USE(stream);
				StreamList.push_back(stream);
			}
		}
		else
		{
			for (int i = 0; i < nStreams; i++)
			{
				Stream3D *stream = ss.StreamList[i];
				StreamList.push_back(stream ? stream->Copy() : NULL);
			}
		}
	}

	int Size()
	{
		return StreamList.size();
	}

	void Transfer(StreamSet3D &s)
	{
		int nStreams = s.StreamList.size();
		for (int i = 0; i < nStreams; i++)
			StreamList.push_back(s.StreamList[i]);
		s.StreamList.clear();
	}

	uint64 EstimateMemoryUsage()
	{
		uint64 mem = 0;
		int nStreams = StreamList.size();
		for (int i = 0; i < nStreams; i++)
			mem += StreamList[i]->EstimateMemoryUsage();
		return mem;
	}

	Stream3D * &operator[](int i) { return StreamList[i]; }

	Stream3D * const &operator[](int i) const { return StreamList[i]; }

	void Zero(int idx)
	{
		if (StreamList[idx])
		{
			StreamList[idx]->Recycle();
			StreamList[idx] = 0;
		}
	}

	int Add(Stream3D *stream)
	{
		uassert(stream);
		int ret = StreamList.size();
		StreamList.push_back(stream);
		return ret;
	}

	// replace the given stream if it already exists, otherwise just add it
	int Replace(Stream3D *stream)
	{
		int idx = GetIndex(stream->Name);
		if (idx != -1)
		{
			StreamList[idx]->Recycle();
			StreamList[idx] = stream;
		}
		else
			idx = Add(stream);

		return idx;
	}

	void AddFront(Stream3D *stream)
	{
		uassert(stream);
		StreamList.insert(StreamList.begin(), stream);
	}

	bool IsEmpty()
	{
		return StreamList.empty();
	}

	Stream3D *GetStream(int idx)
	{
		return StreamList[idx];
	}

	void Compact()
	{
		int nStreams = StreamList.size();
		int dst = 0;
		for (int src = 0; src < nStreams; src++)
			if (StreamList[src])
				StreamList[dst++] = StreamList[src];

		int nRemoves = nStreams - dst;
		for (int i = 0; i < nRemoves; i++)
			StreamList.pop_back();
	}

	void Swap(int i, int j)
	{
		Stream3D *tmp = StreamList[i];
		StreamList[i] = StreamList[0];
		StreamList[0] = tmp;
	}

	Stream3D *Find(const FuID &streamName)
	{
		int idx = GetIndex(streamName);
		return idx != -1 ? StreamList[idx] : NULL;
	}

	Stream3D *Find(const FuID &streamName, CS_CoordSpace cs)
	{
		int idx = GetIndex(streamName, cs);
		return idx != -1 ? StreamList[idx] : NULL;
	}

	void *GetTuplePtr(const FuID &streamName, CS_CoordSpace cs)
	{
		int idx = GetIndex(streamName, cs);
		return idx != -1 ? StreamList[idx]->Tuples : NULL;
	}

	int GetIndex(const FuID &streamName)
	{
		int nStreams = StreamList.size();
		for (int i = 0; i < nStreams; i++)
			if (streamName == StreamList[i]->Name)
				return i;
		return -1;
	}

	int GetIndex(const FuID &streamName, CS_CoordSpace cs)
	{
		int nStreams = StreamList.size();
		for (int i = 0; i < nStreams; i++)
			if (streamName == StreamList[i]->Name && cs == StreamList[i]->CoordSpace)
				return i;
		return -1;
	}

	bool DebugIsValid();

	//Stream3D *CreateStream(RenderContext3D &rc, SN_StreamName name, CS_CoordSpace cs);
};



#endif