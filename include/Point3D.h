#ifndef _Point3d_H_
#define _Point3d_H_

#ifndef _VECTOR_MATH_H_INCLUDED_
#error Include VectorMath.h
#endif

#pragma pack(push, 4)

class Vector3d
{
public:
	union
	{
		struct
		{
			float64 X;
			float64 Y;
			float64 Z;
		};

		float64 V[3];
	};

public:
	Vector3d() {}
	Vector3d(const Vector4 &v);
	Vector3d(const Vector3f &v);
	Vector3d(const Vector4f &v);
	Vector3d(float64 x, float64 y, float64 z) { X = x; Y = y; Z = z; }

	float64 Length() const { return sqrt(X * X + Y * Y + Z * Z); }
	float64 Length2() const { return (X * X + Y * Y + Z * Z); }
	
	float64 Normalize()
	{
		float64 len = Length();

		if (len > 0.0)
		{
			float64 norm = 1.0 / len;
			X *= norm;
			Y *= norm;
			Z *= norm;

			return norm;
		}

		return 0.0;
	}

	bool IsNormalized() const { return fabs(Length() - 1.0) < 0.00000001; }
	
	float64 operator*(const Vector3d &p) const
	{
		return X * p.X + Y * p.Y + Z * p.Z;
	}
	
	Vector3d operator*(const float64 s) const
	{
		return Vector3d(X * s, Y * s, Z * s);
	}
	
	Vector3d operator / (const float64 s) const
	{
		float64 is = 1.0 / s;
		return Vector3d(X * is, Y * is, Z * is);
	}
	
	Vector3d operator+(const Vector3d &p) const
	{
		return Vector3d(X + p.X, Y + p.Y, Z + p.Z);
	}
	
	Vector3d operator-(const Vector3d &p) const
	{
		return Vector3d(X - p.X, Y - p.Y, Z - p.Z);
	}
	
	Vector3d operator-()
	{
		X = -X;
		Y = -Y;
		Z = -Z;
		return *this;
	}
	
	Vector3d &operator+=(const Vector3d &p)
	{
		X += p.X;
		Y += p.Y;
		Z += p.Z;

		return *this;
	}
	
	Vector3d &operator-=(const Vector3d &p)
	{
		X -= p.X;
		Y -= p.Y;
		Z -= p.Z;

		return *this;
	}
	
	Vector3d &operator*=(const float64 s)
	{
		X *= s;
		Y *= s;
		Z *= s;

		return *this;
	}
	
	Vector3d &operator/=(const float64 s)
	{
		float64 is = 1.0 / s;
		
		X *= is;
		Y *= is;
		Z *= is;

		return *this;
	}
	
	Vector3d &operator=(const Vector3d &p)
	{
		X = p.X;
		Y = p.Y;
		Z = p.Z;

		return *this;
	}
	
	Vector3d operator * (const Matrix4 &m) const;

	void Project(const Vector3d &axis)
	{
		X = (X - axis.X) / (Z - axis.Z) + axis.X;
		Y = (Y - axis.Y) / (Z - axis.Z) + axis.Y;
	}
};

inline Vector3d operator*(float64 s, const Vector3d &p) 
{
	return Vector3d(s * p.X, s * p.Y, s * p.Z);
}

inline Vector3d operator^(const Vector3d &p, const Vector3d &q)
{
	return Vector3d(
		p.Y * q.Z - p.Z * q.Y,
		p.Z * q.X - p.X * q.Z,
		p.X * q.Y - p.Y * q.X);
}

inline Vector3d Normalize(const Vector3d &p)
{
	float64 iLen = 1.0 / p.Length();
	return Vector3d(p.X * iLen, p.Y * iLen, p.Z * iLen);
}

inline float64 Length(const Vector3d &p)
{
	return p.Length();
}

// lerp from p to q
inline Vector3d Lerp(const Vector3d &p, const Vector3d &q, float64 t)
{
	return (1.0 - t) * p + t * q;
}

inline float64 Dot3(const Vector3d &p0, const Vector3d &p1) 
{
	return p0.X * p1.X + p0.Y * p1.Y + p0.Z * p1.Z;
}


#pragma pack(pop)



#endif
