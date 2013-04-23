#ifndef _PREVIEWCONTROL_H_
#define _PREVIEWCONTROL_H_

#include "TagList.h"
#include "Input.h"
#include "RegistryLists.h"
#include "StandardControls.h"
#include "Pixel.h"
#include "ContextMenu.h"
#include "VectorMath.h"
#ifndef DFLIB
#include <GL/gl.h>
#include <GL/glu.h>
#endif


class FusionDoc;
class PreviewControl;
class ControlWnd;
class Wintab;						// Wintab tablet object
class Operator;
class CChildFrame;
class Matrix4;
class FuView;


enum DragType
{
	DR_None = 0,
	DR_Following,
	DR_Dragging,
};

extern FuSYSTEM_API const FuID CCMD_Delete;				// data = NULL, return TRUE if accepted
extern FuSYSTEM_API const FuID CCMD_Select;				// data = PRect *
extern FuSYSTEM_API const FuID CCMD_KeyDown;				// data = char, return TRUE if key accepted
extern FuSYSTEM_API const FuID CCMD_KeyUp;				// data = char, return TRUE if key accepted
extern FuSYSTEM_API const FuID CCMD_Init;
extern FuSYSTEM_API const FuID CCMD_Cleanup;
extern FuSYSTEM_API const FuID CCMD_GetInputAt;			// return Input *
extern FuSYSTEM_API const FuID CCMD_Copy;					// data = bool, return TRUE if accepted
extern FuSYSTEM_API const FuID CCMD_Cut;					// data = bool, return TRUE if accepted
extern FuSYSTEM_API const FuID CCMD_Paste;				// data = bool, return TRUE if accepted
extern FuSYSTEM_API const FuID CCMD_HitTest;				// data = uint32 (flags defined below).
extern FuSYSTEM_API const FuID CCMD_FollowCursor;		// data = EraseType, 0 = draw new only, 1 = erase old & draw new, 2 = erase old only
extern FuSYSTEM_API const FuID CCMD_KeyDownAll;			// data = char, return true if key accepted (may still get CCMD_KeyDown)
extern FuSYSTEM_API const FuID CCMD_KeyUpAll;			// data = char, return true if key accepted
extern FuSYSTEM_API const FuID CCMD_TimeChanged;		// data = TimeStamp *, set to 'pHint'

// Flags for the uint32 * sent to CCMD_HitTest
// When CCMD_HitTest is called if !CHT_Hit then or in CHT_Hit iff mouse pos is over control. Set some state
// information to draw hover states during the following DrawControls. If CHT_Hit was already set then clear
// state information for following DrawControls. CHT_Hit implies CH_Changed. CHT_Changed can be set to force
// DrawControls, for example when the control is no longer being hovered over.
// Only if CHT_Hit was set should the hover state be drawn in next DrawControls. All other times draw normally.
#define CHT_Hit				0x00000001					// A control was hit/is being hovered over
#define CHT_Changed			0x00000002					// Call DrawControls

typedef uint32 CtrlCmd;

struct PRect
{
	float64 x1, y1, z1;
	float64 x2, y2, z2;
};

class FuSYSTEM_API PreviewInfo
{
public:
	Operator *RenderOp;

	// Image stuff.							// (all coords in pixels)
	int32 Width, Height;						// Scaled Image size
	int32 RealWidth, RealHeight;			// Source Image size
	int32 OriginalWidth, OriginalHeight;// Source image size before proxy

	// Preview stuff.
	int32 WindowWidth, WindowHeight;		// Physical window size
	int32 WindowXOffset, WindowYOffset;	// Physical window offset
	int32 RectXOffset, RectYOffset;		// Image rect location within window
	int32 ScreenXOffset, ScreenYOffset;	// Window offset in screen coords

	float64 XScale, YScale;					// Normal image scale (aspect, not viewed zoom)
	float64 DispXScale, DispYScale;		// Displayed image scale (aspect, not viewed zoom)
	float64 XOffset, YOffset, ZOffset;	// image/scene-space offsets
	float64 XPos, YPos;						// cursor pos
	float64 ZPos;								// transformed cursor zpos (needed for 3D tools)

	float64 SourceWidth, SourceHeight;	// contains dimensions from the control's source tool
	float64 SourceXScale, SourceYScale;	// contains pixel aspect from the control's source tool

	float64 ViewAspect, SourceAspect;	// contains the frame aspect of the image being viewed, and the image from the tool the previewcontrol is on

	Pixel Pix;
	FltPixel FltPix;

	HPEN Pen, PenHover;
	COLORREF Color, ColorHover;
	COLORREF ControlOpacity;
	HCURSOR Cursor;

