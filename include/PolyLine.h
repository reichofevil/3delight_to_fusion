#ifndef _POLYLINE_H_
#define _POLYLINE_H_

#include "Parameter.h"
#include "Input.h"
#include "PolyPoint.h"
#include "VectorMath.h"

#define POLYLINE_ID zMAKEID('PlLn')
//#define CLSID_DataType_Polyline POLYLINE_ID

#define PLL_XWISE				(1l << 0)
#define PLL_YWISE				(1l << 1)
#define PLL_ROTATE				(1l << 2)
#define PLL_EXPAND				(1l << 3)

#define PLL_LHANDLE				(1l << 3)

#define PLLF_LOCK_LDISP			(1l << 0)
#define PLLF_BPOLY_CLAMPED		(1l << 1)
#define PLLF_TB_SPLINE			(1l << 2)
#define PLLF_BETA_SPLINE		(1l << 3)

enum PLL_DrawStyleIDs
{
	PLL_DS_DASH	= 1,
	PLL_DS_DOT,
};

enum PLL_SpecialAttrs
{
	PLLHint_Add = TAG_INT + 1100,					// index
	PLLHint_Add_to_Hull,							// index
	PLLHint_ChangeShape,								// mask???
	PLLHint_Close,										// NULL
	PLLHint_Open,										// index
	PLLHint_DragKey,									// index
	PLLHint_DragLControl,							// index
	PLLHint_DragRControl,							// index
	PLLHint_DragInAll,								// index
	PLLHint_ApplyHandleChanges,						// true or false
	PLLHint_Paste,										// index
	PLLHint_PasteCT,									// index
	PLLHint_PastePoly,								// index (pasting entire poly)
	PLLHint_PassThroughPath,						// NULL
	PLLHint_PassThroughNPath,						// NULL
	PLLHint_ResetAttrs,								// NULL
	PLLHint_DragPublished,							// index
	PLLHint_UpdatePointAttribs,						// attrib
	PLLHint_NUpdatePublishedPoints,				// index
	PLLHint_RestoreState,							// index
	PLLHint_PublishedInterpolated,				// index
	PLLHint_ClickAdd,									// index
	PLLHint_OtherInputChanged,						// index
	PLLHint_ReducePts,								// index
	PLLHint_MakeDoublePolyLine,						// TRUE/FALSE
	PLLHint_ConvertSpline,							// ID of the type of conversion
	PLLHint_MakePoliesEqual,						// 0
	PLLHint_NullPolyLine,							// 0
	PLLHint_StartOperation,							// 0
	PLLHint_NotifyPreviewControlsOnly,				// 0
	PLLHint_DisabledPolyLine,						// 0 (1 : disable; 0 : enable)
	PLLHint_SetKeyTime,								// 0
	PLLHint_EditType,								// id of editing 
	PLLHint_EaseInOut,								// 1 == ease in, 2 == ease out, 0 or 3 == ease both
	PLLHint_RefineKnotVector,						// subdivision
	PLLHint_DoNotSmooth,							// 0
	PLLHint_None,									// 0

	PLLHint_Parameter = TAG_DOUBLE,					// u param

	PLLHint_Delete = TAG_PTR,						// mask
	PLLHint_Smooth,									// mask
	PLLHint_Linear,									// mask
	PLLHint_Import,									// time list
	PLLHint_ImportMultipleObjects,					// linked list of objects
	PLLHint_Lock,									// index
	PLLHint_SetFollowingPoints,						// mask
	PLLHint_PublishPoints,							// ptr to TransformInfo obj
	PLLHint_PublishToPath,							// ptr to TransformInfo obj
	PLLHint_PublishToXYPath,						// ptr to TransformInfo obj
	PLLHint_CreatePoints,							// PtPath list (create new pts from scratch)
	PLLHint_UnpublishPoints,						// ptr to TransformInfo obj
	PLLHint_AddPoints,								// mask (add to existing pts)
	PLLHint_AddPointsOverride,						// mask (override existing points and add the rest)
	PLLHint_MatchSplines,							// mask (indexes of selected points)
	PLLHint_ImportSpline,							// pointer to the first point of the original spline
	PLLHint_SelectPoints,							// list
	PLLHint_MakeEllipse,							// NULL
	PLLHint_MakeRectangle,							// NULL
	PLLHint_MakeSpiral,								// NULL
	PLLHint_LockPairs,								// NULL
	PLLHint_MultiFrameShaping,						// ptr to a ChangePolyLineInfo obj
	PLLHint_MultiFrameMovePoints,					// ptr to a ChangePolyLineInfo obj
	PLLHint_Tensioning,								// ptr to a ChangePolyLineInfo obj
	PLLHint_MultiFrameTensioning,					// ptr to a ChangePolyLineInfo obj
	PLLHint_MultiFrameEase,							// ptr to a ChangePolyLineInfo obj
	PLLHint_MultiFrameSmoothingPoints,				// ptr to a SmoothPointFilterInfo obj
	PLLHint_MultiFrameSmooth,						// ptr to ChangePolyLineInfo obj
	PLLHint_MultiFrameLinear,						// ptr to ChangePolyLineInfo obj
	PLLHint_MoveCenter,								// ptr to a TransformInfo obj
	PLLHint_CheckExternFollowingPoints,				// NULL
	PLLHint_Continuity,								// mask

