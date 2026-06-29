#include "Physics.h"
#include "../Collision/CollisionFunction.h"
#include "../../ImGui/imgui.h"

namespace
{
	constexpr float GRAVITY = -44.1f;	// 重力値
}

Physics::Physics(Transform& _trans, VECTOR3& _vel, const VECTOR3& _fric, const float& _dis, bool _gravity)
{
	transform		= &_trans;
	velocity		= &_vel;
	friction		= _fric;
	maxDistance		= _dis;

	isGravity		= _gravity;
	gravityVelocity = VECTOR3(0.0f, GRAVITY, 0.0f);
}

Physics::~Physics()
{
}

void Physics::Update()
{
	lastTransform				= *transform;
	float maxDistanceDeltaTime	= maxDistance * Time::GameDeltaTime();

	// 最大速度を超えたら
	/*if (velocity->SquareSize() > (maxDistanceDeltaTime * maxDistanceDeltaTime))
	{
		VECTOR3 n = VECTOR3(velocity->x,0.0f,velocity->z);
		*velocity = n * maxDistanceDeltaTime;
	}*/

	/*VECTOR3 flatVec = VECTOR3(velocity->x, 0.0f, velocity->z);

	if (flatVec.SquareSize() > (maxDistanceDeltaTime * maxDistanceDeltaTime))
	{
		VECTOR3 n = flatVec.Normalize();
		velocity->x = n.x * maxDistanceDeltaTime;
		velocity->z = n.z * maxDistanceDeltaTime;
	}*/

	// 重力
	if (isGravity)
	{
		//gravityVelocity.y = gravityVelocity * Time::GameDeltaTime();

		// 重力速度の適用
		*velocity += gravityVelocity * Time::GameDeltaTime();
	}

	VECTOR3 copyVec = *velocity;

	// 抵抗値からヴェロシティを設定する //
	SetFrictionToVelocity(*velocity, friction);
	SetFrictionToVelocity(addVelocity, addVelocityFriction);

	velocity->y = copyVec.y;

	// 微小速度をカット（誤差防止）
	//if (fabs(velocity->y) < 0.01f)
	//	velocity->y = 0.0f;

	transform->position += *velocity + addVelocity;
}

void Physics::Draw()
{
#if _DEBUG

	//VECTOR3 vec = velocity->Normalize() * 100.0f;

	//DrawLine3D(transform->position, transform->position + vec, 0xff00ff);
	//DrawCone3D(transform->position + vec, transform->position, 10.0f, 16, 0xff00ff, 0xffffff,true);

	/*ImGui::Begin("Velocity");

	ImGui::SliderFloat3("velocity", &velocity->x, -10000.0f, 10000.0f);

	ImGui::End();*/

	//DrawFormatString(100, 500, 0xff0000, "Velocity x %.3lf, y %.3lf, z %.3lf", velocity->x, velocity->y, velocity->z);

#endif
}

void Physics::SetAddVelocity(const VECTOR3& _vel, const VECTOR3& _friction)
{
	addVelocity			= _vel;
	addVelocityFriction = _friction;
}

void Physics::AddForce(const VECTOR3& _vel)
{
	*velocity += _vel;
}

void Physics::SetFrictionToVelocity(VECTOR3& _vel, const VECTOR3& _friction)
{
	// 摩擦力
	VECTOR3 frictionPower = VMult(_vel.Normalize(), _friction) * Time::GameDeltaTime();

	for (int i = 0;i < 3;i++)
	{
		// 摩擦係数を超えたら
		if (GetVMemFabs(_vel, i) > GetVMemFabs(frictionPower, i))
		{
			// 速度を摩擦係数値で引く
			SetVMem(_vel, i, GetVMem(_vel, i) - GetVMem(frictionPower, i));
		}
		else
		{
			// 速度を０にする
			SetVMem(_vel, i, 0.0f);
		}
	}
}
