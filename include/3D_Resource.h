#ifndef _3D_RESOURCE_H_
#define _3D_RESOURCE_H_

#include "Image.h"
#include "GLExtensions.h"
#include "RegistryLists.h"
#include "LockableObject.h"

#include "3D_DataTypes.h"



#define RESOURCE_BASE	      (OBJECT_BASE   + TAGOFFSET)
#define TEXTURE_RESOURCE_BASE	(RESOURCE_BASE + TAGOFFSET)
#define VERTEX_RESOURCE_BASE	(RESOURCE_BASE + TAGOFFSET)
#define SHADER_RESOURCE_BASE	(RESOURCE_BASE + TAGOFFSET)


enum Resource_Tags
{
	RM_BaseIntTag = TAG_INT + RESOURCE_BASE,	
	RM_RegisterWithManager,				 	// allow the manager to control this resource - defaults to true

	RM_CanReleaseOutsideActivate,			// the manager is allowed to free this resource outside of Activate/Deactivate - defaults to true
	RM_CanReleaseOutsidePreRender,		// the manager is allowed to free this resource outside of PreRender/PostRender - defaults to true
	RM_CanReleaseOutsideMapIn,				// the manager is allowed to free this resource outside of MapIn/MapOut - defaults to true

	RM_AlwaysReleaseOutsideActivate,		// the manager will always release this resource outside of Activate/Deactivate - defaults to false
	RM_AlwaysReleaseOutsidePreRender,	// the manager will always release this resource outside of PreRender/PostRender - defaults to false
	RM_AlwaysReleaseOutsideMapIn,			// the manager will always release this resource outside of MapIn/MapOut - defaults to true

	// When the RM_ActivateResource tag (with its value set to TRUE) passed to Create()/Upload(), if the resource upload is successful, 
	// it will be immediately activated.  If you are uploading a texture and don't pass this tag, you should be aware the ResourceMgr 
	// considers any unactivated resources as candidates for being mapped out to free memory.  Instead of passing this tag, you could 
	// optionally Activate() the resource before uploading it and then deactivate it after if the upload failed.
	RM_PreRenderResource,
	RM_ActivateResource,
};






enum ResourceType3D
{
	RT_StreamResource = 0,					// Stream3D
	RT_TextureResource,						// textures - 1D/2D/3D/cube
	RT_IndexResource,							// IndexArray3D
	RT_ShaderResource,
	RT_DisplayListResource,
	RT_OffscreenBufferResource,
	RT_DataResource,							// an object that inherits from Data3D
	RT_UserResource,							// unknown objects - sometimes you just want to free up some GPU memory to do something kinky
	RT_NumResourceTypes
};





class TagList;
class RenderContext3D;
class ResourceManager3D;




// An object that gets cached or otherwise has a non-local lifetime that can also use GPU memory resources should 
// inherit from a Resource3D object.  Resource3D is a very low level class that directly encapsulates an object used by the 
// rendering API (ie. DirectX/OpenGL).  If you want to write a plugin texture (for example), you would subclass TexBaseInputs3D/
// TexBaseData3D/TexBaseImplGL3D/TexBaseImplSW3D and not touch this class.  Resource3D should only be subclassed for API objects,
// such as shaders/textures/vertex arrays/display lists (and most of these already exist, so there should be no reason ever to subclass 
// this class).
class FuSYSTEM_API Resource3D : public Object
{
	FuDeclareClass(Resource3D, Object);

public:
	enum 
	{
		// These are hints you can provide to the resource manager
		SILENTLY_FREE_WHEN_ORPHANED			= 0x00000001,		// the ResourceMgr will silently free this resource even though it thinks it is orphaned
		//DO_NOT_FREE									= 0x00000010,		// the resource manager won't try to free this resource ever, until the actual resource is destroyed
		//DO_NOT_FREE_WHILE_PRERENDERED			= 0x00000020,		// the resource manager won't try to free this resource when it has a prerender count on it

		FREE_WHEN_DEACTIVATED					= 0x00000080,		// the resource manager will try to free this resource when its activate count reaches 0
		FREE_WHEN_POSTRENDERED					= 0x00000100,		// the resource manager will try to free this resource when its prerender count reaches 0

		NOTIFY_ON_PRERENDER						= 0x00000400,		// when a resource is registered, the resource manager sets these flags
		NOTIFY_ON_POSTRENDER						= 0x00000800,		// to let the resource know what notifications it wants to receive
		NOTIFY_ON_ACTIVATE						= 0x00001000,
		NOTIFY_ON_DEACTIVATE						= 0x00002000,
		NOTIFY_ON_PREALLOC						= 0x00004000,
		NOTIFY_ON_POSTALLOC						= 0x00008000,
		//NOTIFY_ON_DESTRUCTION					= 0x00010000,		// when a resource objects gpu data is freed
		NOTIFY_MASK									= 0x0001fe00,

