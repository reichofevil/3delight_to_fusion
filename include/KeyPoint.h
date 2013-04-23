#ifndef __KEYPOINT_H__
#define __KEYPOINT_H__

#include "types.h"
#include "Parameter.h"

/*------------------------------------------------------------------
 * KEYPOINT.H : MODULE FOR DECLERATION OF KEYPOINT STRUCTURE
 *
 * Written by  Srecko ZRILIC
 *------------------------------------------------------------------
 * HISTORY :
 * -------
 *
 * 09-FEB-96 SR  Created file
 *------------------------------------------------------------------
 */

#define XY_UNDEFINED		-1000000000.0

#define KP_SELECTED				(1l << 0)
#define KP_LINEAR				(1l << 1)
#define KP_LOCKX				(1l << 2)
#define KP_LOCKY				(1l << 3)
#define PKP_INITIAL				(1l << 4)
#define KP_SHAPE				(1l << 5)
#define KP_COPYKS				(1l << 6)
#define KP_SELECTEDEX			(1l << 7)
#define KP_STEP_IN				(1l << 8)
#define KP_STEP_OUT				(1l << 9)
#define KP_EXCLUDE				(1l <<10)
#define KP_HIGHLIGHT			(1l <<11)
#define KP_LOOP					(1l <<12)
#define KP_PICK					(1l <<13)
#define KP_FOLLOWING			(1l <<14)
#define KP_PUBLISHED			(1l <<15)
#define KP_PINGPONG				(1l <<16)
#define KP_LOOPREL				(1l <<17)
#define KP_PRELOOP				(1l <<18)
#define KP_PREPINGPONG			(1l <<19)
#define KP_PRELOOPREL			(1l <<20)
#define KP_LOCKED2				(1l <<21)
#define KP_LOCKED2_FIRST		(1l <<22)
#define KP_FLAT					(1l <<23)
#define KP_SMOOTH				(1l <<24)
#define KP_FOLLOWING_EXT		(1l <<25)
//#define KP_LSHAPE				(1l <<26)
//#define KP_RSHAPE				(1l <<27)
#define KP_EXTRAPOLATE			(1l <<28)
#define KP_SHAPED				(1l <<29)
#define KP_CREATED				(1l <<29)
#define KP_CLOSED_SHAPED		(1l <<30)
#define KP_TAGGED				(1l <<31)

#define PT_MOVE_XWISE			(1l << 0)
#define PT_MOVE_YWISE			(1l << 1)
#define PT_MOVE_ZWISE			(1l << 2)
#define PT_NMOVE_XWISE			(1l << 3)
#define PT_NMOVE_YWISE			(1l << 4)
#define PT_NCHECK_LIMIT			(1l << 5)
#define PT_SHIFT_XWISE			(1l << 6)
#define PT_SHIFT_YWISE			(1l << 7)
#define PT_MOVE_NSNAP			(1l << 8)
#define PT_SCALE_XWISE			(1l << 9)
#define PT_SCALE_YWISE			(1l <<10)
#define PT_NMOVE_HANDLES		(1l <<11)
#define PT_MOVE_INDIVIDUAL		(1l <<12)
#define PT_MOVE_CURVE_VALUE		(1l <<13)


#define PT_PREV					(1l << 0)
#define PT_NEXT					(1l << 1)
#define PT_CLOSED				(1l << 2)
#define PT_FORCE				(1l << 3)
#define PT_KEEP_TANGENTS		(1l << 4)
#define PT_EDIT					(1l << 5)

#define SPL_SMOOTH_FIRST_LAST_NHORZ		(1l << 0)
#define SPL_SMOOTH_NLIN_SEG				(1l << 1)
#define SPL_SMOOTH_MAKE_POSITIVE_SEG	(1l << 2)
#define SPL_SMOOTH_ONLY_SEG				(1l << 3)
#define SPL_SMOOTH_FAST					(1l << 4)
#define SPL_SMOOTH_ALL					(1l << 5)
#define SPL_SMOOTH_KEEP_FIRST_LAST		(1l << 6)
#define SPL_CLOSED						(1l << 7)
#define SPL_SET_HANDLES_INCLUDED		(1l << 8)
#define SPL_SMOOTH_SEGMENTS_ONLY		(1l << 9)
#define SPL_SMOOTH_NEXT_SEGMENT			(1l << 10)

