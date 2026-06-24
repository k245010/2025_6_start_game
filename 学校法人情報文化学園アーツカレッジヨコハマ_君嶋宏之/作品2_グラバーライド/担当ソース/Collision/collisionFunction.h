#pragma once
#include "../Library/gameObject.h"

inline VECTOR VMult(const VECTOR& v1, const VECTOR& v2)
{
	VECTOR re;
	re.x = v1.x * v2.x;
	re.y = v1.y * v2.y;
	re.z = v1.z * v2.z;
	return re;
}

inline VECTOR VAddF(const VECTOR& v1, const float& add)
{
	VECTOR re;
	re.x = v1.x + add;
	re.y = v1.y + add;
	re.z = v1.z + add;
	return re;
}

inline VECTOR VSubF(const VECTOR& v1, const float& sub)
{
	VECTOR re;
	re.x = v1.x - sub;
	re.y = v1.y - sub;
	re.z = v1.z - sub;
	return re;
}

inline VECTOR VDivF(const VECTOR& v1, const float& d)
{
	VECTOR re;
	re.x = v1.x / d;
	re.y = v1.y / d;
	re.z = v1.z / d;
	return re;
}

inline VECTOR VDiv(const VECTOR& v1, const VECTOR& v2)
{
	VECTOR re;
	re.x = v1.x / v2.x;
	re.y = v1.y / v2.y;
	re.z = v1.z / v2.z;
	return re;
}

inline float GetVMem(const VECTOR& v1, const int& i)
{
	switch (i)
	{
	case 0:
		return v1.x;
		break;
	case 1:
		return v1.y;
		break;
	default:
		return v1.z;
		break;
	}
}

inline void SetVMem(VECTOR& v1, const int& i, const float& set)
{
	switch (i)
	{
	case 0:
		v1.x = set;
		break;
	case 1:
		v1.y = set;
		break;
	default:
		v1.z = set;
		break;
	}
}

inline void SetFSwap(float& v1, float& v2)
{
	const float cpyV = v1;
	v1 = v2;
	v2 = cpyV;
}

inline void SetVSwap(VECTOR& v1, VECTOR& v2)
{
	const VECTOR cpyV = v1;
	v1 = v2;
	v2 = cpyV;
}

inline VECTOR GetVDirection(const int& member,const VECTOR& rot)
{
	VECTOR vec = VGet(0, 0, 0);

	SetVMem(vec, member, 1);

	MATRIX mRot = MGetIdent();
	mRot = MMult(mRot, MGetRotZ(rot.z));
	mRot = MMult(mRot, MGetRotX(rot.x));
	mRot = MMult(mRot, MGetRotY(rot.y));

	vec = VTransform(vec, mRot);

	return vec;
}

inline VECTOR GetVFabs(const VECTOR& vec)
{
	VECTOR re;
	re.x = fabsf(vec.x);
	re.y = fabsf(vec.y);
	re.z = fabsf(vec.z);

	return re;
}

inline float GetVMax(const VECTOR& vec)
{
	float max = vec.x;
	int mem = 0;
	VECTOR re = VGet(0, 0, 0);
	for (int i = 1;i < 3;i++)
	{
		if (max < GetVMem(vec, i))
		{
			max = GetVMem(vec, i);
		}
	}
	return max;
}

inline VECTOR GetVMaxMemNorm(const VECTOR& vec)
{
	float max = vec.x;
	int mem = 0;
	VECTOR re = VGet(0, 0, 0);
	for (int i = 1;i < 3;i++)
	{
		if (max < GetVMem(vec, i))
		{
			max = GetVMem(vec, i);
			mem = i;
		}
	}
	SetVMem(re, mem, 1);
	return re;
}

inline VECTOR GetVMaxMemNormFabs(const VECTOR& vec)
{
	float max = vec.x;
	int mem = 0;
	float sign = 1.0f;
	VECTOR re = VGet(0, 0, 0);
	for (int i = 1;i < 3;i++)
	{
		if (fabsf(max) < fabsf(GetVMem(vec, i)))
		{
			max = GetVMem(vec, i);
			mem = i;
		}
	}
	if (max < 0)
		sign = -1.0f;

	SetVMem(re, mem, sign);
	return re;
}

inline VECTOR GetVMinMemNorm(const VECTOR& vec)
{
	float min = vec.x;
	int mem = 0;
	VECTOR re = VGet(0, 0, 0);
	for (int i = 1;i < 3;i++)
	{
		if (min > GetVMem(vec, i))
		{
			min = GetVMem(vec, i);
			mem = i;
		}
	}
	SetVMem(re, mem, 1);
	return re;
}

inline VECTOR GetVMinMemNormFabs(const VECTOR& vec)
{
	float min = vec.x;
	int mem = 0;
	float sign = 1.0f;
	VECTOR re = VGet(0, 0, 0);
	for (int i = 1;i < 3;i++)
	{
		if (fabsf(min) > fabsf(GetVMem(vec, i)))
		{
			min = GetVMem(vec, i);
			mem = i;
		}
	}
	if (min < 0)
		sign = -1.0f;

	SetVMem(re, mem, sign);
	return re;
}

inline int GetVMemIndexMax(const VECTOR& vec)
{
	float max = vec.x;
	int re = 0;
	for (int i = 1;i < 3;i++)
	{
		if (max < GetVMem(vec, i))
		{
			max = GetVMem(vec, i);
			re = i;
		}
	}
	return re;
}

inline int GetVMemIndexMin(const VECTOR& vec)
{
	float min = vec.x;
	int re = 0;
	for (int i = 1;i < 3;i++)
	{
		if (min > GetVMem(vec, i))
		{
			min = GetVMem(vec, i);
			re = i;
		}
	}
	return re;
}

inline bool VAllSame(const VECTOR& v1, const VECTOR& v2)
{
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
	{
		return true;
	}
	return false;
}

inline bool VAllSameF(const VECTOR& v1, const float& v2)
{
	if (v1.x == v2 && v1.y == v2 && v1.z == v2)
	{
		return true;
	}
	return false;
}


