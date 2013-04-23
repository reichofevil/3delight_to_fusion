#ifndef _FuPlugin_h_
#define _FuPlugin_h_

#define _SECURE_SCL 0	// speeds up the STL - if plugins don't define this they'll disagree with fusion on the size/alignment of std::vector<> objects

#undef COMPANY_ID
#undef COMPANY_ID_DOT
#undef COMPANY_ID_SPACE
#undef COMPANY_ID_DBS

#define COMPANY_ID "RoE" // this should be changed to be your company name

#ifdef COMPANY_ID
#define COMPANY_ID_DOT   COMPANY_ID "."
#define COMPANY_ID_SPACE COMPANY_ID " "
#define COMPANY_ID_DBS   COMPANY_ID "\\"
#endif

#ifndef COMPANY_ID
#error "You must uncomment your company ID in FuPlugIn.h"
#endif

#ifdef _TYPES_H_
#error "You must include FuPlugIn.h before any other Fusion header files"
#endif

#ifndef FuPLUGIN
#define FuPLUGIN
#endif

//#define FILM_VERSION
#define RELEASE_VERSION

#include "ThreadedOperator.h"
#include "PipelineDoc.h"
#include "Pipeline.h"
#include "OperatorControl.h"
#include "SourceOperator.h"

extern FuSYSTEM_API FusionApp *App;

#endif // _FuPlugin_h_