	PLLHint_DeleteKeyPoints,						// ptr to an array of key times to be deleted, terminated by TIME_UNDEFINED
	PLLHint_SmoothKeyPoints,						// ptr to an array of key times to be smoothed, terminated by TIME_UNDEFINED
	PLLHint_LinearKeyPoints,						// ptr to an array of key times to be linearised, terminated by TIME_UNDEFINED
	PLLHint_TensioningKeyPoints,					// ptr to an array of key times to be tensioned, terminated by TIME_UNDEFINED
	PLLHint_EaseKeyPoints,							// ptr to an array of key times to be eased, terminated by TIME_UNDEFINED
	PLLHint_DragKeyPoints,							// ptr to an array of key points (Vector2s), terminated by one withTIME_UNDEFINED
	PLLHint_ImportKeyPoints,						// ptr to an array of key points (Vector2s), terminated by one withTIME_UNDEFINED
	PLLHint_ShapingKeyPoints,						// ptr to an array of key points (Vector2s), terminated by one withTIME_UNDEFINED

	PLLHint_TransformMatrix,						// ptr to a transform matrix
};

#define DM_MASK 0xf

enum DrawModes
{
	DM_NONE = 0,
	DM_CLICK,
	DM_FREEHAND,
	DM_INSERTMODIFY,
	DM_MODIFYONLY,
};

enum DrawPolyLineIDs
{
	PLL_DRAW_KEYPTS = 1,
	PLL_DRAW_HANDLES,
};

enum PolyLineConversionIDs
{
	PLL_CONV_BEZIER_TO_BSPLINE,
	PLL_CONV_BEZIER_TO_CARDINAL,
	PLL_CONV_BEZIER_TO_UBSPLINE,
	PLL_CONV_BSPLINE_TO_BEZIER,
	PLL_CONV_UBSPLINE_TO_BEZIER,
	PLL_CONV_CARDINAL_TO_BEZIER,
};

enum AddPointIDs
{
	ADD_RECALC_XY = 1,
	ADD_LINEAR_PT,
	ADD_TO_HULL		= (1l << 2),
};

class Operator;
class Point;
class Input;

struct TransformInfo
{
	uint8 *mask;
	double CenterX;
	double CenterY;
	double CenterZ;
	double Size;
	double AngleX; // in deg
	double AngleY; // in deg
	double AngleZ; // in deg
	double Aspect;
	Vector3 Change;
	LPVOID lParam; // can be Operator or Input

	TransformInfo(double cx, double cy, double cz, double sz, double angx, double angy, double angz, double asp)
	{
		CenterX = cx; CenterY = cy; CenterZ = cz; Size = sz; AngleX = angx; AngleY = angy; AngleZ = angz; Aspect = asp;
		lParam = NULL; mask = NULL;
		Change.Set(0.0,0.0,0.0);
	}
	TransformInfo(double cx, double cy, double cz, double sz, double angx, double angy, double angz, double asp, uint8 *m, LPVOID par)
	{
		CenterX = cx; CenterY = cy; CenterZ = cz; Size = sz; AngleX = angx; AngleY = angy; AngleZ = angz; Aspect = asp;
		mask = m; lParam = par;
		Change.Set(0.0,0.0,0.0);
	}
};

struct ChangePolyLineInfo
{
	uint8 operation;
	Vector3 value;
	Vector3 value_org;
	uint8 *pts_mask;
	double Aspect;
	short sign;
	LPVOID lParam; // array of handles and point changes
	ChangePolyLineInfo(uint8 oper, Vector3 &val, Vector3 &val_org, uint8 *pmask, double asp, short sgn)
	{
		operation = oper;
		value = val;
		value_org = val_org;
		pts_mask = pmask;
		Aspect = asp;
		sign = sgn;
		lParam = NULL;
		mt = NULL;
	}
	Matrix4 *mt;
};

