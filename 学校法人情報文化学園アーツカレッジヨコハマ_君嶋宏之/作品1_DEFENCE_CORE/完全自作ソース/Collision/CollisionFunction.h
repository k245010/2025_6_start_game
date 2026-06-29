#pragma once
#include "../../Library/gameObject.h"
#include "../Transform/Transform.h"
#include <assert.h>

// 誤差
constexpr float EPSILON_NUM = 0.0001f;

/// <summary>
/// VECTORとVECTORの掛け算を行う
/// </summary>
/// <param name="v1"> VECTOR１ </param>
/// <param name="v2"> VECTOR２ </param>
/// <returns> VECTOR×VECTOR </returns>
inline VECTOR3 VMult(const VECTOR3& v1, const VECTOR3& v2)
{
	VECTOR3 re;
	re.x = v1.x * v2.x;
	re.y = v1.y * v2.y;
	re.z = v1.z * v2.z;
	return re;
}

/// <summary>
/// VECTORの各要素の足し算
/// </summary>
/// <param name="v1"> 足されるVECTOR </param>
/// <param name="add"> 足す値 </param>
/// <returns> VECTOR.x + add,VECTOR.y + add,VECTOR.z + add </returns>
inline VECTOR3 VAddF(const VECTOR3& v1, const float& add)
{
	VECTOR3 re;
	re.x = v1.x + add;
	re.y = v1.y + add;
	re.z = v1.z + add;
	return re;
}

/// <summary>
/// VECTORの各要素の引き算
/// </summary>
/// <param name="v1"> 引かれるVECTOR </param>
/// <param name="sub"> 引く値 </param>
/// <returns> VECTOR.x - sub,VECTOR.y - sub,VECTOR.z - sub </returns>
inline VECTOR3 VSubF(const VECTOR3& v1, const float& sub)
{
	VECTOR3 re;
	re.x = v1.x - sub;
	re.y = v1.y - sub;
	re.z = v1.z - sub;
	return re;
}

/// <summary>
/// VECTORの各要素の割り算
/// </summary>
/// <param name="v1"> 割られるVECTOR </param>
/// <param name="d"> 割る値 </param>
/// <returns> VECTOR.x / d,VECTOR.y / d,VECTOR.z / d </returns>
inline VECTOR3 VDivF(const VECTOR3& v1, const float& d)
{
	VECTOR3 re;
	re.x = v1.x / d;
	re.y = v1.y / d;
	re.z = v1.z / d;
	return re;
}

/// <summary>
/// VECTORの各要素の割り算
/// </summary>
/// <param name="v1"> 割られるVECTOR </param>
/// <param name="d"> 割る値 </param>
/// <returns> VECTOR.x / d,VECTOR.y / d,VECTOR.z / d </returns>
inline VECTOR3 VDivI(const VECTOR3& v1, const int& d)
{
	VECTOR3 re;
	re.x = v1.x / d;
	re.y = v1.y / d;
	re.z = v1.z / d;
	return re;
}

/// <summary>
/// VECTORとVECTORの割り算
/// </summary>
/// <param name="v1"> VECTOR１ </param>
/// <param name="v2"> VECTOR２ </param>
/// <returns> VECTOR / VECTOR </returns>
inline VECTOR3 VDiv(const VECTOR3& v1, const VECTOR3& v2)
{
	VECTOR3 re;
	re.x = v1.x / v2.x;
	re.y = v1.y / v2.y;
	re.z = v1.z / v2.z;
	return re;
}

/// <summary>
/// 引数で渡されたint番目のVECTORメンバーを無視した引き算をする
/// </summary>
/// <param name="v1"> VECTOR1 </param>
/// <param name="v2"> VECTOR2 </param>
/// <param name="structIndexNumber"> 無視したいメンバーの番目 x..0 から </param>
/// <returns> 特定の値を0.0fとして返す </returns>
inline VECTOR3 VSubIgnoreNumber(const VECTOR3& v1, const VECTOR3& v2, const int& structIndexNumber)
{
	VECTOR3 re;
	re = v1 - v2;
	switch (structIndexNumber)
	{
	case 0:
		re.x = 0.0f;
		return re;
		break;
	case 1:
		re.y = 0.0f;
		return re;
		break;
	case 2:
		re.z = 0.0f;
		return re;
		break;
	default:
		break;
	}
	assert(false);
	return re;
}

