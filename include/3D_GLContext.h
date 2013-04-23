#ifndef _GL_CONTEXT3D_H_
#define _GL_CONTEXT3D_H_

#include "LockableObject.h"


#define OPENGL_RENDER_CONTEXT_BASE		(RENDER_CONTEXT_BASE + TAGOFFSET)


class FuSYSTEM_API GLContext3D : public Object
{
public:
	HDC   hDC, OldDC;				
	HGLRC hRC, OldRC;
	HWND  hWnd;								// Zero size dummy window

	LockableObject Lock;					// Synchronization object controlling access to this context - Use Obtain()/Release()

	int LockCount;							// allow for nested Obtain()s on the same thread

	FuID m_Name;							// this is purely to make debugging multithreaded/multicontexts easier

public:
	GLContext3D(const FuID &name);
	GLContext3D &operator=(const GLContext3D &context);			/// Unimplemented
	GLContext3D(const GLContext3D &toCopy);							/// Unimplemented
	~GLContext3D();

	const char *GetName();
	
	// OpenGL contexts require a DC and window to exist.  This method ecapsulates this creation process.  It creates a
	// dummy, zero size window and associated DC and RC.  You can optionally share the textures/display lists/VBOs on the
	// new context with the existing GLTools context.  The windowName must be unique.  Returns true on success, false on failure.
	bool Create(int width = 0, int height = 0, bool sharelists = true);

	/**
	 * Makes this context current to the calling thread.  The context can be current to only one thread at a time.  If for some 
	 * reason, Obtain() fails, it will attempt to restore the previous dc/context.  Supported nested locking.
	 */
	bool Obtain();

	/**
	 * Releases this context.  If restore is true, Release() attempts to restore the context that was current when Obtain() was
	 * previously called.
	 */
	void Release(bool restore = true);

	// Shares the supplied rc with this context.  This shares textures, VBOs, and display lists.  You should only share lists like this:
	//   GLTools.ShareLists(my_rendering_context);
	// All contexts share themselves with GLTool's HubContext, so by sharing your context with GLTools it should be shared
	// with all of them.  Note that my_rendering_context must be a newly created (empty) context when you do this.
	bool ShareLists(HGLRC rc);

	bool IsActive() { return wglGetCurrentContext() == hRC; }

	bool IsInitialized() { return hDC && hRC && hWnd; }			// for debugging

protected:
	friend class GLTools3D;

	void Free();

	static bool Init();
	static void Shutdown();

	static char *DummyWindowClassName;				// Class name to be shared among all windows
};



//
//class FuSYSTEM_API AutoGLContext
//{
//public:
//	GLContext3D *Context;
//
//public:
//	AutoGLContext(GLContext3D *context, Object *obj) 	{ Context = ResourceMgr.ObtainCgContext(); }
//	~AutoGLContext()		{ if (Context) ResourceMgr.ReleaseCgContext(Context); }
//	bool IsLocked()		{ return Context != NULL; }
//};
//
//




#endif