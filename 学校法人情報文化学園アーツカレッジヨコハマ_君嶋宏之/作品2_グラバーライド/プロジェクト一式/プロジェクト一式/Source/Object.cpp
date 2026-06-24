#include"Object.h"
#include "debugScreen.h"
#include "sphereCollision.h"
#include "obbCollision.h"
#include "effectManager.h"
#include "commonFind.h"
#include "loadManager.h"
#include "stageObjectManager.h"

Object::Object(SceneBase* _scene) :GameObject(_scene)
{
	position = VGet(0, 0, 0);
	scale = VGet(1.0f, 1.0f, 1.0f);
	rotate = VGet(0, 0, 0);
	size = VOne * 1.0f;

	commonFind = SceneManager::CommonScene()->FindGameObject<CommonFind>();
	active = true;
	loadManager = commonFind->CommonFindObject<LoadManager>();


	/*slowMostionManager = commonFind->CommonFindObject<SlowMostionManager>();
	slowMostionManager->SlowObject(this);*/
	debugStop = false;

	gravityVelocity = BASE_GRAVITY_VELOCITY;
	gravityManager = GetScene()->FindGameObject<GravityManager>();
	gravityRotation = 0.0f;
	for (int i = 0; i < G_DIRECTION_NUM; i++)
	{
		canGravityDirection[i] = false;
	}
	//canGravityDirection[G_DOWN] = true;
	gravityAdd = BASE_GRAVITY_VELOCITY;

	gActive = false;

	sphereCollision = new SphereCollision(this);
	obbCollision = new OBBCollision(this);

	gravityAddRate = 1.0f;
	objectGravityRate = 1.0f;

	rgb.SetColor(255, 255, 255);
	effectManager = _scene->FindGameObject<EffectManager>();

	gameState = GameManager::PLAY_BEFORE;
	velocity = VGet(0.0f, 0.0f, 0.0f);

	moveMax = MaxGet(-15.0f, -20.0f);
	gravityManager->CreateGravityObject(this);
	visionMove = true;
	alpha = 1.0f;
}

Object::~Object()
{

	//gravityManager->RemoveGravityObject(this);
	if (sphereCollision != nullptr)
	{
		delete sphereCollision;
		sphereCollision = nullptr;
	}
	if (obbCollision != nullptr)
	{
		delete obbCollision;
		obbCollision = nullptr;
	}
}

void Object::Update()
{
	if (debugSlowMode) {
		slowModeCounter++;
		if (slowModeCounter % 8 == 0) {
			debugStop = false;
		}
		else {
			debugStop = true;
			return;
		}
	}
	else {
		debugStop = false;
	}
	velocity = VGet(0.0f, 0.0f, 0.0f);
}

void Object::Draw()
{
	if (CheckCameraViewClip_Box(position - VGet(size.x * scale.x, size.y * scale.y, size.z * scale.z), position + VGet(size.x * scale.x, size.y * scale.y, size.z * scale.z))) {
		return;
	}
	if (hModel > 0) {
		MV1SetMatrix(hModel, GetMatrix());
		MV1SetDifColorScale(hModel,rgb.ColorF(alpha));
		MV1DrawModel(hModel);
	}
}

MATRIX Object::GetMatrix()
{
	MATRIX mRotate = GetRotateMatrix();

	MATRIX mScale = MGetScale(scale);

	VECTOR drawPosition;

	drawPosition = VAdd(shakePos, position);

	MATRIX mTransrate = MGetTranslate(drawPosition);

	MATRIX matrix = mScale * mRotate * mTransrate;

	return matrix;
}

MATRIX Object::GetAftimageMatrix(VECTOR _position, VECTOR _rotate, VECTOR _scale)
{
	MATRIX mRotate = GetRotateMatrix(_rotate);

	MATRIX mScale = MGetScale(_scale);

	VECTOR drawPosition = _position;

	//drawPosition = VAdd(shakePos, _position);

	MATRIX mTransrate = MGetTranslate(drawPosition);

	MATRIX matrix = mScale * mRotate * mTransrate;

	return matrix;
}


