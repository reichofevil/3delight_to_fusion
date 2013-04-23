#ifndef _FURECT_H_
#define _FURECT_H_

#include "Types.h"

#define DFR_NULL		-1000000

#ifndef __AFXWIN_H__
struct CRect
{
   int32 left;
   int32 top;
   int32 right;
   int32 bottom;
};
#endif

struct FuSYSTEM_API FuRect
{
	float32 left, bottom;			// bottom == min y
	float32 right, top;				// top == max y

	FuRect();
	FuRect(float32 left, float32 bottom, float32 right, float32 top);
	FuRect(const CRect &rect);		// Note: This normalises so that bottom is min-y, top is max-y

	bool operator == (const FuRect &rect) const;
	bool operator != (const FuRect &rect) const;
	FuRect operator | (const FuRect &rect) const;
	FuRect operator & (const FuRect &rect) const;
	FuRect & operator |= (const FuRect &rect);
	FuRect & operator &= (const FuRect &rect);
	operator CRect () const;

	bool IsNull() const																	{ return (top == DFR_NULL && left == top && right == top && bottom == top); }
	void SetNull();

	bool IsEmpty() const																	{ return fabsf(right - left) < FLT_EPSILON || fabsf(top - bottom) < FLT_EPSILON; }

	float32 Width() const																{ return right - left; }
	float32 Height() const																{ return top - bottom; }

	bool IsWithin(float32 x, float32 y) const										{ return (x >= left && x < right && y >= bottom && y < top); }
	bool IsWithin(const FuRect &rect) const;

	void Set(float32 l, float32 b, float32 r, float32 t)						{ left = l; bottom = b; right = r; top = t; }

	void Get(float32 &l, float32 &b, float32 &r, float32 &t) const			{ l = left; b = bottom; r = right; t = top; }
	void Get(int32 &l, int32 &b, int32 &r, int32 &t) const					{ l = floorf(left+0.5f); b = floorf(bottom+0.5f); r = floorf(right+0.5f); t = floorf(top+0.5f); }

	void Offset(float32 x, float32 y)												{ left += x; bottom += y; right += x; top += y; }
	void Offset(int32 x, int32 y)														{ left += x; bottom += y; right += x; top += y; }
	void Offset(int x, int y)															{ Offset(int32(x), int32(y)); }
	void Inflate(float32 x, float32 y)												{ left -= x; bottom -= y; right += x; top += y; }
	void Inflate(int32 x, int32 y)													{ left -= x; bottom -= y; right += x; top += y; }
	void Inflate(int x, int y)															{ Inflate(int32(x), int32(y)); }
	void Scale(float32 x, float32 y)													{ left *= x; bottom *= y; right *= x; top *= y; }
	void Scale(float32 x)																{ Scale(x, x); }
	void Normalise()																		{ if (right < left) FlipX(); if (top < bottom) FlipY(); }
	void FlipX()																			{ float32 t = left; left = right; right = t; }
	void FlipY()																			{ float32 t = top; top = bottom; bottom = t; }
};

struct FuSYSTEM_API FuRectInt
{
	int32 left, bottom;			// bottom == min y
	int32 right, top;				// top == max y

	FuRectInt();
	FuRectInt(int32 left, int32 bottom, int32 right, int32 top);
	FuRectInt(const CRect &rect);		// Note: This normalises so that bottom is min-y, top is max-y

	bool operator == (const FuRectInt &rect) const;
	bool operator != (const FuRectInt &rect) const;
	FuRectInt operator | (const FuRectInt &rect) const;
	FuRectInt operator & (const FuRectInt &rect) const;
	FuRectInt & operator |= (const FuRectInt &rect);
	FuRectInt & operator &= (const FuRectInt &rect);
	operator CRect () const;

	bool IsNull() const																	{ return (top == DFR_NULL && left == top && right == top && bottom == top); }
	void SetNull();

	bool IsEmpty() const																	{ return right - left == 0 || top - bottom == 0; }

	int32 Width() const																	{ return right - left; }
	int32 Height() const																	{ return top - bottom; }

	bool IsWithin(int32 x, int32 y) const											{ return (x >= left && x < right && y >= bottom && y < top); }
	bool IsWithin(float32 x, float32 y) const										{ return (x >= left && x < right && y >= bottom && y < top); }
	bool IsWithin(const FuRectInt &rect) const;

	void Set(int32 l, int32 b, int32 r, int32 t)									{ left = l; bottom = b; right = r; top = t; }

	void Get(float32 &l, float32 &b, float32 &r, float32 &t) const			{ l = left; b = bottom; r = right; t = top; }
	void Get(int32 &l, int32 &b, int32 &r, int32 &t) const					{ l = left; b = bottom; r = right; t = top; }

	void Offset(float32 x, float32 y)												{ left += floorf(x+0.5f); bottom += floorf(y+0.5f); right += floorf(x+0.5f); top += floorf(y+0.5f); }
	void Offset(int32 x, int32 y)														{ left += x; bottom += y; right += x; top += y; }
	void Offset(int x, int y)															{ Offset(int32(x), int32(y)); }
	void Inflate(float32 x, float32 y)												{ left -= floorf(x+0.5f); bottom -= floorf(y+0.5f); right += floorf(x+0.5f); top += floorf(y+0.5f); }
	void Inflate(int32 x, int32 y)													{ left -= x; bottom -= y; right += x; top += y; }
	void Inflate(int x, int y)															{ Inflate(int32(x), int32(y)); }
	void Scale(float32 x, float32 y)													{ left = floorf(left*x+0.5f); bottom = floorf(bottom*y+0.5f); right = floorf(right*x+0.5f); top = floorf(top*y+0.5f); }
	void Scale(float32 x)																{ Scale(x, x); }
	void Normalise()																		{ if (right < left) FlipX(); if (top < bottom) FlipY(); }
	void FlipX()																			{ left ^= right; right ^= left; left ^= right; }
	void FlipY()																			{ top ^= bottom; bottom ^= top; top ^= bottom; }
};

#endif