struct SmoothPointFilterInfo
{
	int nleft;
	int nright;
	short order;
	float sampl_spac;
	uint8 *pts_mask;
	SmoothPointFilterInfo(){ nleft = 5; nright = 5; order = 4; sampl_spac = 1.0; pts_mask = NULL; }
	SmoothPointFilterInfo(int nl, int nr, short ord, float s_spac, uint8 *ptsm){ nleft = nl; nright = nr; order = ord; sampl_spac = s_spac; pts_mask = ptsm; }
};

class FuSYSTEM_API PolyLine : public Parameter
{
	FuDeclareClass(PolyLine, Parameter); // declare ClassID static member, etc.

protected:
	int Count;
	PolyPoint *tail;
	uint32 ParFlag;
	bool m_Closed;
	int32 Width, Height;
	short m_Order;

//	void InterpolateData(PolyPoint *hdata, int count, bool line);

public:
	uint32 mFlag;
	double PhysicalLength;
	PolyPoint *FirstKP;

	PolyLine();
	PolyLine(const Registry *reg, const ScriptVal &table, const TagList &tags);
	PolyLine(PolyLine &poly);
	PolyLine(PolyPoint *ph);

	static const PolyLine &Get(Input *in, Request *req) { return *(PolyLine *) in->GetValue(req); }

	bool operator == (PolyLine &pc);
	virtual bool Save(ScriptVal &table, const TagList &tags);
	virtual ~PolyLine();
	virtual Parameter *Copy();
	virtual Parameter *InterpolateWith(double weight, Parameter *rightvalue, TagList *tags = NULL);
	virtual bool IsFromSplineList(){ return TRUE; }
	virtual bool UpdatePublish(void *data, Parameter *param, TagList *tags = NULL);

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual PolyPoint *AddPoint(Vector3 &pc, double asp, bool atEnd);
	virtual void AddPoint( double lp_l, double da_y_x, double da_z_xy, double asp, bool atEnd );
	virtual PolyPoint *InsertPoint(PolyPoint *sp, Vector3 &add_at, BYTE add_flag = ADD_RECALC_XY);
	virtual PolyPoint *InsertPoint(int index, double dist );

