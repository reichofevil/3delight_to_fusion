#ifndef _VIEWSHADER_H_
#define _VIEWSHADER_H_

#include "3D_CgWrapper.h"
#include "DFScript.h"
#include <gl/gl.h>
#include <vector>


///////////////////////////////////////////////////
// ViewShader declaration

class FuSYSTEM_API ViewShader : public Object
{
	FuDeclareClass(ViewShader, Object); // declare ClassID static member, etc.

public:
	FuID Name;
	const char *String;
	size_t Len;
	ParamCg *Shader;
	std::vector<FuID> ParamNames;
	std::vector<ParamCg *> Params;

	ViewShader *CloneSource, *NextClone;

public:
	ViewShader(const char *name, const char *str = NULL, size_t len = 0);
	ViewShader(ViewShader &orig);
	~ViewShader();

	void DisconnectAll();													// disconnects all source parameters

	int AddParam(const char *name);										// returns paramname/ParamCg index

	GLuint GetTextureParam(int idx);
	void SetTextureParam(int param, GLuint texObj);					// sets ParamCg value
		
	void SetParam(int param, bool val);
	void SetParam(int param, int val);
	void SetParam(int param, float val);
	void SetParam(int param, float val1, float val2);
	void SetParam(int param, float val1, float val2, float val3);
	void SetParam(int param, float val1, float val2, float val3, float val4);

#ifndef USE_NOSCRIPTING
	DECLARE_SIMPLE_LUATYPE(ViewShader);
#endif
};

class FuSYSTEM_API ViewShaderMerge : public ViewShader
{
	FuDeclareClass(ViewShaderMerge, ViewShader); // declare ClassID static member, etc.

public:
	ViewShaderMerge(const char *name, const char *str = NULL, size_t len = 0) : ViewShader(name, str, len) {}

private:
	int OutToList;
	std::vector<int> InLists;

	friend class ViewShaderGroup;
};

typedef std::vector<ViewShader *> ViewShaderList;


///////////////////////////////////////////////////
// ViewShaderGroup declaration

class FuSYSTEM_API ViewShaderGroup : public ShaderCg
{
protected:
	std::vector<ViewShaderList *> m_ShaderLists;
	std::vector<ViewShaderMerge *> m_MergeShaders;
	ViewShaderList m_TexShaders;
	std::vector<GLuint> m_Textures;

public:
	ViewShaderGroup();
	ViewShaderGroup(const ViewShaderGroup &toCopy);		// nyi
	virtual ~ViewShaderGroup();

	int CreateShaderList();																	// start a new list of shaders
	int CreateTexShaderList(GLuint tex, bool swizzle);								// start new list, and add a texRECT reading shader to start it off
	void FreeShaderList(int list);

	bool AddShader(ViewShader *shader, int list = 0);								// All shaders except first in list must declare a "ViewShader source;" param (no AddParam() needed)
	bool AddShaderMerge(ViewShaderMerge *shader, int list, int n, ...);		// pass n ints (list #s) to be connected to, matched with first n AddParam()s (ViewShaders), -1L terminates
	bool AddShaderClone(ViewShader *origshader, int list = 0);					// Add a clone of 'origshader' to 'list'
	bool FreeShader(ViewShader *shader, int list = 0);

	bool Build();

	bool IsEmpty();
	int GetNumLists();
	int GetNumShaders(int list = 0);
	ViewShader *GetShader(int index, int list = 0);
};

#endif
