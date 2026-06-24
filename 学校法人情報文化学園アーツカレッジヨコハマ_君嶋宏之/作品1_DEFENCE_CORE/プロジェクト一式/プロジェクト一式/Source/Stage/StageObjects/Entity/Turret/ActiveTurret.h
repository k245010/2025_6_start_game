#pragma once
#include "TurretBase.h"

/// <summary>
/// メンテナンスをしないと時間で、動作しなくなるタレット
/// </summary>
class ActiveTurret : public TurretBase
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_modelData">		モデルデータ					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	ActiveTurret(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	~ActiveTurret();

	void Update() override;
	void Draw() override;

	/// <summary>
	///						罠のメンテナンスを行う	タレットなら弾の補充など
	/// </summary>
	/// <param name="_add">	進行させるメンテナンス値	0.0f～1.0f(0～100%)	</param>
	/// <returns>			現在のメンテナンス値		0.0f～1.0f(0～100%)	</returns>
	float Maintaining(const float& _add) override;

private:

	float activeTime = 0.0f;	// タレットの動作時間
};