	bool DoubleClick;
	bool DrawingOutline;
	bool ActiveControl;
	bool Drawing;

	PCCoordSpace m_CoordSpace;

	uint32 Time;								// System time (in ms) at last msg
	Wintab *WintabMgr;						// Wintab tablet object (NULL if none). Get extended tablet data via this.

	PreviewControl *DragPC;

	HRGN Region; //TODO: Remove. Replace with bbox?
	ThreadSafePtr<Matrix4> ViewTransform, ViewInverse;
	ThreadSafePtr<Matrix4> ControlTransform, ControlInverse;
	ThreadSafePtr<Matrix4> ControlRotation, ControlRotationInverse;

	PCCoordSpace m_ViewCoordSpace;

	bool RemoteDrag;

public:
	PreviewInfo();
	PreviewInfo(PreviewInfo &pi);
	virtual ~PreviewInfo();

	void TransformIn(float64 &x, float64 &y, float64 &z);	// uses ControlInverse
	void TransformIn(float64 &x, float64 &y);
	void TransformIn(Vector4 &vec);
	void TransformIn(FuRect &rect);
//	void TransformIn(Vector3f &pt);

	float64 TransformXIn(float64 x);								// legacy, for PreviewView
	float64 TransformYIn(float64 y);
	float64 AspectScaleXIn(float64 x);
	float64 AspectScaleYIn(float64 y);

	void TransformOut(float64 &x, float64 &y, float64 &z);// uses ControlTransform
	void TransformOut(float64 &x, float64 &y);				// uses ControlTransform
	void TransformOut(Vector4 &vec);
	void TransformOut(FuRect &rect);

	float64 TransformXOut(float64 x);
	float64 TransformYOut(float64 y);
	float64 AspectScaleXOut(float64 x);
	float64 AspectScaleYOut(float64 y);

	// Overridables
	virtual bool AddControl(PreviewControl *pc, Operator *op = NULL, int32 pri = -100) = 0;
	virtual bool RemoveControl(PreviewControl *pc, Operator *op = NULL) = 0;
	virtual bool SetPriority(PreviewControl *pc, int32 pri);
	virtual void Lock();
	virtual void Unlock();
	virtual void ReadLock();
	virtual void ReadUnlock();
	virtual PreviewControl *GetNextPC(void *&pos) = 0;
	virtual void Redraw(bool immediate = false);
	virtual CChildFrame *GetParentFrame();		// Deprecated, use GetView()->m_pParentFrame
	virtual FuView *GetView();

#ifndef DFLIB
	void Color4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
	void Color4ubv(const GLubyte *v);
	void Color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
#endif
};

#ifndef ALLOW_GL_COLOR_FUNCTIONS
	#define glColor3b __error_glColor3b_not_allowed__
	#define glColor3d __error_glColor3d_not_allowed__
	#define glColor3f __error_glColor3f_not_allowed__
	#define glColor3i __error_glColor3i_not_allowed__
	#define glColor3s __error_glColor3s_not_allowed__
	#define glColor4b __error_glColor4b_not_allowed__
	#define glColor4d __error_glColor4d_not_allowed__
	#define glColor4f __error_glColor4f_not_allowed__
	#define glColor4i __error_glColor4i_not_allowed__
	#define glColor4s __error_glColor4s_not_allowed__
	#define glColor3ub __error_glColor3ub_not_allowed__
	#define glColor3ui __error_glColor3ui_not_allowed__
	#define glColor3us __error_glColor3us_not_allowed__
	#define glColor4ub __error_glColor4ub_not_allowed__
	#define glColor4ui __error_glColor4ui_not_allowed__
	#define glColor4us __error_glColor4us_not_allowed__
	#define glColor3bv __error_glColor3bv_not_allowed__
	#define glColor3dv __error_glColor3dv_not_allowed__
	#define glColor3fv __error_glColor3fv_not_allowed__
	#define glColor3iv __error_glColor3iv_not_allowed__
	#define glColor3sv __error_glColor3sv_not_allowed__
	#define glColor4bv __error_glColor4bv_not_allowed__
	#define glColor4dv __error_glColor4dv_not_allowed__
	#define glColor4fv __error_glColor4fv_not_allowed__
	#define glColor4iv __error_glColor4iv_not_allowed__
	#define glColor4sv __error_glColor4sv_not_allowed__
	#define glColor3ubv __error_glColor3ubv_not_allowed__
	#define glColor3uiv __error_glColor3uiv_not_allowed__
	#define glColor3usv __error_glColor3usv_not_allowed__
	#define glColor4 ubv __error_glColor4ubv_not_allowed__
	#define glColor4uiv __error_glColor4uiv_not_allowed__
	#define glColor4usv __error_glColor4usv_not_allowed__