	PolyPoint *AddPointAtDisp( double disp );
	virtual double CalcClosestPoint( Vector3 &to, PolyPoint **prev, PolyPoint *ph, Vector3 &cp, double size);
	virtual double CalcLength(double width_height = -1.0, double *lopen=NULL);
	bool CalcSegmentPar( KeyPoint *pr, double &lp_l, double &da_y_x, double &da_z_xy, bool atEnd );
	virtual Vector3 CalcXYZ(PolyPoint *sp, double dist);
	virtual Vector3 CalcXYZ( PolyPoint *sp, double disp, double *pa, double *pa_zx, double width_height, bool high_precision = TRUE);
	virtual bool Close(double asp);
	void Copy(PolyLine &poly);
	void CopyRecord(PolyLine &poly);
	virtual void DeleteAllPoints();
	virtual bool DeletePoints( uint8 *mask, bool rscl=TRUE );
	virtual double Distance( PolyPoint *sp, PolyPoint *ep, Vector3 &to );
	bool EqualToBy(PolyLine *pc, uint32 flag);
	KeyPoint *FindMatchPoint(KeyPoint *pc, KeyPoint *src );
	KeyPoint *FindExternFollowingPoint(KeyPoint *src, PolyLine *poly2);
	PolyPoint *First( uint64 attrib=0 );
	double GetAspect();
	int GetIndex( KeyPoint *pc );
	int GetCount(uint64 attrib = 0);
	PolyPoint *GetPoint(int index );
	virtual Vector3 GetPointOnPath(double normalized_distance, KeyPoint **sp, double *a, double *pa_zx, double width_height = -1.0);
	int GetPtPos(KeyPoint *sp, Vector3 &at, double &dist );
	bool IsAllSelected();
	bool IsAspValid(){ return (Width>0 && Height>0? TRUE : FALSE); }
	bool IsClosed(){ return (Count<2? FALSE : m_Closed); }
	bool IsCutCondition();
	PolyPoint *Last( uint64 attrib=0 );
	virtual void Linear( KeyPoint *pl, uint8 *mask );
	void LinearizePoint( PolyPoint *pl );
	virtual void MovePointProportionally( double lp_l, double da_y_x, double da_z_xy, double asp, bool atEnd );
	virtual void Open(int ipo);
	void PastePoints(PolyLine *src, uint8 *mask);
	void ResetAttrib( uint64 attrib );
	void SetClosed(bool close){ m_Closed = close; }
	void SetColinearHandler( PtInfo *pi, Vector3 &dxyz , double asp );
	void SetCount(){ Count = (FirstKP? FirstKP->GetCount() : 0); }
	bool SetHandlerLinear( PolyPoint *kp, uint8 fl );
	void SetHandlerSmooth( PolyPoint *prev, Vector3 &at, PolyPoint *ph, double asp );
	void SetLinearSegment( PolyPoint *ps );
	void SetPrevPtrs();
	void SetWidthHeight(int32 w, int32 h){ Width = w; Height = h; }
	void Shaping(Vector3 &cp, Vector3 &cp_org, PolyLine *porg, uint8 how, uint8 *mask, double asp);
	virtual void Smooth( KeyPoint *ps, double asp, uint8 *mask, uint16 flag );
	void SmoothPoint( PolyPoint *ps, double asp, uint8 flag );
	int32 TransformDXOut(double dx){ return int32(dx * Width); }
	int32 TransformDYOut(double dy){ return int32(dy * Height); }
	void UpdateShape();
	uint8 UpdatePublishedPoints(Point *p, PolyLine *following, TransformInfo &ti, FuID inpID, TimeStamp time, bool animate);
//	void ValidateLocks();
	virtual PolyPoint *MovePoints(Vector3 &dxyz, PtInfo *pmi, uint8 *mask = NULL);
	uint32 SetFollowing(uint8 *mask, double asp, bool set, bool test);
	PolyPoint *GetPublishedPoint(int *ip = NULL);
	bool CheckFollowingPoints();
	void SetFollowingPointsOrg(double asp);
	PolyPoint *NextPt(PolyPoint *p);
	PolyPoint *PrevPt(PolyPoint *p);
	void TransformToAbsolute(TransformInfo &ti);
//	void TransformToRelative(double cx, double cy, double size, double angle, double asp);
//	void OnOtherInputsChanged(double angle, double aspect, double centerX, double centerY, double size, Operator *op, double delta_angle, Input *polyline_input, TimeStamp time);
	void OnPublishedChanged(TransformInfo *ti, PolyPoint *po, TimeStamp time);
	bool PublishPoints(TransformInfo *ti);
	bool UnpublishPoints(Operator *op, uint8 *mask);
	void UpdateAllPublishedPoints(TransformInfo &ti, Operator *owner, Input *polyline_input, Request *req, PolyLine *following, Input *polyline_input_following);
	double RecalcDisplacements(double disp_first = 0.0, double disp_last = 1.0);
	bool CheckLoop(double asp, bool force);
	PolyPoint *GetSelectedPublishedPoint(uint8 *mask, int *ip);
	void UpdatePublishedPointInputs(TransformInfo &ti, Operator *op, uint8 *mask);
	void OnOtherInputsChanged(PolyLine *org, TransformInfo *tin, TransformInfo *tio, Operator *op, Input *polyline_input, TimeStamp time, PolyLine *following, Input *polyline_input_following);

	virtual void MakeEllipse(Vector3 &Center, double width, double height, double asp, bool cw = TRUE);
	virtual void MakeRectangle(Vector3 &Center, double width, double height, double asp, bool cw = TRUE);
	virtual void MakeSpiral(Vector3 &Center, double radius_start, double radius_end, int turns, double length, double asp, bool cw);
	virtual Vector3 *GetDigitalPoints(int &count);
	short GetOrder(){ return m_Order; }
	virtual void SetReducedPoints(Vector3 *points, int count, BYTE *rem_pts_mask);
	virtual Vector3 ValueAt(double u, short derive, PolyPoint *ps, PolyPoint *pe);
	Vector3 CalcSlope( PolyPoint *ps, PolyPoint *pe, Vector3 &at);
	virtual double CalcClosestPar(PolyPoint *sp, PolyPoint *ep, Vector3 &to);
	virtual int GetSamplePoints(int count, double aspect, class Vector2 *points, int *seg_samp_count, int seg_count);
	virtual int GetSegmentsCount(uint64 attrib);
	virtual int GetSamplePoints(int subdiv, PolyPoint **hp){ return 0; }
	void Tensioning(double rel_tens, BYTE *mask, PolyLine *porg);
	void Expand(double d, double asp, short &sign);
	virtual void Convert(PolyLine *poly, uint8 *mask);
	virtual void SetDiscontinualSegementMask(uint8 *dmask);

//	PolyLine(class BSplinePolyLine &poly);
//	PolyLine(class CardinalSplinePolyLine &cpoly);
	double (*pLenFunc)(PolyPoint *, PolyPoint *, double , double);
	double Length(PolyPoint *ps, PolyPoint *pe, double asp = 1.0);
	bool AddPublishedPointInputs(TransformInfo *ti);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(PolyLine);
#endif

protected:
	virtual bool _IsValid();
};

