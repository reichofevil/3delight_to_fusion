#ifndef _RESMATH_H_
#define _RESMATH_H_

#include "Types.h"

class FuSYSTEM_API ResPosition
{
private : 
	bool IsFit;
	int32 Left, Top, Width, Height;
	int32 RealLeft, RealTop, RealWidth, RealHeight; // Set by Fit(ing) to an actual frame with a certain width/height

public :
	ResPosition(int32 left = 0, int32 top = 0, int32 width = 0, int32 height = 0);

	void Set(int32 left = 0, int32 top = 0, int32 width = 0, int32 height = 0)
	{ 
		IsFit = FALSE;
		Left = RealLeft = left;
		Top = RealTop = top; 
		Width = RealWidth = width;
		Height = RealHeight = height; 
	}

	bool IsValid() { return IsFit; }

	void SetLeft(int32 left) { IsFit = FALSE; Left = RealLeft = left; }
	void SetTop(int32 top) { IsFit = FALSE; Top = RealTop = top; }
	void SetWidth(int32 width) { IsFit = FALSE; Width = RealWidth = width; }
	void SetHeight(int32 height) { IsFit = FALSE; Height = RealHeight = height; }

	ResPosition &Fit(int32 &framewidth, int32 &frameheight, bool adapt = TRUE); // Converts negative values to the specified frame size, extends framesize if required and adapt == TRUE
	ResPosition &Fit(int32 &framewidth, int32 &frameheight, int32 offsetx, int32 offsety, bool adapt = TRUE); // ... and offsets

	int32 GetLeft() { ASSERT(IsFit); return RealLeft; }
	int32 GetRight() { ASSERT(IsFit); return RealLeft + RealWidth; }
	int32 GetTop() { ASSERT(IsFit); return RealTop; }
	int32 GetBottom() { ASSERT(IsFit); return RealTop + RealHeight; }
	int32 GetWidth() { ASSERT(IsFit); return RealWidth; }
	int32 GetHeight() { ASSERT(IsFit); return RealHeight; }

	CRect GetRect() { ASSERT(IsFit); return CRect(RealLeft, RealTop, RealLeft+RealWidth, RealTop+RealHeight); }
	CRect GetRectExpanded(int32 all) { ASSERT(IsFit); return CRect(RealLeft - all, RealTop - all, RealLeft+RealWidth + 2*all, RealTop+RealHeight + 2*all); }
	CRect GetRectExpanded(int32 left, int32 top, int32 right, int32 bottom) { ASSERT(IsFit); return CRect(RealLeft - left, RealTop - top, RealLeft+RealWidth + left + right, RealTop+RealHeight + top + bottom); }

	int32 GetOriginalLeft() { return Left; }
	int32 GetOriginalTop() { return Top; }
	int32 GetOriginalWidth() { return Width; }
	int32 GetOriginalHeight() { return Height; }

	bool Inside(int32 x, int32 y) { ASSERT(IsFit); return (x >= RealLeft) && (x<= RealLeft+RealWidth) && (y >= RealTop) && (y <= RealTop+RealHeight); }
	bool InsideHorizontally(int32 x) { ASSERT(IsFit); return (x >= RealLeft) && (x<= RealLeft+RealWidth); }
	bool InsideVertically(int32 y) { ASSERT(IsFit); return (y >= RealTop) && (y <= RealTop+RealHeight); }
};

#endif // _RESMATH_H_
