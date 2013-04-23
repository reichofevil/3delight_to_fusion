#ifndef _IMAGEREGION_H_
#define _IMAGEREGION_H_


#include "types.h"


/*

  ***** ImagePixelRegion

*/

class FuSYSTEM_API ImagePixelRegion
{
public :
	int32 Top, Left;
	int32 Width, Height;

	ImagePixelRegion(int32 left = 0, int32 top = 0, int32 width = 0, int32 height = 0);
	void Set(int32 left, int32 top, int32 width, int32 height);

	bool IsEmpty();

	ImagePixelRegion Scale(int32 Sc);
};

class FuSYSTEM_API ImageRegion
{
public :
	double Top, Bottom, Left, Right;

	ImageRegion(double left = 0.0, double top = 0.0, double right = -100000.0, double bottom = -100000.0);
	void Set(double left, double top, double right, double bottom);

	bool IsEmpty();
	bool IsInFrame(double framewidth = 1.0, double frameheight = 1.0);
	bool IsInFrame(ImageRegion *region);
	void Empty();
	void Add(double x, double y);
	void Add(ImageRegion region);

	void Expand(double x, double y);

	ImagePixelRegion ToPixelRegion(int width, int height, double pixelaspect);
};

#endif //_IMAGEREGION_H_