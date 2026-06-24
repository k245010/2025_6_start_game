#pragma once
#include "../../../Library/gameObject.h"
#include "CollisionBase.h"
//#include "../CharaBase.h"
#include <list>
#include <vector>

/// <summary>
/// Modelの当たり判定
/// </summary>
class ModelCollision : public CollisionBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_trans"> トランスフォームポインタ </param>
	/// <param name="_tagMe"> 自身の当たり判定オブジェクトタグ </param>
	/// <param name="_hModel"> モデルハンドル </param>
	/// <param name="_ownerName"> 自身の当たり判定オブジェクト名 </param>
	/// <param name="func"> あたったときに呼ぶ関数 </param>
	/// <param name="_onHitDeleteMe"> あたったときに自身の当たり判定オブジェクトを非アクティブにするかどうか　非アクティブにする : 非アクティブにしない </param>
	ModelCollision(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, int _hModel, const std::string& _ownerName = "", std::function<bool(const CollisionHitInfoData&)> func = nullptr, bool _onHitDeleteMe = false);
	~ModelCollision();

	void Update() override;
	void Draw() override;

	/// <summary>
	/// 自身と球の当たり判定をする
	/// </summary>
	/// <param name="col"> SphereCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToSphere(SphereCollision* col) override;

	/// <summary>
	/// 自身とOBBの当たり判定をする
	/// </summary>
	/// <param name="col"> ModelCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToOBB(OBBCollision* col) override;

	/// <summary>
	/// 自身とレイの当たり判定をする
	/// </summary>
	/// <param name="col"> RayCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToRay(RayCollision* col) override;

	/// <summary>
	/// 自身とモデルの当たり判定をする
	/// </summary>
	/// <param name="col"> ModelCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToModel(ModelCollision* col) override;

private:

	int hModel;	// モデルハンドル
#ifdef _DEBUG

	struct SaveVecData
	{
		SaveVecData() : pos1(VZero), pos2(VZero) {}
		SaveVecData(VECTOR3 _pos1, VECTOR3 _pos2) : pos1(_pos1), pos2(_pos2) {}

		VECTOR3 pos1;
		VECTOR3 pos2;
	};

	std::list<SaveVecData> saveVecList;
#endif
};