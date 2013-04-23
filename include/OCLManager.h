#ifndef _OCL_MANAGER_H_
#define _OCL_MANAGER_H_

#include "Object.h"
#include "VectorMath.h"
#include "DFRect.h"
#include <vector>

// These are from cl.h
struct _cl_platform_id;
struct _cl_device_id;
struct _cl_context;
struct _cl_command_queue;
struct _cl_mem;
struct _cl_program;
struct _cl_kernel;
struct _cl_event;
struct _cl_sampler;

typedef struct _cl_platform_id *    cl_platform_id;
typedef struct _cl_device_id *      cl_device_id;
typedef struct _cl_context *        cl_context;
typedef struct _cl_command_queue *  cl_command_queue;
typedef struct _cl_mem *            cl_mem;
typedef struct _cl_program *        cl_program;
typedef struct _cl_kernel *         cl_kernel;
typedef struct _cl_event *          cl_event;
typedef struct _cl_sampler *        cl_sampler;

class DIB;
class OCLManager;
class OCLProgram;

typedef int16 OCLKernel;		// simple index
//typedef void *OCLMemory;		// OpenCL memory buffer

// SetArg|Buffer() flags
#define OCLSETF_None			0
#define OCLSETF_UseGL		(1UL << 0)
#define OCLSETF_UseMem		(1UL << 1)
#define OCLSETF_Readable	(1UL << 2)
#define OCLSETF_Writable	(1UL << 3)

// OCLMemory flags
#define OCLMEMF_Buffer		(1UL << 4)
#define OCLMEMF_Image		(1UL << 5)



class FuSYSTEM_API OCLMemory : public LockableObject
{
	FuDeclareClass(OCLMemory, LockableObject); // declare ClassID static member, etc.

protected:
	cl_mem mem;
	uint32 flags;

	OCLProgram *prog;		// for device queue access

public:
	OCLMemory(cl_mem _mem, uint32 _flags, OCLProgram *_prog);
	virtual ~OCLMemory();

	uint32 GetFlags();
//	OCLMemory *Copy();
	bool Release();

	OCLMemory *Blur(float blurx, float blury, OCLMemory *tempcl, OCLMemory *outcl);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(OCLMemory);
#endif

	friend class OCLProgram;
};


class FuSYSTEM_API OCLProgram : public LockableObject
{
	FuDeclareClass(OCLProgram, LockableObject); // declare ClassID static member, etc.

	OCLManager *m_Manager;
	int m_DeviceNum;
	cl_command_queue m_Queue;

	const char *m_Source;
	size_t m_SourceLen;
	CString m_BuildOptions;

	int m_SizeX, m_SizeY;
	int m_WorkgroupSizeX, m_WorkgroupSizeY;

	cl_program m_Program;

	std::vector<FuID> m_KernelNames;
	std::vector<cl_kernel> m_Kernels;

	HANDLE m_BuildEvent;

	int64 m_PerfTimer, m_PerfTime;

public:
	enum RunFlags { Block, NoBlock };

	CString m_CachePath;

protected:
	static DWORD BuildThread(void *data);
	static void BuildNotify(cl_program prog, void *data);

public:
	OCLProgram(OCLManager *mgr, int devnum, const char *source, size_t len = 0);
	OCLProgram(OCLManager *mgr, int devnum, const uint8 *binary, size_t len);
	virtual ~OCLProgram();

	bool Build(bool wait = true, const char *options = NULL);
	bool WaitForBuild(uint32 timeout = INFINITE);

	uint8 *GetBinary(size_t& len);
	bool WriteBinaryTo(const char *cachepath);

	int CreateAllKernels();
	OCLKernel CreateKernel(const char *name);
	OCLKernel GetKernelIndex(const char *name);
	void SetSize(int xsize, int ysize = -1);
	void SetWorkgroupSize(int xsize = -1, int ysize = -1);			// 0 is NULL (default, impl-defined), -1 is Auto (Fusion-defined), >0 is manual

	bool RunKernel(OCLKernel index, RunFlags flags = Block);