#define IsKP(x) (!( (x)   %3))
#define IsRH(x) (!(((x)-1)%3))
#define IsLH(x) (!(((x)-2)%3))

inline double pow2(double x)
{
	return x*x;
}

inline double pow3(double x)
{
	return x*x*x;
}

inline double pow4(double x)
{
	return x*x*x*x;
}

inline double pow5(double x)
{
	return x*x*x*x*x;
}

enum HoverTypeIDs
{
	HVR_NONE = 0,
	HVR_KEYPOINT,
	HVR_LCONTROL,
	HVR_RCONTROL,
	HVR_TCONTROL,
	HVR_BCONTROL,
	HVR_SEGMENT,
	HVR_HULL,

	HVR_DRAW	=	(1l << 8),
};

struct HoverInfo
{
	UINT type;
	int index;
	int index_end;
	HoverInfo() { type = HVR_NONE; index = -1; index_end = -1; }
	bool operator == (HoverInfo &hv ){ return type==hv.type && index==hv.index && index_end == hv.index_end; } 
	HoverInfo& operator=( HoverInfo& hv ){ type = hv.type; index = hv.index; index_end = hv.index_end; return *this; }
	void Reset(){ type = HVR_NONE; index = index_end = -1; }
};

//#define KP_KFDISP_RECT	(1l <<12)

struct KeyPoint
{
	uint64 s;			// 1st bit on/off: the point is selected/deselected; 
						// 2nd bit on/off: left segment linearized/delinearized; 
						// 3th bit is on: disabled for deleting manually and for moving in X direction.
						// 4th bit is on: disabled for deleting manually and for moving in Y direction.
						// 5rd bit on: the point is not the first nor the last of the link.
						// this bit is set immediately before and reset immediately after drawing
						// a part of the link.
						// 6th bit on: point is selected for stretching or shaping via control box.
						// 7th bit on: deep copying of KeySplines represented by this KeyPoint.
						// 8th bit on: the point is selected in KeyFrameView only.
	double X;			// X value of key point.
	double Y;			// Y value of key point.
	KeyPoint *next;		// pointer to the next KeyPoint.
	KeyPoint *prev;		// pointer to the previous KeyPoint.
	FuPointer<Parameter>  Par;

