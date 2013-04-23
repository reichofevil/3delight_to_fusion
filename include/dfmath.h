#if (defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
  #if defined(__ICC)
  #define ICC_VERSION	__ICC
  #endif

  #if defined(__ICL)
  #define ICC_VERSION	__ICL
  #endif

  #if defined(__ECC)
  #define ICC_VERSION	__ECC
  #endif

  #if defined(__ECL)
  #define ICC_VERSION	__ECL
  #endif

  #if ICC_VERSION >= 700
    #ifdef _DLL
    #define _TEMP_DLL
    #undef _DLL
    #endif

    #include <mathimf.h>

    #if defined(__cplusplus)    // C linkage
    extern "C" {
    #endif

    #ifndef _CRT_ABS_DEFINED
    #define _CRT_ABS_DEFINED
      int       __cdecl abs(int _X);
      long      __cdecl labs(long _X);
      long long __cdecl llabs(long long _X);
    #endif

    #if defined(__cplusplus)    // end C linkage
    }
    #endif

    #ifdef _TEMP_DLL
    #define _DLL
    #undef _TEMP_DLL
    #endif
  #else
    // Nearest integer, absolute value, etc.
    #define ceilf		msceilf
    #define fabsf		msfabsf
    #define floorf		msfloorf
    #define fmodf		msfmodf
    #define rintf		msrintf
    #define hypotf		mshypotf

    // Power functions
    #define powf		mspowf
    #define sqrtf		mssqrtf

    // Exponential and logarithmic functions
    #define expf		msexpf
    #define logf		mslogf
    #define log10f		mslog10f

    // Trigonometric functions
    #define acosf		msacosf
    #define asinf		msasinf
    #define atanf		msatanf
    #define cosf		mscosf
    #define sinf		mssinf
    #define tanf		mstanf

    // Hyperbolic functions
    #define coshf		mscoshf
    #define sinhf		mssinhf
    #define tanhf		mstanhf

    #include <math.h>

    // Nearest integer, absolute value, etc.
    #undef ceilf
    #undef fabsf
    #undef floorf
    #undef fmodf
    #undef rintf
    #undef hypotf

    // Power functions
    #undef powf
    #undef sqrtf

    // Exponential and logarithmic functions
    #undef expf
    #undef logf
    #undef log10f

    // Trigonometric functions
    #undef acosf
    #undef asinf
    #undef atanf
    #undef cosf
    #undef sinf
    #undef tanf

    // Hyperbolic functions
    #undef coshf
    #undef sinhf
    #undef tanhf

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <mathf.h>

    #ifdef __cplusplus
    }
    #endif
  #endif
#else
  #include <math.h>

  #if !defined(acosf) && (!defined(__cplusplus) || defined(WINCE))
    #define acosf(x)    ((float)acos((double)(x)))
    #define asinf(x)    ((float)asin((double)(x)))
    #define atanf(x)    ((float)atan((double)(x)))
    #define atan2f(x,y) ((float)atan2((double)(x), (double)(y)))
    #define _cabsf      _cabs
    #define ceilf(x)    ((float)ceil((double)(x)))
    #define cosf(x)     ((float)cos((double)(x)))
    #define coshf(x)    ((float)cosh((double)(x)))
    #define expf(x)     ((float)exp((double)(x)))
    #define fabsf(x)    ((float)fabs((double)(x)))
    #define floorf(x)   ((float)floor((double)(x)))
    #define fmodf(x,y)  ((float)fmod((double)(x), (double)(y)))
    #define frexpf(x,y) ((float)frexp((double)(x), (y)))
    #define _hypotf(x,y)    ((float)_hypot((double)(x), (double)(y)))
    #define ldexpf(x,y) ((float)ldexp((double)(x), (y)))
    #define logf(x)     ((float)log((double)(x)))
    #define log10f(x)   ((float)log10((double)(x)))
    #define _matherrf   _matherr
    //#define modff(x,y)  ((float)modf((double)(x), (double *)(y)))
    #define powf(x,y)   ((float)pow((double)(x), (double)(y)))
    #define sinf(x)     ((float)sin((double)(x)))
    #define sinhf(x)    ((float)sinh((double)(x)))
    #define sqrtf(x)    ((float)sqrt((double)(x)))
    #define tanf(x)     ((float)tan((double)(x)))
    #define tanhf(x)    ((float)tanh((double)(x)))

    #define _copysignf(x,y)	((float)_copysign((double)(x), (double)(y)))
  #elif !defined(_M_AMD64)
    #define _copysignf(x,y)	((float)_copysign((double)(x), (double)(y)))
  #endif

  #define copysignf	_copysignf
#endif

//#ifdef __ICC
//#include </usr/include/math.h>
//#else
//#include <math.h>
//#endif
