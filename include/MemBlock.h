#ifndef _MemBlock_h_
#define _MemBlock_h_

#pragma pack(push, 8)

class MemBlock
{
public:
	void *m_Ptr;
	uint32 m_Size;

public:
	MemBlock() { m_Ptr = NULL; m_Size = 0; }
	MemBlock(void *ptr, uint32 size) { m_Ptr = ptr; m_Size = size; }

	MemBlock(const char *filename)	// Don't forget to free(m_Ptr) yourself!
	{
		m_Ptr = NULL;
		m_Size = 0;

		FILE *f = fopen(filename, "rb");
		if (f)
		{
			fseek(f, 0, SEEK_END);
			m_Size = ftell(f);

			m_Ptr = malloc(m_Size);
			if (m_Ptr)
			{
				fseek(f, 0, SEEK_SET);
				m_Size = (uint32)fread(m_Ptr, 1, m_Size, f);
				if (m_Size == 0)
				{
					free(m_Ptr);
					m_Ptr = NULL;
				}
			}
			else
				m_Size = 0;

			fclose(f);
		}
	}

	bool Save(const char *filename) const
	{
		bool ok = false;

		if (m_Ptr)
		{
			FILE *f = fopen(filename, "wb");

			if (f)
			{
				ok = (fwrite(m_Ptr, 1, m_Size, f) == m_Size);
				fclose(f);
			}
		}
		return ok;
	}
};

#pragma pack(pop)

#endif
