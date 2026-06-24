#pragma once
#include "BlockBase.h"

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
	/// <param name="_hModel">			モデルハンドル					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	SpikeBlock(const Transform& _trans, const int& _hModel, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
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
};
