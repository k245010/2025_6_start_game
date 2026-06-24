#pragma once
#include "BlockBase.h"

class Gauge;
/// <summary>
/// トゲブロック
/// </summary>
class SpikeBlock : public BlockBase
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_modelData">		モデルデータ					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	SpikeBlock(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	~SpikeBlock();

	void Update() override;
	void Draw() override;

private:

	/// <summary>
	/// ブロックが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	bool HitBlock(const CollisionHitInfoData& _targetData) override;

	/// <summary>
	/// UI描画
	/// </summary>
	void UIDraw();

	int uiFunctionIDNumber = -1;	// UIDraw関数に対するIDナンバー

	Gauge* gauge;

	float hpMax;					// 最大HP	
	bool isInvincible = false;		// 無敵かどうか
	float invincibleCount = 0.0f;	// 無敵カウント
};
