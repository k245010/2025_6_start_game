#include "noGravityEnemyBase.h"

NoGravityEnemyBase::NoGravityEnemyBase(SceneBase* _scene):EnemyBase(_scene)
{
	gActive = false;
}

NoGravityEnemyBase::~NoGravityEnemyBase()
{
}

MATRIX NoGravityEnemyBase::GetRotateMatrix()
{
	MATRIX mRotateX = MGetRotX(rotate.x);
	MATRIX mRotateY = MGetRotY(rotate.y);
	MATRIX mRotateZ = MGetRotZ(rotate.z);

	MATRIX mRotate = MMult(MMult(mRotateX, mRotateY), mRotateZ);

	return mRotate;
}

MATRIX NoGravityEnemyBase::MoveRotateMatrix()
{
	MATRIX mRotateX = MGetRotX(moveRotate.x);
	MATRIX mRotateY = MGetRotY(moveRotate.y);
	MATRIX mRotateZ = MGetRotZ(moveRotate.z);

	MATRIX mRotate = MMult(MMult(mRotateX, mRotateY), mRotateZ);

	return mRotate;
}