	KeyPoint(uint64 sx = 0, double xx = 0.0, double yx = 0.0) {s = sx; X = xx; Y = yx; Par = NULL; }
	KeyPoint& operator=( KeyPoint& p ){ s=p.s; X=p.X; Y=p.Y; Par = p.Par; return *this; }
	KeyPoint& operator[]( int i ){ int j; KeyPoint *p; if( i>=0 ) for( j=0,p=this; j<i && p; p=p->next,j++ ); else for( j=0,p=this; j>i && p; p=p->prev,--j ); return *p; }// subscription operator
	bool operator == (KeyPoint &p ){ return fabs(X-p.X)<FL_ERR && fabs(Y-p.Y)<FL_ERR? TRUE : FALSE; } 
	int GetCount(){ int i=0; KeyPoint *p=this; for( ; p; p=p->next,++i ); return i; }
	int GetIndex(){ int i=0; KeyPoint *p; for(p=this; p->prev; p = p->prev, ++i ); return (p? i : -1); }
	int GetIndex( KeyPoint *pc ){ int i=0; KeyPoint *p; for(p=this; p && p!=pc; p = p->next, ++i ); return (p? i : -1); }
//	KeyPoint *GetFirst( uint32 attrib ){ for( KeyPoint *p=this; p && (p->s&attrib)!=attrib; p=p->next ); return p; }
	KeyPoint *GetFirst( uint64 attrib, KeyPoint *pl=NULL ){ KeyPoint *p; for(p=this; p && (p->s&attrib)!=attrib && p!=pl; p=p->next ); return (p==pl? NULL : p); }
	KeyPoint *GetFirstSelected( uint64 attrib ){ KeyPoint *p; for(p=this; p && ((p->s&attrib)!=attrib || !(p->s&(KP_SELECTED|KP_SELECTEDEX))); p=p->next ); return p; }
	KeyPoint *GetLast(){ KeyPoint *p; for(p=this; p && p->next; p=p->next ); return p; }
	KeyPoint *GetLast( uint64 attrib ){ KeyPoint *pa, *p; for(pa=NULL,p=this; p; p=p->next ) pa = (p->s&attrib)==attrib? p : pa; return pa; }
//	KeyPoint *GetPrev( KeyPoint *h ){ if(h==this) return NULL; for( KeyPoint *p=h; p && p->next && p->next!=this; p=p->next ); return p; }
//	KeyPoint *GetPrev3( KeyPoint *h ){ if(h==this) return NULL; for( KeyPoint *p=h; p && p->next && p->next->next && p->next->next->next!=this; p=p->next->next->next ); return p; }
	KeyPoint *GetSegment( double xc ){ KeyPoint *p; for(p=this; p && p->next && p->next->X<=xc; p=p->next ); return p; }
	KeyPoint *GetSegment( uint64 attrib ){ KeyPoint *p; for(p=this; p && ((p->s&attrib)!=attrib || p->next && (p->next->s&attrib)!=attrib); p=p->next ); return p; }
	KeyPoint *GetSegment3( double xc ){ KeyPoint *p; for(p=this; p && p->next && p->next->next && p->next->next->next && p->next->next->next->X<=xc; p=p->next->next->next ); return p; }
	KeyPoint *GetSegment3( uint64 attrib ){ KeyPoint *p; for(p=this; p && ((p->s&attrib)!=attrib || p->next && p->next->next && p->next->next->next && (p->next->next->next->s&attrib)!=attrib); p=&(*p)[3] ); return p; }
	KeyPoint *Next(uint64 attrib){ KeyPoint *p; for(p=this->next; p && (p->s&attrib)!=attrib; p=p->next ); return p; }
	KeyPoint *Prev(uint64 attrib){ KeyPoint *p; for(p=this->prev; p && (p->s&attrib)!=attrib; p=p->prev ); return p; }
	KeyPoint *Next1(int i, uint32 ex){ int di=(i>=0? 1 : -1); int ip = 0; KeyPoint *p; for(p = this; abs(ip)<abs(i) && p; p=&(*p)[di],ip=(p && !(p->s&ex)? ip+di : ip) ); return p; }
	KeyPoint *Next3(int i, uint32 ex){ int di=(i>=0? 3 : -3); int ip = 0; KeyPoint *p; for(p = this; abs(ip)<abs(i) && p; p=&(*p)[di],ip=(p && !(p->s&ex)? ip+di : ip) ); return p; }
	KeyPoint *NextLink(){ KeyPoint *pl; for(pl = this->next; pl && pl->prev; pl = pl->next); return pl; }
	KeyPoint *NextPar(){ KeyPoint *p; for(p = this->next; p && !(Parameter *)p->Par; p = p->next ); return p; }  
	KeyPoint *PrevPar(){ KeyPoint *p; for(p = this->prev; p && !(Parameter *)p->Par; p = p->prev ); return p; }  
	KeyPoint *GetObjPt(){ return ((Parameter *)this->Par? this : NextPar()); }
	uint64 PackSFlagToSave()
	{
		uint64 sx = s & (KP_LINEAR|(s & KP_PICK? 0 : KP_LOCKX)|KP_LOCKY);
		sx |= (s & KP_STEP_IN? KP_SELECTED : (s & KP_STEP_OUT? (KP_SELECTED|KP_LINEAR) : 0));
		if(s & KP_PINGPONG)
			sx |= KP_SHAPE;
		else if(s & KP_LOOPREL)
			sx |= (KP_SHAPE|PKP_INITIAL);
		else if(s & KP_LOOP)
			sx |= PKP_INITIAL;
		if(s & KP_PREPINGPONG)
			sx |= KP_COPYKS;
		else if(s & KP_PRELOOPREL)
			sx |= (KP_COPYKS|KP_SELECTEDEX);
		else if(s & KP_PRELOOP)
			sx |= KP_SELECTEDEX;
		sx |= (s & (KP_FLAT|KP_SMOOTH));
		return sx;
	}
	void ResetAttrib( uint64 attrib ){ for( KeyPoint *p=this; p; p = p->next ) p->s&=~attrib; }
	void SetPrevPtrs(){ this->prev = NULL; for( KeyPoint *p = this; p && p->next; p=p->next ) p->next->prev = p; }
	void UnpackSFlag( uint64 sx )
	{
		s = sx;
		if(s & KP_SELECTED)
		{
			s &= ~KP_SELECTED;
			if(s & KP_LINEAR)
			{
				s &= ~KP_LINEAR;
				s |= KP_STEP_OUT;
			} else
				s |= KP_STEP_IN;
		}
		if(s & (PKP_INITIAL|KP_SHAPE))
			if((s & PKP_INITIAL) && (s & KP_SHAPE))
			{
				s &= ~(PKP_INITIAL|KP_SHAPE);
				s |= KP_LOOPREL;
			} else if(s & PKP_INITIAL)
			{
				s &= ~PKP_INITIAL;
				s |= KP_LOOP;
			} else
			{
				s &= ~KP_SHAPE;
				s |= KP_PINGPONG;
			}
			if(s & (KP_SELECTEDEX|KP_COPYKS))
				if((s & KP_SELECTEDEX) && (s & KP_COPYKS))
				{
					s &= ~(KP_SELECTEDEX|KP_COPYKS);
					s |= KP_PRELOOPREL;
				} else if(s & KP_SELECTEDEX)
				{
					s &= ~KP_SELECTEDEX;
					s |= KP_PRELOOP;
				} else
				{
					s &= ~KP_COPYKS;
					s |= KP_PREPINGPONG;
				}
	}

