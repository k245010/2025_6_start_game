#pragma once
#include "../Object3D/Object3D.h"



class SphereCollision;
struct CollisionHitInfoData;
/// <summary>
/// キャラクターの基底クラス
/// </summary>
class CharaBase : public Object3D
{
public:

	CharaBase();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="trans"> トランスフォーム </param>
	/// <param name="_hModel"> モデルハンドル </param>
	CharaBase(const Transform& trans, const int& _hModel);
	virtual ~CharaBase();

	virtual void Update()	override;
	virtual void Draw()		override { Object3D::Draw(); }

	/// <summary>
	/// アクティブ状態かどうかを返す
	/// </summary>
	/// <returns> アクティブ：非アクティブ </returns>
	bool IsActive() const { return active; }

protected:

	/// <summary>
	/// キャラクターが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	virtual bool HitChara(const CollisionHitInfoData& _targetData);

	SphereCollision* sphereColl				= nullptr;	// 体の球の当たり判定クラスポインタ
	SphereCollision* attackSphereColl		= nullptr;	// 攻撃の球の当たり判定クラスポインタ
	VECTOR3 velocity						= VZero;	// ヴェロシティ

	bool active			= true;						// アクティブかどうかのフラグ
	float hp			= 0.0f;						// HP数値

	int damageSoundID	= -1;						// ダメージを受けたときの再生したいサウンドID
	int deathSoundID	= -1;						// 死亡したときの再生したいサウンドID
};