#ifndef _MTL_INPUT_3D_H_
#define _MTL_INPUT_3D_H_

/** 
 * @file  3D_MtlInput.h 
 * @brief MtlInput3D is a special Input that always outputs a MtlGraph3D object but is capable of accepting both material and image 
 *        connections.  Its purpose is to automagically promote image/mask parameters into materials.
 */

#if 0

#include "Input.h"
#include "Object.h"
#include "Request.h"

class Input;
class Request;
class Operator;
class MtlData3D;
class MtlGraph3D;




const int MTLINPUT3D_BASE = OBJECT_BASE + TAGOFFSET;

enum MtlInput3DTags ENUM_TAGS
{
	MI_AddShowCheckbox = MTLINPUT3D_BASE + TAG_INT,
};


class FuSYSTEM_API MtlInput3D : public Object
{
	FuDeclareClass(MtlInput3D, Object);

public:
	Operator *Owner;
	Input *InMaterial;

protected:
	void MtlInputCtor();

public:
	MtlInput3D();
	MtlInput3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	~MtlInput3D();

	// Pass the tags that you would normally pass to AddInput().  Make sure you have:
	//   OBJP_Owner,						ptr_to_operator_who_is_getting_inputs,		// required
	//   LINKID_DataType,				CLSID_DataType_MtlGraph3D,						// required
	//   LINKID_AllowedDataType,		CLSID_DataType_Image,							// optional
	//   LINK_Main,						some_number,										// required
	bool AddInput(const FuID &name, const FuID &id, Tag firstTag = _TAG_DONE, ...);
	virtual bool AddInputTagList(const FuID &name, const FuID &id, TagList &tags);

	virtual Input *GetInput() { return InMaterial; }
	virtual Number *GetValue(Request *req) { return InMaterial->GetValue(req); }

	virtual void AttachTo(Operator *owner, Input *victim);

	//virtual MtlData3D *GetTexData(Request *req);		// if its an image wrap it in a teximage
	virtual MtlData3D *GetMtlData(Request *req);		// Returned pointer must be Recycled()
	virtual MtlGraph3D *GetMtlGraph(Request *req);	// Returned pointer must be Recycled()

	virtual void ShowInputs(bool visible);
};

#endif

#endif