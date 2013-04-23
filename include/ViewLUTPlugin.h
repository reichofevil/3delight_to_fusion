#ifndef _VIEWLUTPLUGIN_H_
#define _VIEWLUTPLUGIN_H_

// baseclass for all plugin view LUTs (CT_ViewLUTPlugin)

#include "Operator.h"

class PreviewInfo;
class Image;
class ViewShader;
class ViewOperator;
class GLTexture;


class FuSYSTEM_API ViewLUTPlugin : public Object
{
	FuDeclareClass(ViewLUTPlugin, Object); // declare ClassID static member, etc.

protected:
	PreviewInfo *m_PrevInfo;
	ScriptVal *m_PrefTable;
	ViewShader *m_ViewShader;
	ViewOperator *m_ViewOp;

	bool m_bLUTChanged;

	uint8 pad[32];

public:
	ViewLUTPlugin(const Registry *reg, const ScriptVal &table, const TagList &tags);
	~ViewLUTPlugin();

	virtual bool Setup(PreviewInfo *pi, const ScriptVal &settings);	// will be called with a valid OpenGL context
	virtual void Close();
	virtual bool Save(ScriptVal &table, const TagList &tags);

	static FusionDoc *GetLUTDoc();

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags);
	virtual bool Edit(HWND parentwnd);								// Pop up your LUT controls

	virtual Image *ApplyToImage(Request *req, Image *img);	// will be called with a valid OpenGL context (img may be NULL)
	virtual ViewShader *SetupShader(Request *req, Image *img);	// return a new ViewShader, with any params (img may be NULL)
	virtual void FreeShader();											// Call this baseclass to destroy m_ViewShader
	virtual bool SetupParams(Request *req, ViewShader *vs, Image *img);	// called to allow you to change any ViewShader params (img may be NULL)

	virtual bool HasNoEffect();										// returns true if LUT will not change the image
	virtual bool IsLUTChanged();										// returns true if LUT has changed since last apply
	virtual void MarkLUTChanged();									// call this to signal that the LUT should be re-applied

	void SetViewOperator(ViewOperator *viewop);
	Operator *GetViewOperator();
	ViewShader *GetViewShader();

	Input *AddInput(const FuID &name, const FuID &id, Tag firsttag, ...);
	Input *AddInputTagList(const FuID &name, const FuID &id, TagList &tags);
	int AddControlPage(const char *name, Tag firsttag = _TAG_DONE, ...);
	int AddControlPageTagList(const char *name, TagList &tags);

	Image *DoApplyToImage(Image *img);
	ViewShader *DoSetupShader(Image *img);
	bool DoSetupParams(Image *img);

	void Redraw();
};

// Load/Setup tags
#define VIEWLUT_BASE		OBJECT_BASE + 100

enum ViewLUTTags ENUM_TAGS
{
	VLUT_ClearAll          = VIEWLUT_BASE + TAG_INT,			// passed to Do() when the image(s) change
	VLUT_ChannelHint,														// passed to SetAttrs(), currently viewed channel

	VLUTS_ImportFile       = VIEWLUT_BASE + TAG_STRING,
};

// Registry tags

enum ViewLUTRegTags ENUM_TAGS
{
	VLUTREG_NoShaders = REG_USER + TAG_INT,						// Use this if you don't override SetupShader() (default false)
};



///////////////////////////////////////////////////
// ViewOperator declaration

class FuSYSTEM_API ViewOperator : public Operator
{
	FuDeclareClassX(ViewOperator, Operator, ViewOperator); // declare ClassID static member, etc.

public:
	FuSafePtr<Request> m_Request;
	ViewLUTPlugin *m_ViewLUT;

public:
	ViewOperator(const Registry *reg, const ScriptVal &table, const TagList &tags);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags);
	virtual void Process(Request *req);
};


#endif
