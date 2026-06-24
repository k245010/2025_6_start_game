#include "rayCollision.h"
#include "obbCollision.h"
#include "SphereCollision.h"
#include "ModelCollision.h"
//#include "../../Player.h"

RayCollision::RayCollision(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, const std::string& _ownerName, std::function<bool(const CollisionHitInfoData&)> func, bool _onHitDeleteMe) : CollisionBase(_trans, _tagMe,_ownerName, func, _onHitDeleteMe)
{
	kind						= COLLISION_KIND::RAY;
	collisionHitInfoData.kind	= COLLISION_KIND::RAY;

	rayVec						= VZero;
}

RayCollision::~RayCollision()
{
}

void RayCollision::Draw()
{
	//int color = GetColor(GetRand(255), GetRand(255), GetRand(255));

	DrawLine3D(collTransform.trans->position, collTransform.trans->position + rayVec, 0x00ff00);

	//DrawCone3D(player->GetRayTarget(), VSub(player->GetRayTarget(), VScale(rayDirection, 250.0f)), 50, 20,GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
}

bool RayCollision::CollisionToSphere(SphereCollision* col)
{
	if (VAllSame(rayVec, VZero))
		assert(false);

	return col->CollisionToRay(this);
}

bool RayCollision::CollisionToOBB(OBBCollision* col)
{
	//return RayToSphere(GetPosition(), rayVec, col->GetPosition(), col->GetLen().x, rayVec.Size());

	if (VAllSame(rayVec, VZero))
		assert(false);


	//__ レイの設定 __//

	// 光線を境界ボックスの空間へ移動
	MATRIX mat = col->GetTransform()->GetMatrix();

	// 始点
	VECTOR3 pRay = collTransform.trans->position * mat;

	for (int i = 0;i < 3;i++)
	{
		// 座標をリセットする (0.0f
		mat.m[3][i] = 0.0f;
	}
	// 方向ベクトル
	VECTOR3 dirRay = rayVec * mat;


	//__ 交差判定 __//

	VECTOR3 p = pRay;
	VECTOR3 d = dirRay;
	VECTOR3 min = col->GetPosition() - col->GetHalfLen();
	VECTOR3 max = col->GetPosition() + col->GetHalfLen();

	float t = -FLT_MAX;
	float tMax = FLT_MAX;

	for (int i = 0;i < 3;i++)
	{
		if (GetVMemFabs(d, i) < EPSILON_NUM)
		{
			float t1 = 0.0f;
			float t2 = 0.0f;
			if (GetVMem(p, i) < GetVMem(min, i) || GetVMem(p, i) > GetVMem(max, i))
			{
				// 交差していない
				return false;
			}
			else
			{
				// スラブとの距離を算出
				// t1が近スラブ、t2が遠スラブとの距離
				float odd = 1.0f / GetVMem(d, i);
				t1 = (GetVMem(min, i) - GetVMem(p, i)) * odd;
				t2 = (GetVMem(max, i) - GetVMem(p, i)) * odd;

				if (t1 > t2)
				{
					float tmp = t1;
					t1 = t2;
					t2 = tmp;
				}
			}

			if (t1 > t)
				t = t1;

			if (t2 < tMax)
				tMax = t2;

			// スラブ交差チェック
			if (t >= tMax)
			{
				// 交差していない
				return false;
			}
		}
	}

	// 交差している
	return true;
}

bool RayCollision::CollisionToModel(ModelCollision* col)
{
	if (VAllSame(rayVec, VZero))
		assert(false);

	return col->CollisionToRay(this);
}
