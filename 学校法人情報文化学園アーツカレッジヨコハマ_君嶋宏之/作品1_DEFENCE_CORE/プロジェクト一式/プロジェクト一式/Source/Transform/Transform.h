#pragma once
#include "../../Library/myDxLib.h"

class Transform
{
public:

	VECTOR3 position;	// 座標
	VECTOR3 rotation;	// 回転
	VECTOR3 scale;		// スケール　拡大率
	VECTOR3 size;		// サイズ　モデルの直径

	/// <summary>
	/// コンストラクタ　スケールとサイズは1.0fで初期化
	/// </summary>
	Transform() 
	{
		position	= VZero;
		rotation	= VZero;
		scale		= VOne;
		size		= VOne;
	}

	/// <summary>
	///							コンストラクタ
	/// </summary>
	/// <param name="_pos">		座標		</param>
	/// <param name="_rot">		回転		</param>
	/// <param name="_scale">	スケール	</param>
	/// <param name="_size">	サイズ		</param>
	Transform(const VECTOR3& _pos, const VECTOR3& _rot, const VECTOR3& _scale, const VECTOR3& _size)
	{
		position = _pos;
		rotation = _rot;
		scale	 = _scale;
		size	 = _size;
	}

	/// <summary>
	///							コンストラクタ
	/// </summary>
	/// <param name="_trans">	トランスフォーム </param>
	Transform(const Transform& _trans)
	{
		position	= _trans.position;
		rotation	= _trans.rotation;
		scale		= _trans.scale;
		size		= _trans.size;
	}
	~Transform() {}

	/// <summary>
	///				回転行列を返す
	/// </summary>
	/// <returns>	回転行列 </returns>
	const MATRIX GetRotMatrix() const;

	/// <summary>
	///				回転Y軸の回転行列を返す
	/// </summary>
	/// <returns>	回転Y軸の回転行列 </returns>
	const MATRIX GetRotMatrixY() const;

	/// <summary>
	///				行列を返す
	/// </summary>
	/// <returns>	行列 </returns>
	const MATRIX GetMatrix() const;

	/// <summary>
	///							行列をモデルにセットし返す
	/// </summary>
	/// <param name="_model">	モデルハンドル				</param>
	/// <returns>				行列がセットされたモデル	</returns>
	int SetMatrixModel(int _model) const;

	// トランスフォームのX軸の直径を返す
	float GetLenX() const { return this->scale.x * this->size.x; }
	
	// トランスフォームのY軸の直径を返す
	float GetLenY() const { return this->scale.y * this->size.y; }
	
	// トランスフォームのZ軸の直径を返す
	float GetLenZ() const { return this->scale.z * this->size.z; }

	// トランスフォームの直径を返す
	VECTOR3 GetLen() const { return VECTOR3(GetLenX(), GetLenY(), GetLenZ()); }

private:

};