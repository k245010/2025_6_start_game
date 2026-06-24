#pragma once
#include "EntityBase.h"

class Coin : public EntityBase
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_modelData">		モデルデータ					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	Coin(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_FREE);
	~Coin();

	void Update() override;

private:

	/// <summary>
	/// エンティティが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	bool HitEntity(const CollisionHitInfoData& _targetData) override;

	VECTOR3 initPosition;		// 初期座標　

	float angle;				// 回転角度
	float time			= 0.0f;	// 時間
	float upLen			= 0.0f;	// Y軸に上がる長さ
	float upCount;				// Y軸に上がるカウント

	VECTOR3 finishScale;		// 最終スケール
};