	bool IsSelected()  { return (s & (KP_SELECTED|KP_SELECTEDEX)) != 0; } //    1|128
	bool IsLinear()    { return (s & KP_LINEAR) != 0;                   } //    2
	bool IsLockX()     { return (s & KP_LOCKX) != 0;                    } //    4
	bool IsLockY()     { return (s & KP_LOCKY) != 0;                    } //    8
	bool IsInitial()   { return (s & PKP_INITIAL) != 0;                 } //   16
	bool IsShape()     { return (s & KP_SHAPE) != 0;                    } //   32
	bool IsCopyKS()    { return (s & KP_COPYKS) != 0;                   } //   64
	bool IsStepIn()    { return (s & KP_STEP_IN) != 0;                  } //  256
	bool IsStepOut()   { return (s & KP_STEP_OUT) != 0;                 } //  512
	bool IsHighlight() { return (s & KP_HIGHLIGHT) != 0;                } // 1024
};

struct PathKeyPoint : public KeyPoint
{
	double	d;							// distance along the total line (0.0 <= d <= 1.0)
	PathKeyPoint(uint64 sx = 0, double xx = 0.0, double yx = 0.0, double dx = 0.0) : KeyPoint(sx,xx,yx) { d = dx; }
	PathKeyPoint& operator=( PathKeyPoint& p ){ s=p.s; X=p.X; Y=p.Y; d=p.d; return *this; }
};

#define PPT_CLEFT		-1
#define PPT_KEY		0
#define PPT_CRIGHT	1
#define PPT_TCRIGHT	2

#define PPF_NEDIT	1

struct PtInfo
{
	int ind;
	int control;
	KeyPoint *pt;
	class Spline *owner;
	PtInfo(){ ind = -1; control = PPT_KEY; pt = NULL; owner = NULL; mt = NULL; flag = 0; }
	PtInfo(int i, KeyPoint *p){ ind = i; pt = p; control = PPT_KEY, owner = NULL; mt = NULL; flag = 0; }
	PtInfo& operator=( PtInfo& pi ){ ind=pi.ind; control=pi.control; pt=pi.pt; owner=pi.owner; mt = pi.mt; flag = pi.flag; return *this; }
	void Set(int i, KeyPoint *p, class Spline *ow=NULL, int ctrl=PPT_KEY){ ind = i; pt = p; owner = ow; control = ctrl; }
	void Reset(){ ind = -1; control = PPT_KEY; pt = NULL; owner = NULL; mt = NULL; flag = 0; }

	class Matrix4 *mt;
	uint8 flag;
};

struct PathPt
{
	double X, Y;
	TimeStamp Time;
	PathPt(){ X = 0.0; Y = 0.0; Time = 0.0; }
	PathPt(double _x, double _y, TimeStamp _time){ X = _x; Y = _y; Time = _time; }
};

