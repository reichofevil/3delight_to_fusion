#ifndef _MATERIALBASESWIMPL3D_H_
#define _MATERIALBASESWIMPL3D_H_

#include "3D_MtlGraph.h"
#include "3D_StreamSet.h"
#include "3D_DataTypes.h"
#include "3D_MaterialBase.h"

class GeomInfoSW;
class MtlImplSW3D;
class CompileBlock3D;
class ShadeContext3D;
class RenderContextSW3D;

typedef void (MtlImplSW3D::*ShadeFunc3D)(ShadeContext3D &sc);





// When the renderer calls Implement() this object should create a software renderer specific implementation of the passed in MtlData3D.
// Note that a overriden version should always call the BaseClass version to allow it to do any setup.  Any resources initialized in Implement() 
// should be cleaned up in DeImplement().  DeImplement() will always be called even if Implement() returns false.  DeImplement() is 
// the matching call to Implement() which allows the material implementation a chance to do any cleanup for items that may have been allocated 
// in Implement().  // MMTODO - Deimplement always called, but postrender/deactivate not?
//
// Everytime the renderer needs to use a material, it will make it the active material by calling Activate() on it.  Note that Activate()/
// Deactivate() will probably be called many times during a render pass.  All the Activate/Deactivate calls in a particular
// render pass will be bracketed by a PreRender/PostRender pair.  PostRender() is called after all fragment processing with this material
// on a given mesh is done. 
// 
// In the current implementation if Implement, ConnectInputs Activate, or PreRender fail a recoverable error is issued.  If the error
// callback opts to resume, the mesh will not get rendered, and the renderer will proceed to rendering the next mesh.
//
// Note that if PreRender() fails PostRender() will not get called, so if PreRender() is going to fail, it should also do any necessary
// cleanup.  The same holds with Activate()/Deactivate() -- if Activate() fails, Deactivate() will not get called.  If PreRender() fails
// on a material, Activate() will never be called.  Note that if even if Activate() fails, the framework is still responsible for calling
// PostRender().
//
// When overriding these functions there is no need to call the base class version.


/**
 * Base class for software renderer materials.
 * Key Overides:
 * - AddInputs()
 * - Shade()
 * - PreRender() / PostRender()
 * - Activate() / Deactivate()
 */
class FuSYSTEM_API MtlImplSW3D : public MtlImpl3D
{
	FuDeclareClass(MtlImplSW3D, MtlImpl3D);

public:
	ShadeFunc3D ShadeFunc;					/**< This is called to do all the work.  It must a valid pointer between Activate/Deactivate() calls.  It may be changed at any time (eg. in the ctor, in PreRender/Activate/ObjectChanged/etc). */

	GeomInfoSW *CurrentGeom;				// a hack to track ObjectChanged() notifications

	void **TLS;									/**< array of per thread TLS, indexed by sc.ThreadNumber */

	static const uint32 UnInitialized = 0xcdcdcdcd;

