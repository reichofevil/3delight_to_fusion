#ifndef _SHADER_ARB_H_
#define _SHADER_ARB_H_

#include "GLExtensions.h"
#include "3D_Resource.h"

class ShaderStream;


class FuSYSTEM_API ShaderARB : public Resource3D
{
	FuDeclareClass(ShaderARB, Resource3D);

public:
	enum ShaderType
	{
		ST_Vertex,
		ST_Fragment,
		ST_NumTypes
	};

public:
	ShaderARB();
	ShaderARB(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~ShaderARB();

	GLuint ProgramID()		{ return ProgID; }
	bool ProgramExists()		{ return ProgID != 0; }

	virtual void Bind();
	virtual void Enable();
	virtual void Disable();

	void ProgramLocal(int slot, const Vector4f &v) { glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, slot, v.V); }

	virtual void FreeData();

	bool CreateProgram(ShaderStream &s, ShaderARB::ShaderType type);
	bool CreateProgram(const char *programString, ShaderARB::ShaderType type);

protected:
	GLenum BindTarget;
	GLuint ProgID;
};














#endif

