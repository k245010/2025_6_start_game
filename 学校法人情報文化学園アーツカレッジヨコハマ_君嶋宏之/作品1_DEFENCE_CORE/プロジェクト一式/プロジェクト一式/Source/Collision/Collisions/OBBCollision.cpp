#include "obbCollision.h"
#include "sphereCollision.h"
#include "rayCollision.h"

OBBCollision::OBBCollision(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, const std::string& _ownerName, std::function<bool(const CollisionHitInfoData&)> func, bool _onHitDeleteMe) : CollisionBase(_trans, _tagMe,_ownerName, func, _onHitDeleteMe)
{
	kind						= COLLISION_KIND::OBB;
	collisionHitInfoData.kind	= COLLISION_KIND::OBB;

	collTransform.lastPosition	= VGet(700, 0, 0);

}

OBBCollision::~OBBCollision()
{

}

void OBBCollision::Update()
{
	CollisionBase::Update();
	
}

void OBBCollision::Draw()
{
	// 当たり判定の描画
	VECTOR pos	= GetPosition();
	VECTOR numV = GetHalfLen(); 
	VECTOR min	= VSub(pos, numV);
	VECTOR max	= VAdd(pos, numV);
	DrawCube3D(min, max, 0xff00ff, 0xfffff, false);

}

bool OBBCollision::CollisionToSphere(SphereCollision* col)
{
	return col->CollisionToOBB(this);
}

bool OBBCollision::CollisionToRay(RayCollision* col)
{
	return col->CollisionToOBB(this);
}

bool OBBCollision::CollisionToModel(ModelCollision* col)
{
	return false;
}

bool OBBCollision::CollisionToOBB(OBBCollision* col)
{
	return false;
}

const VECTOR3 OBBCollision::GetVDirection(int member) const
{
	VECTOR3 vec = VZero;

	SetVMem(vec, member, 1);			// 方向ベクトル

	VECTOR3 rot		= GetRotation();	// 回転値
	MATRIX mRot		= MGetIdent();		// 回転行列
	mRot			= mRot * MGetRotZ(rot.z);
	mRot			= mRot * MGetRotX(rot.x);
	mRot			= mRot * MGetRotY(rot.y);

	vec				= vec * mRot;		// 回転行列で方向ベクトルを設定

	return vec;
}
