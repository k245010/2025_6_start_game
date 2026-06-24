#pragma once
#include "../StageObjectBase.h"
#include "../../StageObjectData.h"
#include "../../StageInfo.h"

class SphereCollision;
class OBBCollision;
struct CollisionHitInfoData;
/// <summary>
/// ブロックの基底クラス
/// </summary>
class BlockBase : public StageObjectBase
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_modelData">		モデルハンドル					</param>
	/// <param name="_kind">			ステージオブジェクトの種類		</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	BlockBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp,const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	virtual ~BlockBase();

	virtual void Update() override;
	virtual void Draw() override;

	const Transform* GetTransformPtr() { return &transform; }

protected:


	/// <summary>
	/// ブロックが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	virtual bool HitBlock(const CollisionHitInfoData& _data) { return false; }

	int rawModelHandle = -1;					// マトリックス計算していないモデルハンドル
	
	SphereCollision* sphereColl;				// 球の当たり判定クラスポインタ
	OBBCollision* obbColl;						// OBBの当たり判定クラスポインタ
};