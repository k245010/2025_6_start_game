#pragma once
#include "BlockBase.h"

class ModelCollision;
class BackBlockBase : public BlockBase
{
public:
	
	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム					</param>
	/// <param name="_modelData">		モデルハンドル						</param>
	/// <param name="_kind">			ステージオブジェクトの種類			</param>
	/// <param name="_hp">				HP									</param>
	/// <param name="_">				メッシュの当たり判定を付けるかどうか</param>
	/// <param name="_putPlaceKind">	置いた場所の種類					</param>
	BackBlockBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, bool _isMeshCollision, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	virtual ~BackBlockBase() final;

private:

	ModelCollision* modelCollision = nullptr;	// メッシュの当たり判定
};
