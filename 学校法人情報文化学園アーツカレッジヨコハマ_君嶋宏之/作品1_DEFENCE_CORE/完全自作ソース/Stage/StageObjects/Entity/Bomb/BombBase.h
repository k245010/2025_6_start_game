#pragma once
#include "../EntityBase.h"

class BombBase : public EntityBase
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_hModel">			モデルハンドル					</param>
	/// <param name="_kind">			ステージオブジェクトの種類		</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	BombBase(const Transform& _trans, const int& _hModel, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	~BombBase();

	virtual void Update()	override;
	virtual void Draw()		override;

protected:

	/// <summary>
	///								エンティティが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData">	相手の当たり判定情報												</param>
	/// <returns>					true:自身の当たり判定クラスポインタを破棄する / false:破棄しない	</returns>
	bool HitEntity(const CollisionHitInfoData& _targetData);

	/// <summary>
	/// 爆発処理
	/// </summary>
	void Explosion();

	bool isExplosion						= false;			// 爆発したかどうか
	Transform explosionCollisionTransform	= Transform();		// 爆発の当たり判定を行うためのトランスフォーム
};
