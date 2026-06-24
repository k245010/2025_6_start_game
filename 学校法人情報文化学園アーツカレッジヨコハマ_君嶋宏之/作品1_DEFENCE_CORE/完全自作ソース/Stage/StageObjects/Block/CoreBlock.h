#pragma once
#include "BlockBase.h"

class Gauge;
/// <summary>
/// コアブロック　敵が攻撃し破壊をする目標
/// </summary>
class CoreBlock : public BlockBase
{
public:
	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_hModel">			モデルハンドル					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	CoreBlock(const Transform& _trans, const int& _hModel, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_FREE);
	~CoreBlock();

	void Update() override;
	void Draw() override;

	/// <summary>
	/// コアが壊れたかどうか返す
	/// </summary>
	/// <returns> true:壊れた false:壊れていない </returns>
	bool IsCoreBroken() { return isBroken; }

private:

	/// <summary>
	/// ブロックが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	bool HitBlock(const CollisionHitInfoData& _targetData) override;

	/// <summary>
	/// UIの描画
	/// </summary>
	void UIDraw();

	//float hp;
	Gauge* hpGauge;				// ゲージクラスポインタ

	VECTOR3 initPosition;		// 初期座標

	bool isBroken	= false;	// コアが壊れたかどうか
	bool isDamage	= false;	// ダメージを受けたかどうか

	float angle		= 0.0f;		// 回転値　
	float upLen		= 0.0f;		// Y軸に上がる値
	float upCount	= 0.0f;		// Y軸に上がるカウント

	VECTOR2I shakePosition;		// 振幅座標
	float shakeTime = 0.0f;		// 振動時間

	int coreUIImage;			// コアのUI画像ハンドル

	int uiFunctionIDNumber = -1;// UIDraw関数に対するIDナンバー
};
