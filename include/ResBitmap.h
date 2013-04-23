#ifndef _RESBITMAP_H_
#define _RESBITMAP_H_

#include "LookPack.h"
#include "Pixel.h"

class Image;

class FuSYSTEM_API ResBitmap : public LookPackItem
{
	FuDeclareClass(ResBitmap, LookPackItem); // declare ClassID static member, etc.

protected:
	uint32 ResID;
	CDC DC;
	HBITMAP Bitmap;
	HBITMAP OldBM;
	int32 Width, Height;
	HINSTANCE InstanceHandle;

	bool IsDC;

	void Init(uint32 id, const Registry *reg);

public:
	ResBitmap(char *name);
	ResBitmap(uint32 id, const Registry *reg = NULL);
	ResBitmap(uint32 width, uint32 height);
	virtual ~ResBitmap();

	virtual bool Load(void);
	virtual void Release(void);

	HBITMAP GetBitmap(void) { if (Bitmap == NULL) Load(); return Bitmap;              }
	CDC    *GetDC(void)     { if (Bitmap == NULL) Load(); return (Bitmap ? &DC : NULL); }
	int32   GetWidth(void)  { if (Bitmap == NULL) Load(); return Width;  }
	int32   GetHeight(void) { if (Bitmap == NULL) Load(); return Height; }

	void SelectDC(void)     { if (Bitmap == NULL) Load(); if (Bitmap && !IsDC) { IsDC = TRUE; OldBM = (HBITMAP) SelectObject(DC.m_hDC, Bitmap); } }
	void SelectBitmap(void) { if (Bitmap == NULL) Load(); if (Bitmap && IsDC) { IsDC = FALSE; SelectObject(DC.m_hDC, OldBM); OldBM = NULL;      } }
};

#define FULL_WIDTH -1
#define FULL_HEIGHT -1

#define COPYTYPE_FullColor 0
#define COPYTYPE_Red 1
#define COPYTYPE_Green 2
#define COPYTYPE_Blue 3
#define COPYTYPE_Alpha 4
#define COPYTYPE_Luminance 5

class FuSYSTEM_API DIB : public LookPackItem
{
	FuDeclareClass(DIB, LookPackItem); // declare ClassID static member, etc.

protected:
	HBITMAP DIBHandle;
	void *Data;
	int32 Width, Height;
	bool Loaded;

	bool IsSafeAddress(uint8 *addr) { return (addr >= (uint8 *)GetData()) && (addr < (uint8 *)GetData() + ((Width*Height) << 2));};
	uint8 *PixAddress(int32 x, int32 y) { return (uint8 *) ((uint8 *)GetData() + ((y * Width + x) << 2));};

	bool AntiAlias;
	int32 PreviousX, PreviousY;

	Pixel32 PenColor;
	int32 PenOpacity;
	float32 PenThickness;
	CRect clip;

	void Init(void);
	bool InitBitmap(int32 w, int32 h);

	Pixel32 GetPixel(uint8 *addr);
	Pixel32 GetPixel(int32 x, int32 y) { return GetPixel(PixAddress(x, y)); };
	void SetPixel(uint8 *addr, Pixel32 &pix);
	void SetPixel(int32 x, int32 y, Pixel32 &pix) { SetPixel(PixAddress(x, y), pix); };
	void BlendPixel(uint8 *addr, Pixel32 &pix, uint16 blend);
	void MergePixel(uint8 *addr, int32 x1, int32 y1, Pixel32 &pix, uint16 blend);

	void LineTo_NoAA(int32 x, int32 y);
	void LineTo_AA(int32 x, int32 y);

public:
	DIB();
	DIB(char *name);
	DIB(int32 w, int32 h);
	DIB(DIB *src);
#ifndef	DFMANAGER
	DIB(Image *image, int32 type = COPYTYPE_FullColor, bool alphasolid = FALSE);
#endif
	virtual ~DIB();

	virtual bool Load(void);
	virtual void Release(void);

	void Clear(void);
	void Bevel(RECT *rect, int thickness = 2);
	void Bevel(RECT *rect, COLORREF color1, COLORREF color2, int thickness = 1);
	void Copy(DIB *src);
	void Merge(DIB *src, int32 x, int32 y);
	void Merge(DIB *src, int32 x, int32 y, int32 sx, int32 sy, int32 sw, int32 sh);
	void Resize(DIB *src);
	void Fill(COLORREF rgb);
	void Fill(int32 r, int32 g, int32 b, int32 a);
	void Fill(int32 r, int32 g, int32 b, int32 a, int32 left, int32 top, int32 right, int32 bottom);
	void Fill(int32 r, int32 g, int32 b, int32 a, CRect &rect) { Fill(r, g, b, a, rect.left, rect.top, rect.right, rect.bottom); };
	void Fill(DIB *bm);
	void Fill(DIB *bm, int32 sx, int32 sy);
	void FillBordered(DIB *border, int32 left, int32 top, int32 right, int32 bottom);
	void FillBordered(DIB *border, CRect &rect) { FillBordered(border, rect.left, rect.top, rect.right, rect.bottom); };
	void FillStretched(DIB *source);
	void Multiply(DIB *src, int32 x, int32 y);
	void Multiply(DIB *src, int32 x, int32 y, int32 sx, int32 sy, int32 sw, int32 sh);
	void PreDivide(void);
	void PostMultiply(void);

	void   *GetData(void)   { if (!Loaded) Load(); return Data;   }
	int32   GetWidth(void)  { if (!Loaded) Load(); return Width;  }
	int32   GetHeight(void) { if (!Loaded) Load(); return Height; }
	HBITMAP GetHandle(void) { if (!Loaded) Load(); return DIBHandle; }

	void Merge(HDC dc, int32 x = 0, int32 y = 0, int32 w = FULL_WIDTH, int32 h = FULL_HEIGHT);
	void Draw(HDC dc, int32 x = 0, int32 y = 0, int32 w = FULL_WIDTH, int32 h = FULL_HEIGHT);

	void AdjustHLS(float32 hshift, float32 lscale, float32 sscale);
	void AdjustHLS(DIB *mask, float32 hshift, float32 lscale, float32 sscale);
	void AdjustGain(float32 gainRed, float32 gainGreen, float32 gainBlue, float32 gainAlpha);
	void AdjustGain(DIB *mask, float32 gainRed, float32 gainGreen, float32 gainBlue, float32 gainAlpha);

	void SetAntiAlias(bool aa = TRUE);
	void SetPenOpacity(float32 opacity = 1.0f);
	void SetPenColor(float32 r, float32 g, float32 b, float32 a = 1.0f);
	void SetPenColor(COLORREF rgb, int32 a = 255);
	void SetPenThickness(float32 thickness = 1.0f);

	void SetClip(CRect &rect);
	void ClearClip();
	CRect &GetClip() { return clip; }

	// Using current pen color
	void MoveTo(int32 x, int32 y);
	void LineTo(int32 x, int32 y);
	void SetPixel(int32 x, int32 y);
};

class FuSYSTEM_API ResDIB : public DIB
{
	FuDeclareClass(ResDIB, DIB); // declare ClassID static member, etc.

protected:
	uint32 ResID;
	HINSTANCE InstanceHandle;

public:
	ResDIB(char *name);
	ResDIB(uint32 id, const Registry *reg = NULL);
	virtual ~ResDIB();

	virtual bool Load(void);
	virtual void Release(void);
};

#endif // _RESBITMAP_H_