/// <summary>
/// VECTORのメンバ数値を返す
/// </summary>
/// <param name="v1"> VECTOR </param>
/// <param name="i"> ほしいメンバ　0 -> x, 1 -> y, 2 -> z </param>
/// <returns> メンバ値 </returns>
inline float GetVMem(const VECTOR3& v1, const int& i)
{
	switch (i)
	{
	case 0:
		return v1.x;
		break;
	case 1:
		return v1.y;
		break;
	case 2:
		return v1.z;
		break;
	}
	assert(false);
	return 0.0f;
}

/// <summary>
/// VECTORのメンバ数値の絶対値を返す
/// </summary>
/// <param name="v1"> VECTOR </param>
/// <param name="i"> ほしいメンバ　0 -> x, 1 -> y, 2 -> z </param>
/// <returns> メンバの絶対値 </returns>
inline float GetVMemFabs(const VECTOR3& v1, const int& i)
{
	switch (i)
	{
	case 0:
		return fabsf(v1.x);
		break;
	case 1:
		return fabsf(v1.y);
		break;
	case 2:
		return fabsf(v1.z);
		break;
	}
	assert(false);
	return 0.0f;
}

/// <summary>
/// VECTORのメンバ数値をセット
/// </summary>
/// <param name="v1"> VECTOR </param>
/// <param name="i"> ほしいメンバ　0 -> x, 1 -> y, 2 -> z </param>
/// <param name="set"> セットする値 </param>
inline void SetVMem(VECTOR3& v1, const int& i, const float& set)
{
	switch (i)
	{
	case 0:
		v1.x = set;
		break;
	case 1:
		v1.y = set;
		break;
	case 2:
		v1.z = set;
		break;
	default:
		assert(false);
		break;
	}
}

/// <summary>
/// VECTORのメンバ数値をセット
/// </summary>
/// <param name="v1"> VECTOR3I </param>
/// <param name="i"> ほしいメンバ　0 -> x, 1 -> y, 2 -> z </param>
/// <param name="set"> セットする値 </param>
inline void SetVMem(VECTOR3I& v1, const int& i, const int& set)
{
	switch (i)
	{
	case 0:
		v1.x = set;
		break;
	case 1:
		v1.y = set;
		break;
	case 2:
		v1.z = set;
		break;
	default:
		assert(false);
		break;
	}
}

/// <summary>
/// VECTORのメンバ数値を全てセット
/// </summary>
/// <param name="v1"> VECTOR </param>
/// <param name="set"> 全てのメンバをセットする値 </param>
inline void SetVMem(VECTOR3& v1, const float& set)
{
	v1.x = set;
	v1.y = set;
	v1.z = set;
}

/// <summary>
/// スワップする
/// </summary>
/// <param name="v1"> 値１ </param>
/// <param name="v2"> 値２ </param>
template <typename T>
inline void Swap(T& v1, T& v2)
{
	const T cpyV = v1;
	v1 = v2;
	v2 = cpyV;
}

/// <summary>
/// モデルを四角形と仮定したときの、法線ベクトルを返す
/// </summary>
/// <param name="member"> ほしいメンバ　0 -> x, 1 -> y, 2 -> z </param>
/// <param name="rot"> 回転ベクトル </param>
/// <returns> 法線ベクトル </returns>
inline VECTOR3 GetVDirection(const int& member,const VECTOR3& rot)
{
	VECTOR3 vec = VGet(0, 0, 0);

	SetVMem(vec, member, 1);

	MATRIX mRot = MGetIdent();
	mRot = MMult(mRot, MGetRotZ(rot.z));
	mRot = MMult(mRot, MGetRotX(rot.x));
	mRot = MMult(mRot, MGetRotY(rot.y));

	vec = VTransform(vec, mRot);

	return vec;
}

/// <summary>
/// VECTORの絶対値を返す
/// </summary>
/// <param name="vec"> 絶対値に変えたいベクトル </param>
/// <returns> VECTORの絶対値 </returns>
inline VECTOR3 GetVFabs(const VECTOR3& vec)
{
	VECTOR3 re;
	re.x = fabsf(vec.x);
	re.y = fabsf(vec.y);
	re.z = fabsf(vec.z);

	return re;
}

