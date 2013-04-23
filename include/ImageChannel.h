#ifndef _IMAGECHANNEL_H_
#define _IMAGECHANNEL_H_

#include "ImageRegion.h"
#include "Shape.h"
#include "VectorMath.h"
#include "Image.h"
#include "Gradient.h"

enum SuperSamplingType
{
	SS_None,
	SS_Pixel,
	SS_Area,
};

enum CombineMode
{
	CM_Copy,
	CM_Merge,
};

// ***** Fill

enum FillMode
{
	FM_Fill,
	FM_Outline,
};

class FuSYSTEM_API FillStyle
{
public:
	FillMode Type;
	FillOverlap Overlap;

	OutlineLineType LineType;
	float64 Thickness, RelativeThickness;

	float64 Expand;
	OutlineJoinType ExpandType;
	bool ExpandIntelligent;

	bool OutsideOnly;
	OutlineJoinType JoinType;
	bool AdaptToPerspective;

	float64 ImageWidth; //Used for properly adapting Thickness & Expand

	FillStyle();

	bool IsTraceCompatibleWith(FillStyle *fill);
	float64 ActualThickness();

	HPEN HPen(int thickness);
};

// ***** ChannelStyle

#define	CT_Solid 0
#define	CT_Image 1
#define	CT_Gradient 2

#define	CIT_Tool 0
#define	CIT_File 1
#define	CIT_Brush 2

enum ChannelImageLevel	
{
	IL_Full,
	IL_Text,
	IL_Line,
	IL_Tab,
	IL_Word,
	IL_Char,
};

class FuSYSTEM_API ChannelStyle
{
public:
	int Type, BevelType;

	float64 SoftnessX, SoftnessY, SoftnessGlow, SoftnessBlend;
	bool SoftnessImage;
	BlurTypes BlurType;
	float64 Opacity;

	Pixel64 Color;
	PixelFlt FltColor;

	Pixel64 BgColor;
	PixelFlt BgFltColor;

	Gradient *ColorGradient;
	int ColorMapping;
	float64 ColorMappingAngle, ColorMappingSize, ColorMappingAspect;
	Image *ColorImage, *ColorImageBevel;
	SuperSamplingType ColorImageSample;
	EdgeModes ColorImageEdges;

	Matrix4 ImageTransform;
	ChannelImageLevel Level;

	ChannelStyle();
	bool RequiresNewImage(int Line, int Tab, int Word, int Char);
	bool IsRenderCompatibleWith(ChannelStyle *channel);

	Matrix4 GetImageTransformInverse();
};

class ImageChannel;
typedef void (*FlushFunc)(ImageChannel *ic, unsigned short *AccumBuffer, int &AccumX0, int &AccumX1, Image *Img, int ActualWidth, int SuperSampleN, int y);

// ***** ImageChannel

#define WINDING_EvenOdd 0
#define WINDING_NonZero 1

class FuSYSTEM_API ImageChannel
{
public:
	void ScanShape   (int pointCount, ShapeArrayPoint *points, ShapeArrayPointIndex *indices, ShapeArrayEdge *edges, int ymin, int ymax, int yinc = 1, FlushFunc ff = NULL, int *StatusPtr = NULL, ImgMPData *mpdata = NULL);
	void ScanShape1x1(int pointCount, ShapeArrayPoint *points, ShapeArrayPointIndex *indices, ShapeArrayEdge *edges, int ymin, int ymax, int yinc = 1, FlushFunc ff = NULL, int *StatusPtr = NULL, ImgMPData *mpdata = NULL);
	void ScanShape8x8(int pointCount, ShapeArrayPoint *points, ShapeArrayPointIndex *indices, ShapeArrayEdge *edges, int ymin, int ymax, int yinc = 1, FlushFunc ff = NULL, int *StatusPtr = NULL, ImgMPData *mpdata = NULL);

	Image *Img;
	uint8 *ImageMap;
	int ActualWidth, ActualHeight;
	ImagePixelRegion Original, Actual;

	float64 XAspect, YAspect;
	int SuperSampleX,  SuperSampleY, SuperSampleN;

	int Winding;

	int PointToPixelXOffset, PointToPixelYOffset;

	void GenerateChannelMask(Image *image, ImagePixelRegion region, CombineMode mode, ChannelStyle &style);

public:
	ImageChannel(Image *image, int supersample, ImageRegion *region = NULL, bool createmap = TRUE);
	~ImageChannel();

	void SetStyleFill(FillStyle *fillstyle);

	// shape
	int ShapeFill(Shape *shape, int winding, int *statusptr, ImgMPData *mpdata);
	int ShapeFill(Shape *shape, int winding = -1, int *statusptr = NULL);
	int SimpleShapeFill(Shape *shape, bool invert, int winding, int *statusptr, ImgMPData *mpdata);
	int SimpleShapeFill(Shape *shape, bool invert = FALSE, int winding = -1, int *statusptr = NULL);

	void CopyToAlpha();
	void PutToImage(CombineMode mode, ChannelStyle &style);
};

void CombineImages(Image *imageOut, Image *imageOver, ImagePixelRegion region, CombineMode mode, ChannelStyle &style);

void CombineImages_Bitmap(Image *imageOut, Image *imageOver, Image *imageColor, SuperSamplingType sample, EdgeModes edges,
	const Matrix4 &transform, ImagePixelRegion region, CombineMode mode, ChannelStyle &style);

#endif // _IMAGECHANNEL_H_
