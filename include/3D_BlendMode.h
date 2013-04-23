#ifndef _3D_BLENDMODE_H_
#define _3D_BLENDMODE_H_

#include "Image.h"
#include "3D_BaseData.h"
#include "3D_BaseInputs.h"

class BlendModeDataSW3D;

const FuID CLSID_BlendModeDataSW3D = "BlendModeDataSW";
const FuID CLSID_BlendModeDataGL3D = "BlendModeDataGL";

const FuID CLSID_BlendModeInputsSW3D = "BlendModeInputsSW";
const FuID CLSID_BlendModeInputsGL3D = "BlendModeInputsGL";

typedef void (*BlendFuncSW3D)(Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);


#define BLEND_MODE_INPUTS_BASE					(INPUTS_BASE + TAGOFFSET)

enum BlendModeInputs3D_Tags ENUM_TAGS
{
	BMI_IntBaseTag = BLEND_MODE_INPUTS_BASE + TAG_INT,
	BMI_AddBlendNest,								// (true) pass to AddInputs() to determine if the inputs get created inside a "Blend Mode" nest or not

	BMI_DblBaseTag = BLEND_MODE_INPUTS_BASE + TAG_DOUBLE,
	BMI_PtrBaseTag = BLEND_MODE_INPUTS_BASE + TAG_PTR,
};





/**
 * Data object for software renderer blend modes.
 */
class FuSYSTEM_API BlendModeDataSW3D : public Data3D
{
	FuDeclareClass(BlendModeDataSW3D, Data3D);

public:
	BlendFuncSW3D BlendFunc;
	float32 Gain;
	float32 AddSub;
	float32 BurnIn;

public:
	BlendModeDataSW3D();
	BlendModeDataSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	BlendModeDataSW3D(const BlendModeDataSW3D &toCopy);
	virtual ~BlendModeDataSW3D();

	static void Additive	    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Subtractive  (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Accumulate   (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void NormalOver0  (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void NormalOver3  (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void NormalIn	    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void NormalHeldOut(Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void NormalAtop   (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void NormalXOr    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void SimpleScreen (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Screen	    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Multiply     (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Overlay	    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void SoftLight    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void HardLight    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void ColorDodge   (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void ColorBurn    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Darken	    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Lighten	    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Difference   (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Exclusion    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Hue		    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Saturation   (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Color		    (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
	static void Luminosity   (Color4f &out, const Color4f &fg, const Color4f &bg, const BlendModeDataSW3D &data);
};






/**
 * Inputs object for software renderer blend modes.
 */
class FuSYSTEM_API BlendModeInputsSW3D : public Inputs3D
{
	FuDeclareClass(BlendModeInputsSW3D, Inputs3D);

public:
	Input *InBlendModeGroup;
	Input *InBlendSWGroup;
	Input *InBlendMode;
	Input *InPorterDuff;
	Input *InAdd;
	Input *InGain;
	Input *InBurnIn;

public:
	BlendModeInputsSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	BlendModeInputsSW3D(const BlendModeInputsSW3D &toCopy);		// ni
	virtual ~BlendModeInputsSW3D();

	static bool InjectInputs(Inputs3D *victim, TagList *tags);

	virtual bool AddInputsTagList(TagList &tags);
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
	virtual void ShowInputs(bool visible);
};




/**
 * Data object for OpenGL renderer blend modes.
 */
class FuSYSTEM_API BlendModeDataGL3D : public Data3D
{
	FuDeclareClass(BlendModeDataGL3D, Data3D);

public:
	GLenum SrcFactor[2], DstFactor[2];		// src/front is blended over dst/back, first component is RGB and second is A

public:
	BlendModeDataGL3D();
	BlendModeDataGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	BlendModeDataGL3D(const BlendModeDataGL3D &toCopy);
	virtual ~BlendModeDataGL3D();
};




/**
 * Inputs object for OpenGL renderer blend modes.
 */
class FuSYSTEM_API BlendModeInputsGL3D : public Inputs3D
{
	FuDeclareClass(BlendModeInputsGL3D, Inputs3D);

public:
	Input *InBlendModeGroup;
	Input *InBlendGLGroup;
	Input *InBlendMode;

public:
	BlendModeInputsGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	BlendModeInputsGL3D(const BlendModeInputsGL3D &toCopy);		// ni
	virtual ~BlendModeInputsGL3D();

	static bool InjectInputs(Inputs3D *victim, TagList *tags);

	virtual bool AddInputsTagList(TagList &tags);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
	virtual void ShowInputs(bool visible);
};





extern FuSYSTEM_API BlendModeDataGL3D DefaultBlendModeDataGL;
extern FuSYSTEM_API BlendModeDataSW3D DefaultBlendModeDataSW;




#endif