/// <summary>
///  VECTORのメンバ数値の最大値を返す
/// </summary>
/// <param name="vec"> VECTOR </param>
/// <returns> メンバ数値の最大値 </returns>
inline float GetVMax(const VECTOR3& vec)
{
	float max = vec.x;
	int mem = 0;
	VECTOR3 re = VGet(0, 0, 0);
	for (int i = 1;i < 3;i++)
	{
		if (max < GetVMem(vec, i))
		{
			max = GetVMem(vec, i);
		}
	}
	return max;
}

/// <summary>
///  VECTORのメンバ数値の最大値を１としてノーマライズしたVECTORを返す
/// </summary>
/// <param name="vec"> VECTOR </param>
/// <returns> メンバ数値の最大値を１としてノーマライズしたVECTOR　0,0,1..etc</returns>
inline VECTOR3 GetVMaxMemNorm(const VECTOR3& vec)
{
	float max = vec.x;
	int mem = 0;
	VECTOR3 re = VGet(0, 0, 0);
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

/// <summary>
///  VECTORのメンバ数値を絶対値として計算し、最大値を１or-1としてノーマライズしたVECTORを返す
/// </summary>
/// <param name="vec"> VECTOR </param>
/// <returns> メンバ数値の最大値を１or-1としてノーマライズしたVECTOR　0,0,1　0,0,-1.. etc</returns>
inline VECTOR3 GetVMaxMemNormFabs(const VECTOR3& vec)
{
	float max = vec.x;
	int mem = 0;
	float sign = 1.0f;
	VECTOR3 re = VGet(0, 0, 0);
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

/// <summary>
///  VECTORのメンバ数値の最小値を１としてノーマライズしたVECTORを返す
/// </summary>
/// <param name="vec"> VECTOR </param>
/// <returns> メンバ数値の最小値を１としてノーマライズしたVECTOR　0,0,1.. etc </returns>
inline VECTOR3 GetVMinMemNorm(const VECTOR3& vec)
{
	float min = vec.x;
	int mem = 0;
	VECTOR3 re = VGet(0, 0, 0);
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

/// <summary>
///  VECTORのメンバ数値を絶対値として計算し、最小値を１or-1としてノーマライズしたVECTORを返す
/// </summary>
/// <param name="vec"> VECTOR </param>
/// <returns> メンバ数値の最小値を１or-1としてノーマライズしたVECTOR　0,0,1　0,0,-1.. etc</returns>
inline VECTOR3 GetVMinMemNormFabs(const VECTOR3& vec)
{
	float min = vec.x;
	int mem = 0;
	float sign = 1.0f;
	VECTOR3 re = VGet(0, 0, 0);
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

/// <summary>
/// VECTORのメンバ数値の最大値のメンバ番号を返す
/// </summary>
/// <param name="vec"> VECTOR </param>
/// <returns> 最大メンバ番号 0 -> x, 1 -> y, 2 -> z </returns>
inline int GetVMemIndexMax(const VECTOR3& vec)
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

/// <summary>
/// VECTORのメンバ数値の最小値のメンバ番号を返す
/// </summary>
/// <param name="vec"> VECTOR </param>
/// <returns> 最小メンバ番号 0 -> x, 1 -> y, 2 -> z </returns>
inline int GetVMemIndexMin(const VECTOR3& vec)
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

/// <summary>
/// VECTOR1とVECTOR2の距離を正のベクトルで返す
/// </summary>
/// <param name="_v1"> VECTOR1 </param>
/// <param name="_v2"> VECTOR2 </param>
/// <returns></returns>
inline VECTOR3 GetVDistance(VECTOR3 _v1, VECTOR3 _v2)
{
	VECTOR3 dis = _v1 - _v2;

	return GetVFabs(dis);
}

/// <summary>
/// floatとfloatの値が同じか判断する
/// </summary>
/// <param name="f1"> float1 </param>
/// <param name="f2"> float2 </param>
/// <param name="_epsilon"> 互いの値の誤差を許す値 </param>
/// <returns> 同じ：同じでない </returns>
inline bool FSame(const float& f1, const float& f2, const float _epsilon = EPSILON_NUM)
{
	if (fabsf(f1 - f2) < _epsilon)
		return true;
	return false;
}

/// <summary>
/// VECTORメンバとVECTORメンバが同じか判断する
/// </summary>
/// <param name="v1"> VECTOR1 </param>
/// <param name="v2"> VECTOR2 </param>
/// <param name="_epsilon"> 互いの値のの誤差を許す値 </param>
/// <returns> 同じ：同じでない </returns>
inline bool VAllSame(const VECTOR3& v1, const VECTOR3& v2,const float _epsilon = EPSILON_NUM)
{
	if (FSame(v1.x,v2.x,_epsilon) && FSame(v1.y,v2.y,_epsilon) && FSame(v1.z,v2.z,_epsilon))
	{
		return true;
	}
	return false;
}

/// <summary>
/// かわいいだけじゃだめですか？
/// VECTORメンバとfloat値がすべて同じか判定する
/// </summary>
/// <param name="v1"> VECTOR </param>
/// <param name="v2"> float </param>
/// <returns> 同じ：同じでない </returns>
inline bool VAllSameF(const VECTOR3& v1, const float& f)
{
	if ((VSquareSize(v1) - f * f) > EPSILON_NUM)
		return false;

	if (fabsf(v1.x - f) < EPSILON_NUM && fabsf(v1.y - f) < EPSILON_NUM && fabsf(v1.z - f) < EPSILON_NUM)
	{
		return true;
	}
	return false;
}

/// <summary>
///							レイと球の当たり判定 始点a1から方向ベクトルv1のレイが、中心点v2、半径radiusの球と衝突しているか
/// </summary>
/// <param name="a1">		レイの始点						</param>
/// <param name="v1">		レイの方向ベクトル				</param>
/// <param name="p">		球の座標						</param>
/// <param name="radius">	球の半径						</param>
/// <param name="deadDis">	判定する距離(二乗の値をいれる)	</param>
/// <returns>				当たっている：あたっていない	</returns>
inline bool RayToSphere(const VECTOR3& a1, const VECTOR3 v1, const VECTOR3 p, const float radius,const float deadDis = -1)
{
	// 半径が0
	if (radius < 0.0f) 
		return false;

	// スタート地点からゴール地点の方向ベクトル
	if (VAllSameF(v1, 0.0f)) 
		return false;

	// 円の中心点が原点になるように始点をオフセット
	VECTOR3 a = a1 - p;
	VECTOR3 v = v1;

	// レイの方向ベクトルを正規化
	v = v.Normalize();

	// 係数tを算出(方向ベクトルと球が当たる場所)
	float dotAV = VDot(a, v);
	float dotAA = VDot(a, a);
	float s = dotAV * dotAV - dotAA + radius * radius;
	
	// 誤差修正
	if (fabsf(s) < EPSILON_NUM)
		s = 0.0f;

	// 衝突していない
	if (s < 0.0f)
		return false;

	float sq = sqrtf(s);
	float t1 = -dotAV - sq;// t1が近い方
	float t2 = -dotAV + sq;// t2が遠い方

	// deadDisが設定されている場合
	if(deadDis >= 0.0f)
	{
		// t1及びt2がdeadDisより大きい場合は衝突していない
		if (t1 * t1 > deadDis)
			return false;
	}

	// もしt1及びt2がマイナスだったら始点が
	// 円内にめり込んでいるのでエラーとする
	if (t1 < 0.0f || t2 < 0.0f)
		return false;

	return true;
}

/// <summary>
///									OBBと球の最短ベクトルを返す
/// </summary>
/// <param name="_obbPos">			OBBの座標					</param>
/// <param name="_obbLen">			OBBの直径					</param>
/// <param name="_obbRot">			OBBの回転値					</param>
/// <param name="_spherePos">		球の座標					</param>
/// <param name="_sphereRadius">	球の半径					</param>
/// <returns>						最短ベクトル				</returns>
inline VECTOR3 OBBToSphereVec(const VECTOR3& _obbPos, const VECTOR3& _obbLen, const VECTOR3& _obbRot, const VECTOR3& _spherePos, const float& _sphereRadius)
{
	VECTOR3 overVec				= VZero;						// はみ出しベクトル
	const VECTOR3 DISTANCE		= _spherePos - _obbPos;			// お互いのベクトル
	const VECTOR3 obbHalfLen	= VDivF(_obbLen, 2.0f);			// １辺の半分の長さ

	// VECTORの座標のメンバ一つ一つを見る
	for (int mem = 0;mem < 3;mem++)
	{
		// 辺の半分の長さが０以下だったら計算しない
		if (GetVMem(obbHalfLen, mem) <= 0)
			continue;

		// 回転値からOBBの各面の法線方向を取得する
		VECTOR3 obbDir	= GetVDirection(mem, _obbRot);

		// 内積を使って四角形に対して現在座標の位置を割り出す -1以上 or 1以上のとき、mem軸の辺に対して、はみ出している
		float lenRate	= VDot(DISTANCE, obbDir) / GetVMem(obbHalfLen, mem);	// 辺に対する位置の割合

		// 絶対値にする
		lenRate = fabsf(lenRate);

		// 四角形の辺からはみ出ている
		if (lenRate > 1)
		{
			// どれだけはみ出したかの算出
			overVec += ((obbDir * (lenRate - 1)) * GetVMem(obbHalfLen, mem));
		}
	}
	return overVec;
}

/// <summary>
///								OBBと球が当たっているか返す
/// </summary>
/// <param name="_obbTrans">	OBBのトランスフォーム							</param>
/// <param name="_sphereTrans">	球のトランスフォーム							</param>
/// <returns>					return:当たっている / false:当たっていない		</returns>
inline bool IsHitOBBToSphere(const Transform& _obbTrans, const Transform& _sphereTrans)
{
	const float collRadius = _sphereTrans.GetLenX();
	return (OBBToSphereVec(_obbTrans.position, _obbTrans.GetLen(), _obbTrans.rotation, _sphereTrans.position, _sphereTrans.GetLen().x).SquareSize() < collRadius * collRadius);
}

/// <summary>
///								OBBと球の最短ベクトルを返す
/// </summary>
/// <param name="_obbTrans">	OBBのトランスフォーム				</param>
/// <param name="_sphereTrans">	球のトランスフォーム				</param>
/// <returns>					最短ベクトル						</returns>
inline VECTOR3 OBBToSphereVec(const Transform& _obbTrans, const Transform& _sphereTrans)
{
	return OBBToSphereVec(_obbTrans.position, _obbTrans.GetLen(), _obbTrans.rotation, _sphereTrans.position, _sphereTrans.GetLen().x);
}

/// <summary>
///								相手がカメラの視野角内に存在するか返す
/// </summary>
/// <param name="_pos">			自身のトランスフォーム			</param>
/// <param name="_viewAngle">	カメラの視野角	ラジアン値		</param>
/// <param name="_targetPos">	相手の座標						</param>
/// <returns>					true:存在する / false:存在しない</returns>
inline bool InCameraTarget(const Transform& _trans, const float& _viewAngle, const VECTOR3& _targetPos)
{
	VECTOR3 frontVec	= VNormZ * MGetRotY(_trans.rotation.y);	// 前ベクトル
	VECTOR3 targetVec	= VNorm(_targetPos - _trans.position);	// 相手とのベクトル

	float dot			= VDot(frontVec, targetVec);

	// 視野角内だったら true / 視野角外だったら false
	return (dot >= cosf(_viewAngle));
}

/// <summary>
///								相手が右側に存在するか返す
/// </summary>
/// <param name="_trans">		自身のトランスフォーム						</param>
/// <param name="_targetPos">	相手の座標									</param>
/// <returns>					true:右側に存在する / false:左側に存在する	</returns>
inline bool IsRightTarget(const Transform& _trans, const VECTOR3& _targetPos)
{
	VECTOR3 rightVec	= VNormX * MGetRotY(_trans.rotation.y);	// 右ベクトル
	VECTOR3 targetVec	= VNorm(_targetPos - _trans.position);	// 相手とのベクトル

	float dot			= VDot(rightVec, targetVec);

	// 右に相手がいたら true / 左に相手がいたら false
	return (dot >= 0);
}

