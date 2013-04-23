#ifndef _FuPath_h_
#define _FuPath_h_

#include "Types.h"
#include "DFScript.h"

class FuSCRIPT_API FuPath : public Lock
{
protected:
	int m_Num;
	char **m_Paths;

	char m_Pad[32];

protected:
	void Init(const char *path);
	bool ReadSubDir(const char *rootpath, const char *dirpath, const char *pattern, ScriptVal &dir, bool subdirs = false, bool flat = false) const;

public:
	FuPath(const char *path = NULL);
	FuPath(const FuPath &path);
	~FuPath();

	FuPath& operator =(const FuPath &path);
	bool    operator ==(const FuPath &path);
	FuPath& operator +=(const FuPath &path);

	void Clear();
	int GetNumSegments() const;
	const char *GetSegment(int num) const;
	bool GetAllSegments(char *buf, int buflen = k_MaxPath) const;

	bool SetSegment(int num, const char *path);
	bool AddSegment(const char *path);
	bool AddSegments(const char *paths);
	bool AddSegments(const FuPath &path);
	bool RemoveSegment(int num);

	bool Exists(const char *path) const;
	bool IsFile(const char *path) const;
	bool IsDir(const char *path) const;
	bool Contains(const FuPath &path) const;

	bool GetFirstMatch(const char *path, char *buf) const;
	bool GetFirstMatch(const char *path, ScriptVal &val) const;
	bool GetAllMatches(const char *path, ScriptVal &ret) const;

	bool GetClosestMatch(const char *path, char *buf) const;
	bool GetClosestMatch(const char *path, ScriptVal &val) const;

	bool ReadDir(const char *pattern, ScriptVal &dir, bool subdirs = false, bool flat = false) const;

	int Map(const ScriptVal &map);
	int ReverseMap(const ScriptVal &map);


public:
	const static int k_MaxPath = MAX_PATH;
};

#endif
