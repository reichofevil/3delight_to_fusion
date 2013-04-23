#ifndef _MATERIALBASEGLIMPL3D_H_
#define _MATERIALBASEGLIMPL3D_H_

#include "3D_DataTypes.h"
#include "3D_CgWrapper.h"
#include "3D_MaterialBase.h"


class MtlImplGL3D;
class TexImageImplGL3D;
class RenderContextGL3D;





class FuSYSTEM_API TexStateFf3D
{
public:
	GLTexture2D *Texture;
	TEX_TexApplyModes ApplyMode;
	TEX_TexFilterModes MinFilter, MagFilter;
	TEX_TexBorderModes BorderMode[3];							// TODO - 3 for now if this is a 3D texture, but cubemaps?

public:
	TexStateFf3D() 
	{	
		Texture = NULL;
		ApplyMode = TEX_MODULATE;
		MinFilter = MagFilter = TEX_BILINEAR;
		for (int i = 0; i < 3; i++)
			BorderMode[i] = TEX_CLAMP_TO_EDGE;
	}
	~TexStateFf3D() {}
};


class FuSYSTEM_API MtlStateFf3D
{
public:
	Color4f Ambient;				// ambient material coefficient, alpha component is meaningless
	Color4f Diffuse;				// diffuse material coefficient, when lighting is disabled this is uses as the color, alpha component is the material's alpha
	Color4f Specular;				// specular material coefficient, alpha component is meaningless
	Color4f Emission;				
	float32 SpecularExponent;

	enum { MAX_FIXED_TEXTURES = 8 };
	TexStateFf3D Textures[MAX_FIXED_TEXTURES];	// Only the 0th texture is currently used

	void *UserData0;					// materials can store whatever they want here...
	void *UserData1;

	TagList *tags;

public:
	MtlStateFf3D()
	{
		Textures[0].Texture = NULL;
	}
};



//
//   When the renderer calls Implement() the mtl should create a MtlImplGL3D for the passed in MtlData3D.  Note that a overriden version 
//   should not call BaseClass::Implement().  DeImplement() is the matching call to Implement() which allows the material implementation a 
//   chance to do any cleanup for items that may have been allocated in Implement().  Implement() must check rc.ShadePath and return false 
//   if it cannot support the shadepath.  If Implement() is called, DeImplement() is guarenteed to be called even if Implement() fails.
//
//   Everytime the renderer needs to use a material, it will Activate/Deactivate it.  All the Activate/Deactivate calls in a particular render
//   pass will be bracketed by a PreRender/PostRender pair.  Inside of PreRender/Activate material should check rc.ShadePath and alter its 
//   behaviour based on that.
//   * RenderPath == RP_Ff:  the mtl must set rc.MtlStateFf
//   * RenderPath == RP_Cg:  the mtl sets all the uniform parameters in the shader
//
//   Deactivate/PostRender give the material a chance to do any needed cleanup.  If PreRender() or Activate() fail, for whatever reason, the 
//   renderer will decide what to do. Inside of PreRender() the material must also call PreRender() on any child materials - the same applies
//   for Activate/Deactivate/PostRender/Implement/Deimplement.
//
//   Note that if PreRender() fails PostRender() will still get called.  The same holds with Activate()/Deactivate() -- if Activate() fails, 
//   Deactivate() will still get called.  If PreRender() fails on a material, Activate() will never be called.  Note that if even if Activate() 
//   fails, the framework is still responsible for calling PostRender().
//
//   There are 4 major overrides to know about when creating a Cg material:
//   * CreateShaderString() - override this so that it loads from disk or creates dynamically the Cg shader strinng.  If this function is 
//     not overriden, by default it calls GetShaderFilename() to get the filename.
//   * GetShaderFilename() - override this so that it returns the filename of the shader file to load.  CreateShaderString() calls this normally.
//   * CreateShadeNode() - override this so it creates and returns a subclass of ShadeNodeCg.  It should call CreateShadeNode() on all child mtls 
//     and then add the returned ShadeNodeCg via shader->AddShadeNode().  It should connect the shade nodes together into a tree.
//   * CreateShaderKey() - override this so it creates a key that is used to determine if a cached shader is compatible.  Keys can be up to 256
//     characters long.  key[0] must be the overall length of the key including the 0-th entry.  The key should include this materials ID and
//     the IDs of all its children.
//


/**
 * Base class for OpenGL renderer materials.
 * 
 */