// command IDs for Do() methods
enum PATH_DoMethod_enums
{
	PATH_AddPoints = OBJECT_BASE + TAGOFFSET*2,		// Usage: pathTool->Do(PATH_AddPoints, OBJP_ObjPtr, &pointlist, TAG_DONE);
};


inline void DeleteLinkList( KeyPoint **h ){ if(!*h) return; for ( KeyPoint *temp = *h; *h; temp = *h ){ *h = (*h)->next; delete temp; } }

inline double CalcBezDX( KeyPoint *ps, double u, KeyPoint *pe=NULL )
{
	pe = (pe? pe : ps->Next3(3,KP_EXCLUDE));
	KeyPoint *pl = (pe->prev? pe->prev : ps->next->next);
	return (9.0 * ps->next->X - 9.0 * pl->X - 3.0 * ps->X + 3.0 * pe->X)*pow2(u) + (6.0*ps->X + 6.0*pl->X - 12.0 * ps->next->X)*u + 3.0 * ps->next->X - 3.0 * ps->X;
}

inline double CalcBezDY( KeyPoint *ps, double u, KeyPoint *pe=NULL )
{
	pe = (pe? pe : ps->Next3(3,KP_EXCLUDE));
	KeyPoint *pl = (pe->prev? pe->prev : ps->next->next);
	return (9.0 * ps->next->Y - 9.0 * pl->Y - 3.0 * ps->Y + 3.0 * pe->Y)*pow2(u) + (6.0*ps->Y + 6.0*pl->Y - 12.0 * ps->next->Y)*u + 3.0 * ps->next->Y - 3.0 * ps->Y;
}

inline double CalcBezX( KeyPoint *ps, double u, KeyPoint *pe=NULL )
{
	pe = (pe? pe : ps->Next3(3,KP_EXCLUDE));
	KeyPoint *pl = (pe->prev? pe->prev : ps->next->next);
	return ps->X * pow3(1.0-u) + 3.0 * ps->next->X*u*pow2(1.0-u) + 3.0 * pl->X*(1.0-u)*pow2(u) + pe->X*pow3(u);
}

inline double CalcBezY( KeyPoint *ps, double u, KeyPoint *pe=NULL )
{
	pe = (pe? pe : ps->Next3(3,KP_EXCLUDE));
	KeyPoint *pl = (pe->prev? pe->prev : ps->next->next);
	return ps->Y * pow3(1.0-u) + 3.0 * ps->next->Y*u*pow2(1.0-u) + 3.0 * pl->Y*(1.0-u)*pow2(u) + pe->Y*pow3(u);
}

inline void CalcHandlerSlope( KeyPoint *ps,bool second, double &dx, double &dy )
{
	float u = second? 0.995 : 0.005;
	dx = CalcBezDX(ps,u);
	dy = CalcBezDY(ps,u);
//	return ( dx==0.0? ( dy==0.0? float(0.0) : 100000.0 ) : dy/dx );
}

inline double CalcSqrParExtr( double x1,double x2, double x3, double y1, double y2, double y3 ){ return -0.5*( (y2-y3)*(x1-x3)*(x1+x3) + (y3-y1)*(x2-x3)*(x2+x3) )/( y1*x2-y1*x3-y3*x2-y2*x1+y2*x3+y3*x1 ); }

inline bool EqualLinkLists( KeyPoint *p1, KeyPoint *p2 )
{
	for( ; p1 && p2 && p1->X==p2->X && p1->Y==p2->Y; p1=p1->next,p2=p2->next );
	return (p1 || p2? FALSE : TRUE );
}

inline double FloatToDouble(float32 x)
{
	char buf[30];

	sprintf(buf, (fabs(x) < 1.0) ? "%.*f" : "%.*e", FLT_DIG, x);
	return atof(buf);
}

