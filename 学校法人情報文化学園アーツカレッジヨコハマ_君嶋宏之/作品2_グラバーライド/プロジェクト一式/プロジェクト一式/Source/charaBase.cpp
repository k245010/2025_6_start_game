#include "charaBase.h"
#include "sphereCollision.h"

CharaBase::CharaBase(SceneBase* _scene) :Object(_scene)
{
	noDamageTime = -1;
	damageNum = -1;
	jump = false;
	charaState = NORMAL;

	attackSphereCollision = new SphereCollision(this);

	
	
}

CharaBase::~CharaBase()
{
	delete attackSphereCollision;
}

void CharaBase::Update()
{
	Object::Update();
	if (noDamageTime > 0) {
		noDamageTime--;
		if (noDamageTime % 10 >= 5) {
			rgb.SetColor(255, 0, 0);
		}
		else {
			rgb.SetColor(255, 255, 255);
		}
		if (noDamageTime == 0) {
			rgb.SetColor(255, 255, 255);
		}
	}
}

void CharaBase::Draw()
{
#if _DEBUG
	DrawSphere3D(position, 20, 10, 0xffffff, 0xffffff, true);
	DrawSphere3D(centerPosition, 20, 10, 0xffff00, 0xffffff, true);
	DrawSphere3D(headPosition, 40, 1, 0xffffff, 0xffffff, false);
#endif // _DEBUG

	
}

void CharaBase::GravityMove(GravityState _state)
{
	if (_state != NO_GRAVITY)
	{
		MATRIX mRotZ = MGetRotZ(gravityRotation);
		gravityVelocity = VTransform(gravityAdd, mRotZ);
		gravityVelocity = VScale(gravityVelocity, objectGravityRate);

		
		centerPosition = VAdd(centerPosition, gravityVelocity);
	}
}

void CharaBase::DirectionChange(float _rotate, Direction _dire)
{
	rotate.y = _rotate;
	direction = _dire;
}

bool CharaBase::Damage(float _damage, int _invencible)
{
	if (debugNoDamage) {
		return false;
	}
	if (noDamageTime > 0 || hp <= 0) {
		return false;
	}
	hp -= (int)_damage;
	noDamageTime = _invencible;
	return true;
}

MATRIX CharaBase::GetMatrix()
{
	MATRIX mRotate = GetRotateMatrix();

	MATRIX mScale = MGetScale(scale);

	VECTOR drawPosition;

	drawPosition = VAdd(shakePos, position);

	MATRIX mTransrate = MGetTranslate(drawPosition);

	MATRIX matrix = MMult(MMult(mScale, mRotate), mTransrate);

	return matrix;
}


VECTOR CharaBase::MoveTransform(VECTOR _velo, VECTOR _addPos)
{
	return  VAdd(_velo, VTransform(_addPos, MoveRotateMatrix()));
}

MATRIX CharaBase::MoveRotateMatrix()
{
	MATRIX mRotateX = MGetRotX(moveRotate.x);
	MATRIX mRotateY = MGetRotY(moveRotate.y);
	MATRIX mRotateZ = MGetRotZ(moveRotate.z);

	MATRIX mRotate = MMult(MMult(mRotateX, mRotateZ), mRotateY);

	return mRotate;
}

void CharaBase::PositionMove(VECTOR _addPos)
{
	VECTOR posi = centerPosition;
	VECTOR mo = moveRotate;
	VECTOR baseVelocity = VGet(0.0f, 0.0f, 0.0f);
	baseVelocity = MoveTransform(baseVelocity, _addPos);
	velocity = VAdd(baseVelocity, velocity);
	centerPosition = VAdd(baseVelocity, centerPosition);
}

void CharaBase::PositionSetDraw()
{
	position = VAdd(centerPosition, VTransform(VGet(0.0f, -centerPositionAmong * scale.y, 0), GetRotateMatrix()));
	headPosition = VAdd(centerPosition, VTransform(VGet(0.0f, centerPositionAmong * scale.y, 0), GetRotateMatrix()));
	landPosition = VAdd(centerPosition, VTransform(VGet(0.0f, -landPositionAdd * scale.y, 0), GetRotateMatrix()));
}

void CharaBase::CharaRotationSet()
{
	float rotatePlayer = gravityRotation;
	if (direction == RIGHT) {
		rotate = VGet(rotatePlayer, -DX_PI_F / 2, 0.0f);
	}
	else {
		rotate = VGet(-rotatePlayer, DX_PI_F / 2, 0.0f);
	}
	moveRotate = rotate;
}

void CharaBase::AnimeInit()
{

}
