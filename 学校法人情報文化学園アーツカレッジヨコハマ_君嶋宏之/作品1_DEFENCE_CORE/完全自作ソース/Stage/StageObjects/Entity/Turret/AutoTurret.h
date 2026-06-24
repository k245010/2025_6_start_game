#pragma once
#include "TurretBase.h"

/// <summary>
/// 自動で弾を発射する銃座
/// </summary>
class AutoTurret : public TurretBase
{
public:
	
	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_hModel">			モデルハンドル					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	AutoTurret(const Transform& _trans, const int& _hModel, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	~AutoTurret();

	void Update() override;
	void Draw() override;
	
	/// <summary>
	///				アップグレードができるか返す
	/// </summary>
	/// <returns>	true:アップグレードできる / false:アップグレードできない	</returns>
	virtual bool CanUpgrade() override { return false; }


	/// <summary>
	///						罠のメンテナンスを行う	タレットなら弾の補充など
	/// </summary>
	/// <param name="_add">	進行させるメンテナンス値	0.0f～1.0f(0～100%)	</param>
	/// <returns>			現在のメンテナンス値		0.0f～1.0f(0～100%)	</returns>
	//float Maintaining(const float& _add) override;

private:
	
};
