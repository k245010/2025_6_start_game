#pragma once
//#include "CollisionFunction.h"
//#include <cstdint>
#include "../Library/myDxLib.h"
#include <assert.h>

struct VECTOR3I
{
	int x, y, z;

	VECTOR3I() : x(0), y(0), z(0) {};

	VECTOR3I(int _x, int _y, int _z)
		: x(_x), y(_y), z(_z) {
	}

	VECTOR3I(const int& _x, const int& _y, const int& _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	VECTOR3I(const VECTOR3I& _v)
	{
		x = _v.x;
		y = _v.y;
		z = _v.z;
	}

	/*VECTOR3I(VECTOR3I&& _v) noexcept
	{
		x = _v.x;
		y = _v.y;
		z = _v.z;
	}*/

	VECTOR3I(const VECTOR& _v)
	{
		x = (int)_v.x;
		y = (int)_v.y;
		z = (int)_v.z;
	}

	inline VECTOR3I& operator=(const VECTOR3I& _v)
	{
		x = _v.x;
		y = _v.y;
		z = _v.z;
		return *this;
	}

	bool operator ==(const VECTOR3I& v2) const noexcept
	{
		return (x == v2.x && y == v2.y && z == v2.z);
	}
};

// VECTOR3Iをmapでハッシュを使うため定義  キー検索できる
namespace std
{
	// templateの特殊化　（VECTOR3Iにだけ対応したテンプレート）
	template<>
	// ハッシュにVECTOR3Iを定義して、size_tでreturnするようにする
	struct hash<VECTOR3I>
	{
		// 関数呼び出し演算子 operator()  // noexcept その関数が例外を投げないの明示化
		size_t operator()(const VECTOR3I& v) const noexcept
		{
			size_t h1 = std::hash<int>()(v.x);
			size_t h2 = std::hash<int>()(v.y);
			size_t h3 = std::hash<int>()(v.z);
			// メンバをビット演算子で左にずらし、唯一無二のパターンとして保存
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}

inline VECTOR3I VGetI(const int& _x, const int& _y, const int& _z)
{
	VECTOR3I v = VECTOR3I();
	v.x = _x;
	v.y = _y;
	v.z = _z;
	return v;
}

struct VECTOR2I
{
	int x, y;

	VECTOR2I() : x(0), y(0) {};

	VECTOR2I(const int& _x, const int& _y)
	{
		x = _x;
		y = _y;
	}
};

struct VECTORXZ
{
	float x, z;

	VECTORXZ() : x(0.0f), z(0.0f) {};
};

inline VECTOR2I V2IGet(const int& x, const int& y)
{
	VECTOR2I re = VECTOR2I();
	re.x = x;
	re.y = y;
	return re;
}

inline VECTORXZ VXZGet(const float& x, const float& z)
{
	VECTORXZ re = VECTORXZ();
	re.x = x;
	re.z = z;
	return re;
}

inline const VECTORXZ operator -(const VECTORXZ& v, const float& sub) { return VXZGet(v.x - sub, v.z - sub); }

inline const VECTORXZ operator +(const VECTORXZ& v, const float& add) { return VXZGet(v.x + add, v.z + add); }
inline const VECTORXZ& operator +=(VECTORXZ& v, const float& add) { return v = VXZGet(v.x += add, v.z += add); }

inline const VECTORXZ operator *(const VECTORXZ& v, const float& scale) { return VXZGet(v.x * scale, v.z * scale); }
inline const VECTORXZ operator /(const VECTORXZ& v, const float& div) { return VXZGet(v.x / div, v.z / div); }

inline const VECTORXZ operator +(const VECTORXZ& v, const VECTORXZ& add) { return VXZGet(v.x + add.x, v.z + add.z); }
inline VECTORXZ& operator +=(VECTORXZ& v, const VECTORXZ& add) { return v = v + add; }

inline const VECTORXZ operator -(const VECTORXZ& v, const VECTORXZ& sub) { return VXZGet(v.x - sub.x, v.z - sub.z); }
inline VECTORXZ& operator -=(VECTORXZ& v, const VECTORXZ& sub) { return v = v - sub; }

inline const VECTOR2I operator +(const VECTOR2I& v, const VECTOR2I& add) { return V2IGet(v.x + add.x, v.y + add.y); }
inline const VECTOR2I operator -(const VECTOR2I& v, const VECTOR2I& sub) { return V2IGet(v.x - sub.x, v.y - sub.y); }

inline const VECTOR2I& operator *(const VECTOR2I& v, const float& scale) { return V2IGet((int)(v.x * scale), (int)(v.y * scale)); }
inline const VECTOR2I& operator *(const VECTOR2I& v, const VECTOR2I& scale) { return V2IGet(v.x * scale.x, v.y * scale.y); }

inline VECTOR2I& operator *=(VECTOR2I& v, const VECTOR2I& add) { return v = v * add; }
inline VECTOR2I& operator *=(VECTOR2I& v, const float& add) { return v = v * add; }

inline const VECTOR3I operator /(const VECTOR3I& v, const int& div) { return VGetI(v.x / div,v.y / div, v.z / div); }
inline VECTOR3I operator *(const VECTOR3I& v, const int& scale) { return VGetI(v.x * scale, v.y * scale, v.z * scale); }

inline const VECTOR3I operator +(const VECTOR3I& v, const VECTOR3I& add) { return VGetI(v.x + add.x, v.y + add.y, v.z + add.z); }
inline const VECTOR3I operator +(const VECTOR3I& v, const int& add) { return VGetI(v.x + add, v.y + add, v.z + add); }
inline VECTOR3I& operator +=(VECTOR3I& v, const VECTOR3I& add) { return v = v + add; }
inline VECTOR3I& operator +=(VECTOR3I& v, const int& add) { return v = v + add; }

inline const VECTOR3I operator -(const VECTOR3I& v, const VECTOR3I& sub) { return VGetI(v.x - sub.x, v.y - sub.y, v.z - sub.z); };
inline VECTOR3I& operator -=(VECTOR3I& v, const VECTOR3I& sub) { return v = v - sub; }