#endif


class FuSYSTEM_API PreviewControl : public Object
{
	FuDeclareClass(PreviewControl, Object); // declare ClassID static member, etc.

public:
	char *Name;
	DragType Dragging;
	int32 Active;
	Operator *op; // erk!

	List InputList;

	uint32 ControlGroup;
	FusionDoc *Document;
	PreviewInfo *View;

	bool UndoSet;
	bool Visible;
	bool WantsMouseMoves;
	bool _Pad;

	PCCoordSpace m_CoordSpace;
	
	Vector4 DisplayOffset;
	Vector4 DisplayOffsetCS[10];

public:
	PreviewControl(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~PreviewControl();
	virtual void SetView(PreviewInfo *pi, TagList *tags = NULL);
	
	DragType IsDragging() { return Dragging; }
	int32 GetActive() { return Active; }
	virtual Operator *GetOperator(TagList *tags = NULL);

	virtual bool SetAttrsTagList(Input *inp, const TagList&  taglist);	// special PreviewControl version
	virtual bool SetAttrsTagList(const TagList&  taglist);					// normal Object version
	// TODO: CopyAttr...
	
	virtual bool AddInput(Input *in, TagList *tags = NULL);
	virtual bool RemoveInput(Input *in, TagList *tags = NULL);
	virtual void SetInputs(Parameter *param, TimeStamp time, uint32 id, TagList *tags = NULL);

	virtual void NotifyChanged(Input *input, Parameter *param, TagList *tags = NULL);
	virtual void UpdateDisplay(Input *input, TagList *tags = NULL);
	
	virtual void OnConnect(Input *inp, TagList *tags = NULL);
	virtual void OnDisconnect(Input *inp, TagList *tags = NULL);

	virtual void SetActive(int32 val = 2, TagList *tags = NULL);
	virtual void SetInactive(TagList *tags = NULL);

	virtual void GLDrawControls(PreviewInfo *pi, TagList *tags = NULL);
	virtual DragType ControlDown(PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual void ControlMove(PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual void ControlUp  (PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual uint_ptr ControlCommand(PreviewInfo *pi, const FuID &id, uint_ptr data, TagList *tags = NULL);
	virtual bool ControlGetTip(PreviewInfo *pi, char *buf, TagList *tags = NULL);

	virtual void ResetUndo(TagList *tags = NULL);
	virtual void SetUndo(uint32 id, TagList *tags = NULL);

	virtual void AddToMenu(Menu *menu, TagList &tags);
//	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);

	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual void GetName(char *buffer, int buflen, TagList *tags = NULL) const;									// get the control's name (overridden by PCS_Name)
	virtual void GetFullName(char *buffer, int buflen, Input *in = NULL, TagList *tags = NULL) const;	// name in "op: name" format, uses input's name if passed
	
	bool GetDisabledTags(const TagList &tags, bool def);
};

enum PreviewControlReg_Tags
{
	PCREG_CustomOutline = TAG_INT + REG_USER,   // Default: FALSE
	PCREG_3DSpaceControl,                       // "false" (default) gives GL previewcontrols 2D window coords, "true" gives native 3D coords
};

#define PREVIEWCONTROLFUNC_BASE (OBJECT_BASE + TAGOFFSET)

// For tags passed to various ControlXXX things.
enum PreviewControlFunc_Tags
{
	PCC_RepeatCount = TAG_INT | PREVIEWCONTROLFUNC_BASE,  // For key repeat
	PCC_MouseDown,                                        // Mouse state for control move (default TRUE!)
};

#ifndef DFLIB
inline void GLRhombus(Vector2f *pt, GLfloat width)
{
//	width = 1.41 * width / 2; 
	width /= 2.0;
	glBegin(GL_LINE_LOOP);
		glVertex2f(pt->X - width, pt->Y);
		glVertex2f(pt->X, pt->Y - width);
		glVertex2f(pt->X + width, pt->Y);
		glVertex2f(pt->X, pt->Y + width);
	glEnd();
}

inline void GLRhombus(Vector3f *pt, GLfloat width) { GLRhombus((Vector2f *)pt, width); }

inline void GLPolyline(Vector2f *pt, int n, int step = 1)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, step==1? 0 : sizeof(Vector2f)*step, pt);
	glDrawArrays(GL_LINE_STRIP, 0, n);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// this is for drawing rectangle within already existing glStart(GL_LINES) ... glEnd()
inline void GLRect(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glVertex2f(x1, y2);
	glVertex2f(x1, y1);
}
#endif

#endif
