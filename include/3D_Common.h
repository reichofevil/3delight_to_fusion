#ifndef _3D_COMMON_H_
#define _3D_COMMON_H_

#define SMALL_NUMBER 0.0001
#define LARGE_NUMBER 1e30

#define COLOR_LIMIT(v) \
	if (v < 0.0) \
		v = 0.0; \
	else if (v > 1.0) \
		v = 1.0;

#endif
