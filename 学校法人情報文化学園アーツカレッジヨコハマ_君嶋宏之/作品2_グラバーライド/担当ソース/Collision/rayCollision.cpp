#include "rayCollision.h"
#include "function.h"
#include "obbCollision.h"
#include "player.h"

RayCollision::RayCollision(Object* obj) : CollisionBase(obj)
{
	if (obj->GetObjectType() != (int)ObjectType::PLAYER)
		CreateMessageBox("プレイヤー以外でRayCollisionは使えません", "RayCollisionのnewでエラー");

	object = obj;
	kind = COLLISION_KIND::RAY;

	directionKind = (RAY_DIRECTION_KIND)-1;

	rayDirection = VGet(0.0f);
	reRay = VGet(0.0f);

	player = dynamic_cast<Player*>(obj);
}

RayCollision::~RayCollision()
{
}

void RayCollision::Draw()
{
	DrawCone3D(player->GetRayTarget(), VSub(player->GetRayTarget(), VScale(rayDirection, 250.0f)), 50, 20,GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
}

bool RayCollision::CollisionToOBB(OBBCollision* col)
{
	if (directionKind == (RAY_DIRECTION_KIND)-1)
		CreateMessageBox("レイのenumの初期化がされてません", "RayCollisionのCollisionToOBBでエラー");

	ResetRayDirection();
	rayDirection *= player->GetRotateMatrix();
	int index = GetVMemIndexMax(GetVFabs(rayDirection));
	if (index == 2) // playerの右はxではなくzだから、zの値をxに代入
		SetFSwap(rayDirection.x, rayDirection.z);
	
	if (col->GetLen().x < 0.0f)
		return false;

	if (VAllSameF(rayDirection, 0.0f))
		return false;

#if 1
	// 円の中心点が原点になるように始点をオフセット
	VECTOR sPos = VSub(player->GetRayTarget(), col->GetPosition());

	VECTOR vel = VNorm(rayDirection);

	// 係数tを算出
	float dotSVec = VDot(sPos, vel);
	float dotSS = VDot(sPos, sPos);
	float s = dotSVec * dotSVec - dotSS + col->GetLen().x * col->GetLen().x;
	if (fabsf(s) < 0.000001f)
		s = 0.0f;// 誤差修正

	if (s < 0.0f)
		return false;// 衝突していない

	float sq = sqrtf(s);
	float t1 = -dotSVec - sq;
	float t2 = -dotSVec + sq;

	// もしt1及びt2がマイナスだったら始点が
	// 円内にめり込んでいるのでエラーとする
	if (t1 < 0.0f || t2 < 0.0f)
		return false;

	reRay = VScale(rayDirection, VSize(sPos) - (col->GetLen().x * 1.1f)); 
	rayDirection = VScale(rayDirection,VSize(sPos));
	return true;
#else

#endif
}

void RayCollision::ResetRayDirection()
{
	switch (directionKind)
	{
	case RAY_DIRECTION_KIND::SIDE: //プレイヤーの逆進行方向はレイをとばさない
		rayDirection = VGet(1, 0, 0);
		break;
	case RAY_DIRECTION_KIND::UP:
		rayDirection = VGet(0, 1, 0);
		break;
	case RAY_DIRECTION_KIND::DOWN:
		rayDirection = VGet(0, -1, 0);
		break;
	}
}
