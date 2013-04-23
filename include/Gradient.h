#ifndef _GRADIENT_H_
#define _GRADIENT_H_

#include "types.h"
#include "Pixel.h"
#include "Parameter.h"
#include "Input.h"

#define GRADIENT_ID zMAKEID('Grad')
//#define CLSID_DataType_Gradient GRADIENT_ID

#define MAX_GRADIENT_CHANNELS 4

enum GradientPreset
{
	GP_None,
	GP_SolidBlack,
	GP_SolidWhite,
	GP_BlackToWhite,
	GP_WhiteToBlack,
	GP_BlackToTransparant,
	GP_BlackToTransparent = GP_BlackToTransparant,
	GP_WhiteToTransparant,
	GP_WhiteToTransparent = GP_WhiteToTransparant,
};

class FuSYSTEM_API GradientColor
{
	float64 Channels[MAX_GRADIENT_CHANNELS];
	Pixel Pix, PixConverted;
	FltPixel FltPix, FltPixConverted;
public :
	float64 Position;

	GradientColor *Next;

	GradientColor(float64 position = 0.0, float64 r = 0.0, float64 g = 0.0, float64 b = 0.0, float64 a = 0.0, GradientColor *next = NULL);
	GradientColor(GradientColor *color);

	void GetColor(float64 &r, float64 &g, float64 &b, float64 &a);
	void GetColor(Pixel &p);
	void SetColor(float64 r, float64 g, float64 b, float64 a);
	float64 GetChannel(int index);

	void ConvertColorSpace(ColorSpaceType space);

	void InterpolateWith(GradientColor *color, float64 weight, float64 &r, float64 &g, float64 &b, float64 &a, ColorSpaceType space = CS_RGB, bool cyclicR = FALSE, bool cyclicG = FALSE, bool cyclicB = FALSE , bool alreadyconverted = FALSE);
	void InterpolateWith(GradientColor *color, float64 weight, Pixel &p, ColorSpaceType space = CS_RGB, bool cyclicR = FALSE, bool cyclicG = FALSE, bool cyclicB = FALSE , bool alreadyconverted = FALSE);
};

class Gradient;

typedef void (Gradient::*GRADFPTR)(float64, float64 &, float64 &, float64 &, float64 &, ColorSpaceType);
typedef void (Gradient::*GRADPIXFPTR)(float64, Pixel &, ColorSpaceType);

class FuSYSTEM_API Gradient : public Parameter
{
	FuDeclareClass(Gradient, Parameter); // declare ClassID static member, etc.

private :
	uint32 Count;
	int32 SelectedColor;
	GradientColor *Colors, *ColorsLast;

	GradientColor *Table[8];
	bool TableCyclicR, TableCyclicG, TableCyclicB;
	GradientColor *SetTable(ColorSpaceType space);

	int32 spin;

	GRADFPTR GradFunc[8];
	GRADPIXFPTR GradPixFunc[8];

	void InternalSetTable(float64 position, float64 &r, float64 &g, float64 &b, float64 &a, ColorSpaceType space);
	void InternalSetTablePix(float64 position, Pixel &p, ColorSpaceType space);
	void InternalQuickEvaluate(float64 position, float64 &r, float64 &g, float64 &b, float64 &a, ColorSpaceType space);
	void InternalQuickEvaluatePix(float64 position, Pixel &p, ColorSpaceType space);

public:
	Gradient(GradientPreset preset = GP_None);
	Gradient(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Gradient(Gradient &param);
	Gradient(const int32 numcolors, const float64 *colors);
	virtual ~Gradient();

	static const Gradient &Get(Input *in, Request *req) { return *(Gradient *) in->GetValue(req); }

	virtual Parameter *InterpolateWith(double weight, Parameter *rightvalue, TagList *tags = NULL);

	void Clear();
	void ClearTables();

	virtual bool Save(ScriptVal &table, const TagList &tags);
	virtual Parameter *Copy();

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	void AddColor(GradientColor *color);
	void AddColor(float64 position, float64 r, float64 g, float64 b, float64 a);
	void AddColors(const int32 numcolors, const float64 *colors);
	Gradient *AddAtPosition(float64 position, ColorSpaceType space = CS_RGB);
	Gradient *DeleteAt(int32 selectedColor, int32 requiredColors);

	void FindNeighboors(float64 position, GradientColor *&largestSmaller, GradientColor *&smallestLarger);
	void Evaluate(float64 position, float64 &r, float64 &g, float64 &b, float64 &a, ColorSpaceType space = CS_RGB);
	void Evaluate(float64 position, Pixel &p, ColorSpaceType space = CS_RGB);
	void QuickEvaluate(float64 position, float64 &r, float64 &g, float64 &b, float64 &a, ColorSpaceType space = CS_RGB); //only usefull if evaluating multiple positions
	void QuickEvaluate(float64 position, Pixel &p, ColorSpaceType space = CS_RGB); //only usefull if evaluating multiple positions

	Gradient *SetPosition(int32 index, float64 position);
	Gradient *SetColor(int32 index, float64 r, float64 g, float64 b, float64 a = 1.0);
	Gradient *SetPositionColor(int32 index, float64 position, float64 r, float64 g, float64 b, float64 a = 1.0);

	void SetSelection(int32 index) {SelectedColor = index;};
	int32 GetSelection() { return Count > 0 ? SelectedColor : -1;};
	int32 GetSelectionBefore();
	int32 GetSelectionAfter();

	GradientColor *GetSelectedColor() { return GetColor(SelectedColor); };
	Gradient *SetSelectedPosition(float64 position) { return SetPosition(SelectedColor, position); };
	Gradient *SetSelectedColor(float64 r, float64 g, float64 b, float64 a = 1.0) { return SetColor(SelectedColor, r, g, b, a); };
	Gradient *SetSelectedPositionColor(float64 position, float64 r, float64 g, float64 b, float64 a = 1.0) { return SetPositionColor(SelectedColor, position, r, g, b, a); };

	int32 GetColorCount();
	GradientColor *GetColor(int32 index); 
	GradientColor *GetFirstColor();

	void SetPreset(GradientPreset preset);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Gradient);
#endif

protected:
	virtual bool _IsValid();
};


#endif