	uint8 m_MtlImplSWPad[16];

public:
	/**
	 * The constructor should be overridden to create child material implementations.   The MtlData3D and render context
	 * can be retrieved from the taglist like this:
	 *    \code
	 *		MtlBlinnData3D *mtlData = (MtlBlinnData3D *) tags.GetPtr(MTLP_MaterialData, NULL);
	 *		RenderContextSW3D &rc = *((RenderContextSW3D *) tags.GetPtr(MTLP_RenderContext, NULL));
	 *    \endcode
	 * The render context can be useful when deciding which child materials to create.  For example, if lighting is not
	 *	enabled, there would be no purpose in creating specular color or bumpmap child materials.  
	 * If the constructor fails an exception should be thrown (TODO - which exception?)
	 * @param tags contains pointers to the rendercontext and the associated MtlData3D.
	 */
	MtlImplSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);

	MtlImplSW3D(const MtlImplSW3D &toCopy);	/**< purposefully left unimplemented */

	/**
	 * Calls FreeChildMtls().
	 */
	~MtlImplSW3D();

	/**
	 * Creates an implementation of the supplied MtlData3D for use by the software renderer.  It does this by searching the class
	 * registry for a matching implementation by calling mtlData->GetImplRegistry().
	 * @return can return NULL if a matching implementation could not be found or throw an exception if the creation failed
	 * @see    MtlData3D::GetImplRegistry()
	 * @see    CreateChildMtl()
	 */
	static MtlImplSW3D *CreateMtl(RenderContextSW3D &rc, MtlData3D *mtlData); // throws CMemoryException 

	// If managed is set to true the created material is added to the ChildMtls list.   The default behaviour for managed materials 
	// is to call PreRender/Activate/Shade/Deactivate/PostRender on them.  If you self manage it yourself you must call those
	// functions manually.  If mtlData is NULL, NULL will be returned.
	virtual MtlImplSW3D *CreateChildMtl(RenderContextSW3D &rc, int slot); // throws CMemoryException
	virtual void FreeChildMtls(TagList *tags = NULL); // frees all managed mtls

	/**
	 * @return  allocate return using 'new'
	 */
	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc);

	virtual bool DoPreRender(RenderContextSW3D &rc);
	virtual void DoPostRender(RenderContextSW3D &rc);
	virtual bool DoActivate(RenderContextSW3D &rc);
	virtual void DoDeactivate(RenderContextSW3D &rc);
	virtual void DoObjectChanged(RenderContextSW3D &rc);

	virtual bool DoDebugIsValid(RenderContextSW3D &rc);

	/**
	 * Does per-render setup.  Overide PreRender() to do any per-render setup such as creating lookup tables.  All Activate() and 
	 * Deactivate() calls during a render will be bracketed by a PreRender/PostRender pair.  If PreRender() returns true then PostRender() 
	 * will be called.  However if PreRender() fails PostRender() will not get called, which means PreRender() needs to clean up on failure.
	 * @param rc   access global render state
	 * @param tags future API expandability
	 * @return     return false to indicate that the material failed to initialize correctly (render will fail)
	 * @see DoPreRender()
	 */
	virtual bool PreRender(RenderContextSW3D &rc);

	/**
	 * Does per-render cleanup.  PostRender() is the matching call to PreRender().
	 * @param rc   access global render state
	 * @param tags future API expandability
	 * @see			DoPostRender()
	 */
	virtual void PostRender(RenderContextSW3D &rc);

	/**
	 * Does per-use setup.  Overide Activate() to do any per-use initialization such as mapping textures into memory.  The software 
	 * renderer will only keep one or a limited number of materials active at a given time in order to reduce memory usage.  
	 * Activate/Deactivate can be called many times on a material during the course of a render.  If Activate() succeeds then 
	 * Deactivate() will be called.  If Activate() fails Deactivate() will not get called which means Activate() needs to clean up on failure.  
	 * @param rc   access global render state
	 * @param tags future API expandability
	 * @return     return false to indicate that the material failed to activate correctly (render will fail)
	 * @see			DoActivate()
	 */
	virtual bool Activate(RenderContextSW3D &rc);

	/**
	 * Does per-use cleanup.  Overide Deactivate() to do any cleanup for things done in Activate().
	 * @param rc   access global render state
	 * @param tags future API expandability
	 * @see DoDeactivate()
	 */
	virtual void Deactivate(RenderContextSW3D &rc);

	/**
	 * Computes the pixel color.  This is where the main work of the material gets done.  Shade() computes the per pixel outputs from
	 * your material.  Shade() can also output things like modified normals and texture coordinates.  Shade() must be thread safe as it can 
	 * be called by multiple threads at once.  This means you can read data from the class but must never write data into the class. 
	 * @param sc    global render state
	 * @param block access to inputs/outputs that were created in AddInputs()
	 */
	void Shade(ShadeContext3D &sc) { return ((*this).*ShadeFunc)(sc); }

	virtual void Transmit(ShadeContext3D &sc);

	/**
	 * Calls Shade() on all child materials.  This is just a helper function, Shade() can be call more efficiently manually
	 * since you can typically avoid the extra for loop.
	 */
	inline void ShadeChildMtls(ShadeContext3D &sc, uint8 *block);

	virtual void ObjectChanged(RenderContextSW3D &rc);

	/**
	 * Allocates any thread local storage this material will require for rendering.  The rendering framework will call this function when a
	 * material is being prepared for first use.  Materials needing thread local storage should override this method.  If this method fails
	 * it should throw a memory exception via AfxThrowMemoryException().
	 * @return pointer to the the TLS
	 */
	virtual void *AllocTLS(RenderContextSW3D &rc);

	/**
	 * Frees the supplied thread local storage.   The framework will call this to free up any TLS allocated via AllocTLS().
	 */
	virtual void FreeTLS(void *tls);

	/**
	 * Retrieves a pointer thread local storage created in AllocTLS().  This is usually called from within Shade().
	 */
	void *GetTLS(ShadeContext3D &sc);

	// Called internally by framework, this sets the tls into MtlImplSW3D::TLS[rc.ThreadNumber]
	void SetTLS(RenderContextSW3D &rc, void *tls);

	virtual void Use();
	virtual void Recycle();

	virtual bool DebugIsValid(RenderContext3D &rc);

	static bool IsSet(uint32 *pValue)   { return *pValue != UnInitialized; }
	static bool IsSet(float32 *pValue)  { return IsSet((uint32 *) pValue); }
	static bool IsSet(Vector2f *pValue) { return IsSet((uint32 *) pValue); }
	static bool IsSet(Vector3f *pValue) { return IsSet((uint32 *) pValue); }
	static bool IsSet(Vector4f *pValue) { return IsSet((uint32 *) pValue); }
};






#endif
