#ifndef _RESCOLOR_H_
#define _RESCOLOR_H_

#include "LookPack.h"

#define LP_SYSCOL(a)          ((COLORREF)(((DWORD)(BYTE)(a))<<24))

class FuSYSTEM_API ResColor : public LookPackItem
{
	FuDeclareClass(ResColor, LookPackItem); // declare ClassID static member, etc.

protected:
	COLORREF ForeColor;
	HBRUSH ForeBrush;

	COLORREF BackColor;
	HBRUSH BackBrush;

	COLORREF DefForeColor;
	COLORREF DefBackColor;

	void Init(uint32 forecol, uint32 backcol);

public:
	ResColor(char *name);
	ResColor(char *name, uint32 forecol, uint32 backcol = 0);
	ResColor(uint32 forecol, uint32 backcol = 0);
	virtual ~ResColor();

	virtual bool Load(void);
	virtual void Release(void);

	COLORREF GetForeColor(void) { if (!IsLoaded()) Load(); return ForeColor; }
	HBRUSH GetForeBrush(void) { if (!IsLoaded()) Load(); return ForeBrush; }

	COLORREF GetBackColor(void) { if (!IsLoaded()) Load(); return BackColor; }
	HBRUSH GetBackBrush(void) { if (!IsLoaded()) Load(); return BackBrush; }

	void AdjustHLS(float32 hshift, float32 lscale, float32 sscale);
	void AdjustHLS_Fore(float32 hshift, float32 lscale, float32 sscale);
	void AdjustHLS_Back(float32 hshift, float32 lscale, float32 sscale);
	static COLORREF AdjustHLS_Col(COLORREF col, float32 hshift, float32 lscale, float32 sscale);

	void AdjustGain(float32 gainRed, float32 gainGreen, float32 gainBlue);
	void AdjustGain_Fore(float32 gainRed, float32 gainGreen, float32 gainBlue);
	void AdjustGain_Back(float32 gainRed, float32 gainGreen, float32 gainBlue);
	static COLORREF AdjustGain_Col(COLORREF col, float32 gain);
	static COLORREF AdjustGain_Col(COLORREF col, float32 gainRed, float32 gainGreen, float32 gainBlue);

	void AdjustOffset(float32 offsetRed, float32 offsetGreen, float32 offsetBlue);
	void AdjustOffset_Fore(float32 offsetRed, float32 offsetGreen, float32 offsetBlue);
	void AdjustOffset_Back(float32 offsetRed, float32 offsetGreen, float32 offsetBlue);
	static COLORREF AdjustOffset_Col(COLORREF col, float32 offset);
	static COLORREF AdjustOffset_Col(COLORREF col, float32 offsetRed, float32 offsetGreen, float32 offsetBlue);

	void Lerp(ResColor &col, float32 amount);
	void Lerp(COLORREF col, float32 amount);
	void Lerp_Fore(COLORREF col, float32 amount);
	void Lerp_Back(COLORREF col, float32 amount);
	static COLORREF Lerp_Col(COLORREF origcol, COLORREF col, float32 amount);

	float32 GetForeLuma(void);
	float32 GetBackLuma(void);
	static float32 GetLuma_Col(COLORREF col);
};

#endif // _RESCOLOR_H_
