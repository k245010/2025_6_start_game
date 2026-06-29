#include "ModelCollision.h"
#include "SphereCollision.h"
#include "RayCollision.h"

ModelCollision::ModelCollision(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, int _hModel, const std::string& _ownerName, std::function<bool(const CollisionHitInfoData&)> func, bool _onHitDeleteMe) : CollisionBase(_trans, _tagMe, _ownerName, func, _onHitDeleteMe)
{
	kind						= COLLISION_KIND::MODEL;
	collisionHitInfoData.kind	= COLLISION_KIND::MODEL;

	hModel						= _hModel;

#ifdef _DEBUG

	saveVecList.clear();

#endif
}

ModelCollision::~ModelCollision()
{
#ifdef _DEBUG

	saveVecList.clear();

#endif
}

void ModelCollision::Update()
{
	CollisionBase::Update();

}

void ModelCollision::Draw()
{
#ifdef _DEBUG

	for (const auto& saveVec : saveVecList)
	{
		DrawLine3D(saveVec.pos1, saveVec.pos2, 0xff0000);
	}

#endif // _DEBUG

}

bool ModelCollision::CollisionToSphere(SphereCollision* col)
{
	bool re = false;
	if (hModel < 0)
		assert(false);

	// モデルに座標を反映
	MV1SetMatrix(hModel, collTransform.trans->GetMatrix());
	MV1RefreshCollInfo(hModel);

	// 当たり判定
	MV1_COLL_RESULT_POLY_DIM ret = MV1CollCheck_Sphere(hModel, -1, col->GetPosition(), col->GetRadius());

	// 当たったかどうか
	if (ret.HitNum > 0)
	{
		std::vector<VECTOR3> hitPosList;	// 当たった座標
		std::vector<VECTOR3> hitNormList;	// 当たった法線コンテナ

		for (int i = 0;i < ret.HitNum;i++)
		{
			// 座標のpsuh
			hitPosList.emplace_back(ret.Dim[i].HitPosition);
			// 法線のpush
			hitNormList.emplace_back(ret.Dim[i].Normal);	
		}
		
		// 当たった場所のデータを代入
		collisionHitInfoData.hitPointData.emplace(hitPosList, hitNormList);

		re = true;
	}
	return re;
}

bool ModelCollision::CollisionToOBB(OBBCollision* col)
{
	return false;
}

bool ModelCollision::CollisionToRay(RayCollision* col)
{
#ifdef _DEBUG

	bool saveVec = false;


	if (col->GetCollisionHitInfoData().tag == COLLISION_OBJECT_KIND::BULLET)
		saveVec = true;
#endif
	bool isHit		= false;

	if (hModel < 0)
		return false;

	// モデルに座標を反映
	MV1SetMatrix(hModel, collTransform.trans->GetMatrix());
	MV1RefreshCollInfo(hModel);

	MATRIX mcopy = MV1GetMatrix(hModel);

	// レイの設定
	VECTOR3 pos1 = col->GetPosition() + col->GetCenter();
	VECTOR3 pos2 = col->GetPosition() + col->GetTarget();



	if (VAllSameF((pos1 - pos2), 0.0f))
		assert(false && "レイ設定がされていません");

	// 当たり判定
	MV1_COLL_RESULT_POLY ret = MV1CollCheck_Line(hModel, -1, pos1, pos2);

	// 当たったかどうか
	if (ret.HitFlag != 0)
	{
#ifdef _DEBUG
		if (saveVec)
			saveVecList.emplace_back(SaveVecData(pos1,pos2));
#endif // _DEBUG

		std::vector<VECTOR3> hitPos;		// 当たった座標
		std::vector<VECTOR3> hitNormList;	// 当たった法線コンテナ
		
		// Rayの場合は当たったデータは一つだけ存在
		
		// 座標をpush
		hitPos.emplace_back(ret.HitPosition);
		// 法線のpush
		hitNormList.emplace_back(ret.Normal);
	
		// 当たった場所のデータを代入
		collisionHitInfoData.hitPointData.emplace(hitPos, hitNormList);

		isHit = true;
	}
	return isHit;
}

bool ModelCollision::CollisionToModel(ModelCollision* col)
{
	return false;
}