class FuSYSTEM_API MtlImplGL3D : public MtlImpl3D
{
	FuDeclareClass(MtlImplGL3D, MtlImpl3D);

public:
	// private: used by renderer, don't touch
	MtlStateFf3D MtlState;
	ShaderCg *CgShader;

	uint8 m_MtlImplGLPad[16];

protected:
	bool CreateManagedParams(RenderContextGL3D &rc, ShadeNodeCg *sn);
	bool CreateManagedChildMtls(RenderContextGL3D &rc);

	bool ActivateManagedParamsCg(RenderContextGL3D &rc);
	bool CreateManagedParamsCg(RenderContextGL3D &rc, ShadeNodeCg *sn);
	bool CreateManagedChildMtlsCg(RenderContextGL3D &rc);

public:
	MtlImplGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlImplGL3D(const MtlImplGL3D &toCopy);		// purposefully left unimplemented
	~MtlImplGL3D();

	static MtlImplGL3D *CreateMtl(RenderContextGL3D &rc, MtlData3D *mtlData); // throws CMemoryException 

	virtual MtlImplGL3D *CreateChildMtl(RenderContextGL3D &rc, int slot);		// throws CMemoryException
	virtual void FreeChildMtls(TagList *tags = NULL); // frees all managed mtls

	virtual bool DoPreRender(RenderContextGL3D &rc);
	virtual void DoPostRender(RenderContextGL3D &rc);
	virtual bool DoActivate(RenderContextGL3D &rc);
	virtual void DoDeactivate(RenderContextGL3D &rc);
	virtual void DoObjectChanged(RenderContextGL3D &rc);

	// see MtlImplSW3D documentation for these funcs for now
	virtual bool PreRender(RenderContextGL3D &rc);
	virtual void PostRender(RenderContextGL3D &rc);
	virtual bool Activate(RenderContextGL3D &rc);
	virtual void Deactivate(RenderContextGL3D &rc);
	virtual void ObjectChanged(RenderContextGL3D &rc);

	/**
	 * CreatechildMtls() is responsible for creating the child materials of this material.  It is called immediately after the constructor.  
	 * By default it will call CreateManagedChildMtls()
	 */
	virtual bool CreateChildMtls(RenderContextGL3D &rc);								// throws CMemoryException 
	virtual bool CreateParams(RenderContextGL3D &rc);

	/**
	 * Override CreateShaderString() to create your materials shader string and return it.  By default it will call 
	 * LoadShaderString(GetShaderFilename(rc)) but you can override it to create your shader strings however you want.
	 * @ret return a NULL shader string on failure
	 */
	virtual ShaderString3D CreateShaderString(RenderContextGL3D &rc);

	/**
	 * Calls DoCreateShaderString() on the children and then CreateShaderString() on this material.
	 */
	virtual bool DoCreateShaderString(RenderContextGL3D &rc);

	/**
	 * Override this to return the path & filename of a file containing your shader string.  This is function is called by the
	 * default implementation of CreateShaderString().
    * TODO : The file is assumed to be located within the c:/Fusion/Shaders directory.
	 */
	virtual const char *GetShaderFilename(RenderContextGL3D &rc);

	/**
	 * @ret returns false if the shader string could not be loaded
	 */
	virtual ShaderString3D LoadShaderString(RenderContextGL3D &rc, const char *file);

	/**
	 * Override this to return the name of the shade node struct in the Cg source code.  The returned pointer must point to a literal compile
	 * time constant string.
	 */
	virtual const char *GetShadeNodeName(RenderContextGL3D &rc);

	/**
	 * Override this so it creates and returns a subclass of ShadeNodeCg.  To understand what you're supposed to do here its
	 * best to look at the SDK examples.
	 */
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc);

	/**
	 * Internally this will call CreateShadeNode(), GetShadeNodeName() create the Cg parameter and link them all up.
	 */
	virtual ShadeNodeCg *DoCreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader);

	/**
	 * Responsible for connecting the parameters of the ShadeNode to the matching parameters in the material
	 */
	virtual bool ConnectParams          (RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *sn, ParamCg mtlStruct);
	virtual bool ConnectChildMtls       (RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *sn, ParamCg mtlStruct);
	virtual bool ConnectManagedParams   (RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *sn, ParamCg mtlStruct);
	virtual bool ConnectManagedChildMtls(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *sn, ParamCg mtlStruct);

	virtual bool ActivateManagedParams(RenderContextGL3D &rc);

	virtual int GetState(RenderContextGL3D &rc, char buf[16]);

	virtual void GetTextureTransform(Vector4f &texOffset, Vector4f &texScale);

	virtual void Use();
	virtual void Recycle();
};




#endif