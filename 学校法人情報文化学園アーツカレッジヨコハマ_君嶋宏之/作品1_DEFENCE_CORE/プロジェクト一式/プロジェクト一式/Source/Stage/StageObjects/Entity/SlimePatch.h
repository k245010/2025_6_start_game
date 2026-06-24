#pragma once
#include "EntityBase.h"

/// <summary>
/// スライムパッチ　プレイヤーが投げるべとべとスライム　敵は触れると移動速度が低下する
/// </summary>
class SlimePatch : public EntityBase
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_modelData">		モデルデータ					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	SlimePatch(const Transform& _trans, const ModelData& _modelData, const int& _hp,const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_FREE);
	~SlimePatch();
	
	void Update()	override;
	void Draw()		override;

private:

	/// <summary>
	/// SlimePatch大きさの状態
	/// </summary>
	enum class SCALE_STATE
	{
		STAY,			// 待機
		SCALE_UP,		// モデルの大きさを拡大 (召喚直後)
		SCALE_DOWN,		// モデルの大きさを縮小
	};

	/// <summary>
	/// 召喚されてからの経過時間と共にモデルを拡大する
	/// </summary>
	/// <returns>	true:拡大成功 / false:モデルが最大の大きさになった	</returns>
	bool ModelScaleUpUpdate();

	/// <summary>
	///				HP減少と共にモデルを縮小する
	/// </summary>
	/// <returns>	true:縮小成功 / false:HPが0になった	</returns>
	bool ModelScaleDownUpdate();

	/// <summary>
	/// エンティティが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	bool HitEntity(const CollisionHitInfoData& _targetData) override;

	SCALE_STATE scaleState = SCALE_STATE::SCALE_UP;	// 拡縮状態

	float summonTime		= 0.0f;		// 召喚されてからの時間
	float biggerScaleRate;				// 1秒間に対する拡大率
	float addScale;						// 足すスケール値
	float initScaleY;					// 初期化時の初めのYスケール値

	float subHpSum			= 0.0f;		// 減らすHP合計量
	float subHpFrame		= 0.0f;		// 1フレーム間に減らすHP量

	bool isHit				= false;	// 当たったかどうか
	bool isInvincible		= true;		// 無敵かどうか　召喚直後は無敵
	float invincibleCount	= 0.0f;		// 無敵になってからのカウント
};