//MATRIX Object::GetMatrix(VECTOR _pos)
//{
//	/*MATRIX mRotateX = MGetRotAxis(_pos,rotate.x);
//	MATRIX mRotateY = MGetRotAxis(_pos,rotate.y);
//	MATRIX mRotateZ = MGetRotAxis(_pos,rotate.z);*/
//	
//	
//	//MATRIX mRotate = MGetAxis2(rotate, rotate, rotate, position);
//	MATRIX mRotate = GetRotateAxisMatrix();
//
//	MATRIX mScale = MGetScale(scale);
//
//	MATRIX mTransrate = MGetTranslate(position);
//
//	MATRIX matrix = MMult(mRotate, mScale);
//
//	VECTOR rotatePosition = VGet(position.x, position.y, position.z);
//
//	rotatePosition.x += 5000.0f;
//
//	matrix = MGetAxis1(VGet(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]),
//		VGet(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]),
//		VGet(matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]), rotatePosition);
//
//	matrix.m[3][0] = mTransrate.m[3][0];
//	matrix.m[3][1] = mTransrate.m[3][1];
//	matrix.m[3][2] = mTransrate.m[3][2];
//
//	return matrix;
//}

MATRIX Object::GetRotateMatrix()
{
	MATRIX mRotateX = MGetRotX(rotate.x);
	MATRIX mRotateY = MGetRotY(rotate.y);
	MATRIX mRotateZ = MGetRotZ(rotate.z);

	MATRIX mRotate = MMult(MMult(mRotateX, mRotateZ), mRotateY);

	return mRotate;
}

MATRIX Object::GetRotateMatrix(VECTOR _rotate)
{
	MATRIX mRotateX = MGetRotX(_rotate.x);
	MATRIX mRotateY = MGetRotY(_rotate.y);
	MATRIX mRotateZ = MGetRotZ(_rotate.z);

	MATRIX mRotate = MMult(MMult(mRotateX, mRotateZ), mRotateY);

	return mRotate;
}

//MATRIX Object::GetRotateAxisMatrix()
//{
//	MATRIX mRotateX = MGetRotX(rotate.x);
//	MATRIX mRotateY = MGetRotY(rotate.y);
//	MATRIX mRotateZ = MGetRotZ(rotate.z);
//
//	MATRIX mRotate = MMult(MMult(mRotateZ, mRotateX), mRotateY);
//
//	/*/mRotate = MGetAxis1(VGet(mRotate.m[0][0], mRotate.m[0][1], mRotate.m[0][2]),
//		VGet(mRotate.m[1][0], mRotate.m[1][1], mRotate.m[1][2]),
//		VGet(mRotate.m[2][0], mRotate.m[2][1], mRotate.m[2][2]), position);*/
//	return mRotate;
//}

VECTOR Object::Transform(VECTOR addPos)
{
	return VAdd(velocity,VTransform(addPos, GetRotateMatrix()));
}

VECTOR Object::Transform(VECTOR _velocity, VECTOR addPos)
{
	return VAdd(_velocity, VTransform(addPos, GetRotateMatrix()));
}

