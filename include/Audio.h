#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <mmsystem.h>
#include "Parameter.h"
#include "Input.h"
#define AUDIO_ID zMAKEID('Aud')
//#define CLSID_DataType_Audio AUDIO_ID


class FuSYSTEM_API Audio : public Parameter
{
	FuDeclareClass(Audio, Parameter); // declare ClassID static member, etc.

protected:
	bool Allocated;

public:
	union 
	{
		uint32	*Data32;
		uint16	*Data16;
		uint8		*Data8;
		void		*Data;
	};
	uint32	NumSamples;		// Number of Samples :-)
	uint32	SampleRate;		// Hz
	uint8		SampleBits;		// 8/16 bits
	uint8		NumChannels;	// usually 1 or 2
	uint32	DataSize;
	
	WAVEFORMATEX *WFmtX;
	
	TimeStamp Start, End;

public:
	Audio(void *data, uint32 numsamp, uint32 samprate, uint8 sampbits, uint8 numchans, bool alloc = TRUE);
	Audio(void *data, uint32 numsamp, WAVEFORMATEX *wfmtx);
	Audio(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Audio(const char *sfile, TimeStamp start = 0.0, TimeStamp duration = -1.0);
	Audio(Audio &param);
	virtual ~Audio();
	virtual Parameter *Copy();
	
	static const Audio &Get(Input *in, Request *req) { return *(Audio *) in->GetValue(req); }

	virtual bool Save(ScriptVal &table, const TagList &tags);

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

public:
	static Audio *LoadData(const char *sfile, FusionDoc *doc, TimeStamp startfr = 0.0, TimeStamp durationfr = -1.0);	// frames
	static Audio *LoadData(const char *sfile, FusionDoc *doc, int32 startsamp, int32 durationsamps = -1);					// samples
	bool Decompress();

	int32 GetSampleBytes()  { return (SampleBits >> 3) * NumChannels; }
	uint8 *GetSampleAt(uint32 samplenum)  { return &Data8[GetSampleBytes() * samplenum]; }
	void Reverse();
	void FillWithZeroSamples(void *buf, int32 numsamples);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Audio);
#endif

protected:
	virtual bool _IsValid();
};

#define AUDIO_BASE (PARAMETER_BASE + TAGOFFSET)

enum AudioAttrTags ENUM_TAGS
{
	PARAM_SamplePos = PARAMETER_BASE | TAG_INT,
};

#endif
