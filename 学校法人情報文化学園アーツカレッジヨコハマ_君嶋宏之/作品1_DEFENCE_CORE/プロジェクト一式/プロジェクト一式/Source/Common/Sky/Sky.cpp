#include "Sky.h"

namespace
{
	constexpr float ROTATION_SPEED = 0.025f;	// ‰ñ“]‘¬“x
}

Sky::Sky(const Transform& trans, const int& _hModel) : Object3D(trans,_hModel)
{
	angle = 0.0f;
}

Sky::~Sky()
{
}

void Sky::Update()
{
	angle += Time::GameDeltaTime() * ROTATION_SPEED;

	if (angle >= 360 * DegToRad)
	{
		angle -= 360 * DegToRad;
	}

	transform.rotation.y = angle;
}

void Sky::Draw()
{
	Object3D::Draw();
}