//void Object::SetGravityRotation(GravityState _state)
//{
//	GravityState gravityState = gravityManager->GetGravityState();
//	switch (gravityState)
//	{
//	case DOWN_GRAVITY:
//	case DOWN_GRAVITY_START:
//		if (canGravityDirection[G_DOWN])
//		{
//			gravityRotation = 0.0f;
//		}
//		break;
//	case DOWN_RIGHT_GRAVITY:
//	case DOWN_RIGHT_GRAVITY_START:
//		if (canGravityDirection[G_DOWN_RIGHT])
//		{
//			gravityRotation = DX_PI_F / 4;
//		}
//		break;
//	case RIGHT_GRAVITY:
//	case RIGHT_GRAVITY_START:
//		if (canGravityDirection[G_RIGHT])
//		{
//			gravityRotation = DX_PI_F / 2;
//		}
//		break;
//	case UP_RIGHT_GRAVITY:
//	case UP_RIGHT_GRAVITY_START:
//		if (canGravityDirection[G_UP_RIGHT])
//		{
//			gravityRotation = DX_PI_F / 1.5f;
//		}
//		break;
//	case UP_GRAVITY:
//	case UP_GRAVITY_START:
//		if (canGravityDirection[G_UP])
//		{
//			gravityRotation = DX_PI_F;
//		}
//		break;
//	case UP_LEFT_GRAVITY:
//	case UP_LEFT_GRAVITY_START:
//		if (canGravityDirection[G_UP_LEFT])
//		{
//			gravityRotation = -DX_PI_F / 1.5f;
//		}
//		break;
//	case LEFT_GRAVITY:
//	case LEFT_GRAVITY_START:
//		if (canGravityDirection[G_LEFT])
//		{
//			gravityRotation = -DX_PI_F / 2;
//		}
//		break;
//	case DOWN_LEFT_GRAVITY:
//	case DOWN_LEFT_GRAVITY_START:
//		if (canGravityDirection[G_DOWN_LEFT])
//		{
//			gravityRotation = -DX_PI_F / 4;
//		}
//		break;
//	default:
//		break;
//	}
//	
//	gravityAdd = VAdd(gravityAdd, VGet(0, -0.2f * gravityAddRate, 0));
//	if (gravityState % 2 == 0) {
//		if (moveMax.normalGravityAddMax >= gravityAdd.y) {
//			gravityAdd.y = moveMax.normalGravityAddMax;
//		}
//		if (type == PLAYER) {
//			float a = 0;
//		}
//	}
//	else {
//		if (moveMax.playerChangeGravityAddMax >= gravityAdd.y) {
//			gravityAdd.y = moveMax.playerChangeGravityAddMax;
//		}
//		if (type == PLAYER) {
//			float a = 0;
//		}
//	}
//	
//}

void Object::SetGravityRotation(GravityState _state,float _worldGravity)
{
	GravityState gravityState = _state;
	switch (gravityState)
	{
	case DOWN_GRAVITY:
	case DOWN_GRAVITY_START:
		if (canGravityDirection[G_DOWN])
		{
			gravityRotation = 0.0f;
		}
		break;
	case DOWN_RIGHT_GRAVITY:
	case DOWN_RIGHT_GRAVITY_START:
		if (canGravityDirection[G_DOWN_RIGHT])
		{
			gravityRotation = DX_PI_F / 4;
		}
		break;
	case RIGHT_GRAVITY:
	case RIGHT_GRAVITY_START:
		if (canGravityDirection[G_RIGHT])
		{
			gravityRotation = DX_PI_F / 2;
		}
		break;
	case UP_RIGHT_GRAVITY:
	case UP_RIGHT_GRAVITY_START:
		if (canGravityDirection[G_UP_RIGHT])
		{
			gravityRotation = DX_PI_F / 2 + DX_PI_F / 4;
		}
		break;
	case UP_GRAVITY:
	case UP_GRAVITY_START:
		if (canGravityDirection[G_UP])
		{
			gravityRotation = DX_PI_F;
		}
		break;
	case UP_LEFT_GRAVITY:
	case UP_LEFT_GRAVITY_START:
		if (canGravityDirection[G_UP_LEFT])
		{
			gravityRotation = -DX_PI_F / 2 - DX_PI_F / 4;
		}
		break;
	case LEFT_GRAVITY:
	case LEFT_GRAVITY_START:
		if (canGravityDirection[G_LEFT])
		{
			gravityRotation = -DX_PI_F / 2;
		}
		break;
	case DOWN_LEFT_GRAVITY:
	case DOWN_LEFT_GRAVITY_START:
		if (canGravityDirection[G_DOWN_LEFT])
		{
			gravityRotation = -DX_PI_F / 4;
		}
		break;
	default:
		break;
	}

	gravityRotation += _worldGravity;

	
}

void Object::GravityAddSpeed(GravityState _state)
{
	GravityState gravityState = _state;
	gravityAdd = VAdd(gravityAdd, VGet(0, -0.2f * gravityAddRate, 0));
	if (gravityState % 2 == 0) {
		if (moveMax.normalGravityAddMax >= gravityAdd.y) {
			gravityAdd.y = moveMax.normalGravityAddMax;
		}
		if (type == (int)ObjectType::PLAYER) {
			float a = 0;
		}
	}
	else {
		if (moveMax.playerChangeGravityAddMax >= gravityAdd.y) {
			gravityAdd.y = moveMax.playerChangeGravityAddMax;
		}
		if (type == (int)ObjectType::PLAYER) {
			float a = 0;
		}
	}
}