DLLExtern double CalcClosestPoint(double x, double y, KeyPoint **prev, KeyPoint *ph, double &px, double &py, float32 asp=1.0 );
DLLExtern void CalcCubicCoefBz( double *gx, KeyPoint *parr, int n );
DLLExtern void CalcSlope( KeyPoint *sp, double X, double Y, double &dx, double &dy );
DLLExtern void CalcXY( KeyPoint *sp, KeyPoint *ep, double dist, double &px, double &py );
DLLExtern void CleanLinkList( KeyPoint **ph, bool head );
DLLExtern KeyPoint *CopyLinkList( KeyPoint *ps, KeyPoint **pd );
DLLExtern void CreateCircle( KeyPoint *ps, float32 asp, bool ccw=FALSE );
DLLExtern double Distance( KeyPoint *sp, KeyPoint *ep, double x, double y );
DLLExtern double GetHandlerAmplitude( KeyPoint *pa, double dx, double dy, float32 asp, float32 ra=0.3333 );
DLLExtern void InterpolateLinkList( KeyPoint *k1, KeyPoint *k2, double w, KeyPoint **hd );
DLLExtern void LinearizePoint( KeyPoint *pl, float asp, uint32 flag );
DLLExtern void RepairHandlerPoints( KeyPoint *h, KeyPoint *pt );
DLLExtern void RotateHandlers( KeyPoint *prv, KeyPoint *pt, float32 asp );
DLLExtern void RotateLeftHandler( KeyPoint *pl, float asp );
DLLExtern void RotateRightHandler( KeyPoint *pl, float asp );
DLLExtern void SmoothPoint( KeyPoint *ps, float asp, uint32 flag, bool loop=FALSE );
DLLExtern void SmoothSegment( KeyPoint *ps, float asp, uint32 sflg = PT_PREV|PT_NEXT);
DLLExtern void SmoothKeyPointsFast( KeyPoint *ph, KeyPoint *pt, float32 asp, uint32 flag );
DLLExtern void SmoothKeyPointsFast2( KeyPoint *pf, KeyPoint *pl, float32 asp, uint32 flag );
DLLExtern void SetColinearLHandler( KeyPoint *pl, double dx, double dy , float asp );
DLLExtern void SetColinearRHandler( KeyPoint *pl, double dx, double dy , float asp );
DLLExtern void SetHandlerLinear( KeyPoint *kp, uint8 fl );
DLLExtern void SetHandlerStep( KeyPoint *kp, uint8 fl );
DLLExtern void SetHandlerSmooth( KeyPoint *prev, double x, double y, KeyPoint *h1, KeyPoint *h2, float32 asp );
DLLExtern void SetLinearSegment( KeyPoint *ps );
DLLExtern void SetStepSegment( KeyPoint *ps );
DLLExtern void SmoothCubic(KeyPoint *k1, KeyPoint *k2, float32 asp);
DLLExtern void TriDLinEqu( double *dx, double *ex, double *fx, double *gx, int n );

void InsertLink( KeyPoint *, KeyPoint *, uint32 );
void InvertLinkX( KeyPoint **ph );
void SetControlPointsSymetrical(KeyPoint *p1, KeyPoint *p2, float32 asp);
double SetColinearLHandler( KeyPoint *pl, KeyPoint *pn , float32 asp, double &x);
double SetColinearRHandler( KeyPoint *pl, KeyPoint *pn , float32 asp, double &x);
void CalcCubicCoefCb( double *gx, KeyPoint *parr, int n );
BOOL IsTimeNegative( KeyPoint *ph, KeyPoint *pt );
void RepairTime( KeyPoint *ps, float32 asp );
void GetFirstControlPoints(double *rhs, int n, bool start, bool end, double *x);
void SmoothBezierHandles(KeyPoint *pf, KeyPoint *pl, int count, float32 asp);

// 00: save as SMOOTH
// 01: save as STEP
// 10: save as LINEAR
// 11: save as STEPINV
#define PT_SIZE			5
#define PT_SIZE_TRI		6
#define PT_SIZE_PAD		8
#define PT_SIZE_RHOMB	6
#define PT_SIZE_HIGHLIGHT	6

/*#define PPT_SIZE			4
#define PPT_SIZE_HANDLE		3
#define PPT_SIZE_HIGHLIGHT	5
#define PPT_SIZE_LOCKED		6
#define PPT_SIZE_PUBLISHED	8*/
#define PPT_SIZE			5.0
#define PPT_SIZE_HANDLE		4.0
#define PPT_SIZE_HIGHLIGHT	6.0
#define PPT_SIZE_LOCKED		7.0
#define PPT_SIZE_PUBLISHED	8.0
#define PPT_SIZE_RHOMB		8.0

#endif