		INVALID_RESOURCEID						= 0x0fffffff,
	};

	uint32 ResourceFlags;

	uint32 ResourceID;								// used by the resource manager
	ResourceType3D Type;

	volatile bool vIsUploaded;						// locked by UploadLock
	LockableObject UploadLock;						// this gets locked while uploading (it can also be used when registering the resource which you will want to lock if multiple threads could be registering the same object simultaneously)

	volatile LONG PreRenderCount;
	volatile LONG ActivateCount;

	int32 GPUMemUsage;								// GPU memory usage in kilobytes, locked by UploadLock (needs to be signed)

	Resource3D *Next;									// used by the resource manager

	ResourceManager3D *MyManager;					// pointer to its manager in case there are two or more managers

	// TODO - some kind of priority for resource objects to determine the order in which they get freed

	uint8 pad_Resource[32];

protected:
	bool CtorInit();

public:
	Resource3D();
	Resource3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Resource3D(const Resource3D &toCopy);
	virtual ~Resource3D();

	//bool DoNotFree()						{ return (ResourceFlags & DO_NOT_FREE) != 0; }

	bool FreeWhenDeactivated()			{ return (ResourceFlags & FREE_WHEN_DEACTIVATED) != 0; }
	bool FreeWhenPostRendered()		{ return (ResourceFlags & FREE_WHEN_POSTRENDERED) != 0; }

	bool NotifyOnPreRender()	{ return (ResourceFlags & NOTIFY_ON_PRERENDER) != 0; }
	bool NotifyOnPostRender()	{ return (ResourceFlags & NOTIFY_ON_POSTRENDER) != 0; }
	bool NotifyOnActivate()		{ return (ResourceFlags & NOTIFY_ON_ACTIVATE) != 0; }
	bool NotifyOnDeactivate()	{ return (ResourceFlags & NOTIFY_ON_DEACTIVATE) != 0; }
	bool NotifyOnPreAlloc()		{ return (ResourceFlags & NOTIFY_ON_PREALLOC) != 0; }
	bool NotifyOnPostAlloc()	{ return (ResourceFlags & NOTIFY_ON_POSTALLOC) != 0; }
	//bool NotifyOnDestruction() { return (ResourceFlags & NOTIFY_ON_DESTRUCTION) != 0; }

	void SetNotifications(uint32 notifyFlags) { ResourceFlags &= ~NOTIFY_MASK; ResourceFlags |= notifyFlags; }

	// When the ResourceMgr is Shutdown() it searches to see if there are any registered resources, asserts for each registered resource, 
	// and frees them all.  At this point, the only unfreed resources should be those flagged as SILENTLY_FREE_WHEN_ORPHANED and resource 
	// leaks from bad plugin code.  If you have resource that likes to stick around a long time (perhaps in a class static variable), you 
	// can set it to SilentlyFreeWhenOrphaned() and the ResourceMgr will not assert/warn for this resource.
	void SetSilentlyFreeWhenOrphaned(bool freeSilently = true);
	bool SilentlyFreeWhenOrphaned() { return ResourceFlags & SILENTLY_FREE_WHEN_ORPHANED; }

	// These 4 methods are not intended to be overridden.  These methods are provided to allow the resource manager to do more 
	// intelligent resource management, eg. free resources that not prerendered before prerendered resources, free unactivated ones
	// before activated ones.  The renderer will call these functions on any vertex/shader resources it uses, materials must call
	// them on any resources they use.  (eg. if MySpecialMaterial uses two texture resource, whenever the material gets activated, 
	// it should also call Activate() on its two texture resources)
	void PreRender();	
	void Activate();	
	void Deactivate();	
	void PostRender();	

	// Note that by the time this function call returns, the resource may not be uploaded any more.  If the resource has been
	// activated, then you can be sure that the resource will not be unloaded.  So it is best to first Activate() the resource
	// and then see if it is uploaded and upload it if it isn't.
	virtual bool IsUploaded(TagList *tags = NULL);

	// Free's all GPU data associated with this resource
	virtual void FreeData(TagList *tags = NULL) {}

	// FreeTempData() will be called by the 3D framework.  You should only free up stuff that you know for sure nothing is actually using. There's a 
	// good chance that something might still be using textures.  
	virtual void FreeTempData(TagList *tags = NULL) {}

	// This can be called in low memory conditions, so it shouldn't have to allocate any heap memory in order to do its estimation.
	virtual uint32 EstimateMemoryUsage(TagList *tags = NULL) const;			// in kilobytes

	// Is it registered with the resource manager?
	virtual bool IsRegistered(TagList *tags = NULL);

	// Does various internal checking to see if this resource is valid and FuASSERT()s if it isn't
	virtual bool DebugIsValid(TagList *tags = NULL);
};





#endif