void Object::GravityMove()
{
	if (gravityManager->GetGravityState() != NO_GRAVITY)
	{
		MATRIX mRotZ = MGetRotZ(gravityRotation);
		gravityVelocity = VTransform(gravityAdd, mRotZ);
		gravityVelocity = VScale(gravityVelocity,objectGravityRate);
		position = VAdd(position, gravityVelocity);
	}

}

void Object::GravityMove(GravityState _state)
{
	if (_state != NO_GRAVITY)
	{
		MATRIX mRotZ = MGetRotZ(gravityRotation);
		gravityVelocity = VTransform(gravityAdd, mRotZ);
		gravityVelocity = VScale(gravityVelocity, objectGravityRate);
		position = VAdd(position, gravityVelocity);
	}
}

void Object::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate,int _type,int _collisionId)
{
	hModel = _hModel;
	position = _position;
	scale = _scale;
	rotate = _rotate;
	type = _type;
	collisionId = _collisionId;
}

void Object::Init(VECTOR _position, VECTOR _scale, VECTOR _rotate)
{
	position = _position;
	scale = _scale;
	rotate = _rotate;
}



void Object::CollisionEvent(CollisionBase& me, CollisionBase& target)
{
	
}

bool Object::HitStopStart(HitStopState _hit)
{
	if (hitStop.time > 0) {
		return false;
	}
	if (hitStop.time == -1 || shake.time == -1) {
		StopShake();
	}
	hitStop = _hit;
	moveCan = false;
	return true;
}

bool Object::ShakeStart(ShakeState _shake, bool _moveCan)
{
	if (shake.time > 0) {
		return false;
	}
	if (hitStop.time == -1 || shake.time == -1) {
		StopShake();
	}
	shake = _shake;
	moveCan = _moveCan;
	return true;
}

void Object::HitStopShake()
{
	if (hitStop.time == -1) {
		shakePos = VGet(GetRand((int)hitStop.power * 2) - hitStop.power, GetRand((int)hitStop.power * 2) - hitStop.power, 0);
	}
	else if (shake.time == -1) {
		shakePos = VGet(GetRand((int)shake.power * 2) - shake.power, GetRand((int)shake.power * 2) - shake.power, 0);
	}
	else if (hitStop.time > 0) {
		hitStop.time--;
		shakePos = VGet(GetRand((int)hitStop.power * 2) - hitStop.power, GetRand((int)hitStop.power * 2) - hitStop.power, 0);
		if (hitStop.time == 0) {
			shakePos = VGet(0.0f, 0.0f, 0.0f);
		}
	}
	else {
		shake.time--;
		shakePos = VGet(GetRand((int)shake.power * 2) - shake.power, GetRand((int)shake.power * 2) - shake.power, 0);
		if (shake.time == 0) {
			shakePos = VGet(0.0f, 0.0f, 0.0f);
		}
	}
}

void Object::StopShake()
{
	if (hitStop.time == -1 || hitStop.time > 0) {
		hitStop.time = 0;
		hitStop.power = 0;
	}
	else if (shake.time == -1 || shake.time > 0) {
		shake.time = 0;
		shake.power = 0;
	}
	shakePos = VGet(0.0f, 0.0f, 0.0f);
}

void Object::ControlVibrationStart(ControlVibration _vibration)
{
	StartJoypadVibration(DX_INPUT_PAD1,_vibration.power,_vibration.time,-1);
}

int Object::GameStateChange(GameManager::GAME_STATE _state)
{
	if (gameState == _state) {
		return -1;
	}
	gameState = _state;
	return static_cast<int>(gameState);
}

int Object::GroundLanding()
{
	gravityAdd = BASE_GRAVITY_VELOCITY;
	return 1;
}

void Object::RemoveObject()
{
	gravityManager->RemoveGravityObject(this);
}

void Object::RemoveSlowObject()
{
	//slowMostionManager->RemoveSlowObject(this);
}

