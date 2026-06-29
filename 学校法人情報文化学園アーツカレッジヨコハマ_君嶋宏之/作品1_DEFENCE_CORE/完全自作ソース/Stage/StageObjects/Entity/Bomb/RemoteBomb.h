#pragma once
#include "BombBase.h"

class RemoteBomb : public BombBase
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_modelData">		モデルデータ					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	RemoteBomb(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	~RemoteBomb();

	void Update()	override;
	void Draw()		override;

private:

};
