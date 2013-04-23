#ifndef _POINT_H_
#define _POINT_H_

#include "Parameter.h"
#include "Input.h"

#define POINT_ID zMAKEID('Poin')
//#define CLSID_DataType_Point POINT_ID

class FuSYSTEM_API Point : public Parameter
{
	FuDeclareClass(Point, Parameter); // declare ClassID static member, etc.

public:
	float64 X, Y, Z;

public:
	Point(float64 valueX = 0.5, float64 valueY = 0.5, float64 valueZ = 0.0);
	Point(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Point(const Point &param);

	static const Point &Get(Input *in, Request *req) { return *(Point *) in->GetValue(req); }

	virtual bool Save(ScriptVal &table, const TagList &tags);

	virtual ~Point();

	bool operator ==(Point& pt)   { return (fabs(X - pt.X) < 0.00001 && fabs(Y - pt.Y) < 0.00001 && fabs(Z - pt.Z) < 0.00001); }
	Point& operator =(Point& pt)  { X = pt.X; Y = pt.Y; Z = pt.Z; return *this; }
	Point operator +(Point &pt)   { return Point(X + pt.X, Y + pt.Y, Z + pt.Z); }
	Point operator -(Point &pt)   { return Point(X - pt.X, Y - pt.Y, Z - pt.Z); }
	virtual Parameter *Copy();

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Point);
#endif

protected:
	virtual bool _IsValid();
};

class AutoPoint : public AutoParameter
{
	FuDeclareClass(AutoPoint, AutoParameter); // declare ClassID static member, etc.

public:
	AutoPoint(float64 valueX = 0.5, float64 valueY = 0.5, float64 valueZ = 0.0)	{ Param = new Point(valueX, valueY, valueZ); }
	AutoPoint(Input *in, TimeStamp time) : AutoParameter(in, time) {}
	AutoPoint(Input *in, Request *req, TimeStamp time = TIME_UNDEFINED) : AutoParameter(in, req, time) {}
	operator Point *()					{ return (Point *)Param; }
};

#endif