class AutoPolyLine : public AutoParameter
{
	FuDeclareClass(AutoPolyLine, AutoParameter); // declare ClassID static member, etc.

public:
	AutoPolyLine()	{ Param = new PolyLine; }
	AutoPolyLine(Input *in, TimeStamp time) : AutoParameter(in, time) {}
	operator PolyLine *()					{ return (PolyLine *)Param; }
};

// asp == height/width
inline double BezierLengthFunc(PolyPoint *ps, PolyPoint *pe, double u, double asp)
{
	double dx = (9.0 * (ps->X+ps->RightCX) - 9.0 * (pe->X+pe->LeftCX) - 3.0 * ps->X + 3.0 * pe->X)*pow2(u) + (6.0*ps->X + 6.0*(pe->X+pe->LeftCX) - 12.0 * (ps->X+ps->RightCX))*u + 3.0 * (ps->X+ps->RightCX) - 3.0 * ps->X;
	double dy = (9.0 * (ps->Y+ps->RightCY) - 9.0 * (pe->Y+pe->LeftCY) - 3.0 * ps->Y + 3.0 * pe->Y)*pow2(u) + (6.0*ps->Y + 6.0*(pe->Y+pe->LeftCY) - 12.0 * (ps->Y+ps->RightCY))*u + 3.0 * (ps->Y+ps->RightCY) - 3.0 * ps->Y;
	double dz = (9.0 * (ps->Z+ps->RightCZ) - 9.0 * (pe->Z+pe->LeftCZ) - 3.0 * ps->Z + 3.0 * pe->Z)*pow2(u) + (6.0*ps->Z + 6.0*(pe->Z+pe->LeftCZ) - 12.0 * (ps->Z+ps->RightCZ))*u + 3.0 * (ps->Z+ps->RightCZ) - 3.0 * ps->Z;
	dy *= asp;
	return sqrt(pow2(dx) + pow2(dy) + pow2(dz));
}

DLLExtern void UpdatePublishedPointsInputsAttrs(Operator *op);
DLLExtern bool UpdatePublishedPointsInputsAttrs(PolyLine *poly, Operator *op);
DLLExtern void SmoothFast( KeyPoint *ph, KeyPoint *pt, double asp, uint8 *mask, uint16 flag );
DLLExtern void SmoothFast2( KeyPoint *pf, KeyPoint *pl, double asp, uint8 *mask, uint16 flag );
DLLExtern uint8 FollowPolyLine(PolyLine *poly, PolyLine *old_poly, PolyLine *following, Operator *op, TransformInfo &ti, uint8 *mask=NULL, uint8 *mask2=NULL);
DLLExtern void SetMatchingPolyLinePointMask(PolyLine *poly, uint8 *mask, PolyLine *poly2, uint8 *mask2);
DLLExtern bool CheckExternFollowingPoints(PolyLine *poly, PolyLine *following, bool follow, double aspect);
DLLExtern bool FollowExternPoints(PolyLine *poly, KeyPoint *pub, PolyLine *following, double aspect);
DLLExtern bool UpdateLinearPublishedSegments(PolyLine *poly, PolyPoint *pp, double angx, double angy, double angz, double asp);
DLLExtern void FEase(PolyLine *poly, int ease, int ease_org, uint8 io, BYTE *mask, PolyLine *porg);
DLLExtern bool SmoothPoints(PolyLine *poly, PolyLine *poly_org, uint8 *pts_sel, int nleft, int nright, short order, float sampl_spac);
bool HitSegmentTest(double x, double y, PolyPoint *p1, PolyPoint *p2, int width, int height, double size, double grab);
void SetFollowingPointsInAll(PolyLine *src, KeyPoint *dst, double asp, uint8 *mask);
void UpadteFollowParametersInAll(PolyLine *src, KeyPoint *dst);
double CalcPolylineLength( KeyPoint *pf, KeyPoint *pl, double asp );
void SmoothSegment( PolyPoint *ps, PolyPoint *pe, double asp, uint8 sflg );
#endif
