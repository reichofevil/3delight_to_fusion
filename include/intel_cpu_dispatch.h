#ifndef _INTEL_CPU_DISPATCH_H_
#define _INTEL_CPU_DISPATCH_H_

#if __INTEL_COMPILER >= 700 && defined(__AFXWIN_H__)

#include "Pipeline.h"

extern FuSYSTEM_API FusionApp *App;

extern "C" void __intel_cpu_indicator_init(void);
extern "C" uint32 __intel_cpu_indicator;

class ___IntelCPUDispatchHack
{
public:
	inline ___IntelCPUDispatchHack()
	{
		if (App && (App->GetAttrB(PA_IsSSE) || App->GetAttrB(PA_IsSSE2)))
		{
			__intel_cpu_indicator_init();
			if (__intel_cpu_indicator < 0x80)
				__intel_cpu_indicator = App->GetAttrB(PA_IsSSE2) ? 0x200 : (App->GetAttrB(PA_IsSSE) ? 0x80 : __intel_cpu_indicator);
		}
	}
};

static ___IntelCPUDispatchHack icdh;

#endif

#endif // _INTEL_CPU_DISPATCH_H_