	void SetArg(OCLKernel kernel, uint16 arg, bool val);
	void SetArg(OCLKernel kernel, uint16 arg, int val);
	void SetArg(OCLKernel kernel, uint16 arg, float val);
	void SetArg(OCLKernel kernel, uint16 arg, int val1, int val2);
	void SetArg(OCLKernel kernel, uint16 arg, float val1, float val2);
	void SetArg(OCLKernel kernel, uint16 arg, int val1, int val2, int val3);
	void SetArg(OCLKernel kernel, uint16 arg, float val1, float val2, float val3);
	void SetArg(OCLKernel kernel, uint16 arg, int val1, int val2, int val3, int val4);
	void SetArg(OCLKernel kernel, uint16 arg, float val1, float val2, float val3, float val4);
	void SetArg(OCLKernel kernel, uint16 arg, const Vector2f &vec);
	void SetArg(OCLKernel kernel, uint16 arg, const Vector4f &vec);
	void SetArg(OCLKernel kernel, uint16 arg, const Matrix4f &mat);
	void SetArg(OCLKernel kernel, uint16 arg, const FuRectInt &window);
	void SetArg(OCLKernel kernel, uint16 arg, OCLMemory *mem);
	void SetArgBufferSize(OCLKernel kernel, uint16 arg, size_t size);				// use for buffer args with nothing to be uploaded (NULL buffer)
	void SetArgDataWindow(OCLKernel kernel, uint16 arg, Image *img);				// sets img datawindow as int4(offx, offy, width, height)

	OCLMemory *CreateImage(Image *img, uint32 flags = OCLSETF_UseMem | OCLSETF_Readable);
	OCLMemory *CreateImage(DIB *dib, uint32 flags = OCLSETF_UseMem | OCLSETF_Readable);
	bool Download(OCLMemory *mem, Image *img);
	bool Download(OCLMemory *mem, DIB *dib);
	bool Copy(OCLMemory *src, OCLMemory *dst, int w, int h);

	OCLMemory *CreateBuffer(size_t size, void *ptr, uint32 flags = OCLSETF_UseMem | OCLSETF_Readable);	// OCLSETF_UseMem is implied
	OCLMemory *CreateBuffer(Image *img, uint32 flags = OCLSETF_UseMem | OCLSETF_Readable);
	OCLMemory *CreateBuffer(DIB *dib, uint32 flags = OCLSETF_UseMem | OCLSETF_Readable);
	bool Download(OCLMemory *mem, size_t size, void *ptr);

	bool ReleaseGLObject(OCLMemory *mem);
	bool ReleaseMemory(OCLMemory *mem);

	void LockDeviceQueue();
	void ReleaseDeviceQueue();

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(OCLProgram);
#endif
};



class FuSYSTEM_API OCLManager : public LockableObject
{
	FuDeclareClass(OCLManager, LockableObject); // declare ClassID static member, etc.

protected:
	cl_context m_Context;
	cl_device_id m_Device;			// TODO: vector of devices
	cl_command_queue m_Queue;		// TODO: vector of devices

	FuLock m_DeviceLock;				// TODO: vector of devices
	OCLProgram *m_CommonProgram;

public:
	bool SupportsImage, GLSharing;
	bool FP16, FP64;

	FuID CurrentDevice;
	FuID m_PlatformName, m_DeviceName;

protected:
	static bool InitOpenCL(const char *dllpath = NULL);

public:
	OCLManager();
	virtual ~OCLManager();

	static void RegCleanup(Registry *regnode);

	bool Create(const char *device = NULL);
	OCLProgram *BuildCachedProgram(const Registry *reg, const char *source = NULL, size_t len = 0, const char *options = NULL);
	OCLProgram *BuildCachedProgram(const FuID& cacheid, const char *sourcefile, const char *source = NULL, size_t len = 0, const char *options = NULL);
	OCLProgram *BuildProgram(const char *source, size_t len = 0, const char *options = NULL);

	bool FlagRebuildContext();

	cl_context       GetContext();
	cl_device_id     GetDevice(int devicenum = 0);
	cl_command_queue GetDeviceQueue(int devicenum = 0);
	void ReleaseContext(cl_context context);
	void ReleaseDevice(cl_device_id device);
	void ReleaseDeviceQueue(cl_command_queue queue);

	bool GetDeviceAttr(int devicenum, const char *attrname, int retsize, void *ret);
	bool GetDeviceAttr(int devicenum, int attr, int retsize, void *ret);

	static OCLManager *GetManager(bool force = false);
	static bool DumpCLDeviceInfo(std::ostream &buffer);

	// performance testing
	static int64 GetTimer();
	static double CalcTiming(int64 starttime, int64 endtime = 0);				// set factor = 1.0 for seconds etc
	static void LogTiming(const char *msg, ...);	// msg should include %g msec specifier; set factor = 1.0 for seconds etc
	static void LogTiming(Image *img, const char *key, const char *format, ...);	// adds to metadata

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(OCLManager);
#endif
};



#endif