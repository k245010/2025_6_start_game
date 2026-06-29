#pragma once
#include "../Transform/Transform.h"
#include "../../Library/GameObject.h"

/// <summary>
/// 物理クラス　現在はヴェロシティの値を管理しています
/// </summary>
class Physics : public GameObject
{
public:

	/// <summary>
	///							コンストラクタ
	/// </summary>
	/// <param name="_trans">	トランスフォーム参照値		</param>
	/// <param name="_vel">		ヴェロシティ参照値			</param>
	/// <param name="_fric">	抵抗力						</param>
	/// <param name="_dis">		最高速度					</param>
	/// <param name="_gravity"> 重力を有効にするかどうか	</param>
	Physics(Transform& _trans,VECTOR3& _vel, const VECTOR3& _fric,const float& _dis,bool _gravity = false);
	~Physics();

	void Update()	override;
	void Draw()		override;

	/// <summary>
	///						摩擦力を設定
	/// </summary>
	/// <param name="fric"> 摩擦値			</param>
	void SetFriction(const VECTOR3& fric) { friction = fric; }

	/// <summary>
	///								最大速度を設定
	/// </summary>
	/// <param name="_maxDistance"> 最大速度		</param>
	void SetMaxDistance(const float& _maxDistance) { maxDistance = _maxDistance; }

	/// <summary>
	///							追加のヴェロシティを設定　爆発で進行方向とは別のvelocityが必要な時
	/// </summary>
	/// <param name="_vel">		ヴェロシティ	</param>
	/// <param name="friction"> 摩擦値			</param>
	void SetAddVelocity(const VECTOR3& _vel,const VECTOR3& _friction);

	/// <summary>
	///						ヴェロシティにべクトルを加える　
	/// </summary>
	/// <param name="_vel"> べクトル		</param>
	void AddForce(const VECTOR3& _vel);

	/// <summary>
	///				Y軸を0としたヴェロシティを返す
	/// </summary>
	/// <returns>	Y軸0のヴェロシティ				</returns>
	VECTOR3 GetFlatVelocity() { return VECTOR3(velocity->x, 0.0f, velocity->z); }

	/// <summary>
	///						重力のヴェロシティを設定する
	/// </summary>
	/// <param name="_g">	重力ヴェロシティ				</param>
	void SetGravityVec(const VECTOR3& _g)	{ gravityVelocity = _g; }

	/// <summary>
	///						重力を使うかどうか設定する
	/// </summary>
	/// <param name="_use"> true:使う / false:使わない </param>
	void SetUseGravity(const bool& _use)	{ isGravity = _use; }

	/// <summary>
	///				重力を使っているかどうか返す
	/// </summary>
	/// <returns>	true:使っている / false:使っていない	</returns>
	bool IsUseGravity() const { return isGravity; }

	/// <summary>
	///				１フレーム前の座標を返す
	/// </summary>
	/// <returns>	１フレーム前の座標			</returns>
	const Transform& GetLastTransform() const { return lastTransform; }

private:

	/// <summary>
	///								抵抗値からヴェロシティを設定する
	/// </summary>
	/// <param name="_vel">			設定するヴェロシティ	</param>
	/// <param name="_friction">	抵抗値					</param>
	void SetFrictionToVelocity(VECTOR3& _vel,const VECTOR3& _friction);

	Transform* transform;		// トランスフォームのポインタ
	Transform lastTransform;	// １フレーム前の座標
	VECTOR3* velocity;			// ヴェロシティのポインタ
	VECTOR3 addVelocity			= VZero;	// 追加のヴェロシティ
	VECTOR3 addVelocityFriction = VZero;	// 追加のヴェロシティの抵抗値
	VECTOR3 gravityVelocity;	// 重力ヴェロシティ
	bool isGravity;				// 重力を追加するかのフラグ　
	VECTOR3 friction;			// 抵抗値
	float maxDistance;			// 最大速度
};