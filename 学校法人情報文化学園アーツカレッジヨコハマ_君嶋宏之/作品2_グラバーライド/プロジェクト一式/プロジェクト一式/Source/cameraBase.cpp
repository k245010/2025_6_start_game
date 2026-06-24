#include "cameraBase.h"
#include "../ImGui/imgui.h"

namespace {
	const float CAMERA_MOVE_POS = 5.0f;
}

CameraBase::CameraBase(SceneBase* _scene) : GameObject(_scene)
{
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	debugPosition = V2Get(0, 0);
	rate = 1.0f;
	SetCameraNearFar(50.0f, 50000.0f);
}

CameraBase::~CameraBase()
{

}

void CameraBase::Update()
{
	DebugModeChange();
	if (CheckHitKey(KEY_INPUT_K)) {
		SetDrawScreen(DX_SCREEN_BACK);
	}
	switch (debugMode)
	{
	case CameraBase::NORMAL_MODE:

		break;
	case CameraBase::DEBUG_MODE:
		
		ImGui::Begin("cameraMove");
		ImGui::SliderFloat("rate", &rate, 1.0f, 100.0f);
		
		if (CheckHitKey(KEY_INPUT_LSHIFT)) {
			if (CheckHitKey(KEY_INPUT_W)) TargetMoveForward(CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_S)) TargetMoveForward(-CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_A)) TargetMoveRight(-CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_D)) TargetMoveRight(CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_Q))TargetMoveUp(CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_E))TargetMoveUp(-CAMERA_MOVE_POS * rate);
		}
		else if(CheckHitKey(KEY_INPUT_LCONTROL)){
			if (CheckHitKey(KEY_INPUT_W)) MoveForward(CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_S)) MoveForward(-CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_A)) MoveRight(-CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_D)) MoveRight(CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_Q))MoveUp(CAMERA_MOVE_POS * rate);
			if (CheckHitKey(KEY_INPUT_E))MoveUp(-CAMERA_MOVE_POS * rate);
		}
		else {
			if (CheckHitKey(KEY_INPUT_W)) {
				MoveForward(CAMERA_MOVE_POS * rate);
				TargetMoveForward(CAMERA_MOVE_POS * rate);
			}
			if (CheckHitKey(KEY_INPUT_S)) {
				MoveForward(-CAMERA_MOVE_POS * rate);
				TargetMoveForward(-CAMERA_MOVE_POS * rate);
			}
			if (CheckHitKey(KEY_INPUT_A)) {
				MoveRight(-CAMERA_MOVE_POS * rate);
				TargetMoveRight(-CAMERA_MOVE_POS * rate);
			}
			if (CheckHitKey(KEY_INPUT_D)) {
				MoveRight(CAMERA_MOVE_POS * rate);
				TargetMoveRight(CAMERA_MOVE_POS * rate);
			}
			if (CheckHitKey(KEY_INPUT_Q)) {
				MoveUp(CAMERA_MOVE_POS * rate);
				TargetMoveUp(CAMERA_MOVE_POS * rate);
			}
			if (CheckHitKey(KEY_INPUT_E)) {
				MoveUp(-CAMERA_MOVE_POS * rate);
				TargetMoveUp(-CAMERA_MOVE_POS * rate);
			}

		}
		ImGui::End();
		break;
	}
	Apply();
	
}

void CameraBase::Draw()
{
	

#ifdef _DEBUG
	
	if (debugMode == DEBUG_MODE) {
		ImGui::Begin("CAMERA");
		ImGui::Text("P.x = %.3f\n\n P.y = %.3f \n\nP.z = %.3f\n\n\n\n", position.x, position.y, position.z);
		ImGui::Text("T.x = %.3f\n\n T.y = %.3f \n\nT.z = %.3f", target.x, target.y, target.z);
		ImGui::End();
	}
		//DrawSphere3D(target, 50.0f, 2, 0xffffff, 0xffffff, false);
#endif

		
}

// カメラを適用する
void CameraBase::Apply() {
	//target = VAdd(VGet(0.0f, 0.0f, 50.0f), position);
	//target = player->GetCenterPosition();
	SetCameraPositionAndTargetAndUpVec(position, target, cameraUpVec);
	//SetCameraNearFar(nearClip, farClip);
	SetupCamera_Perspective(fov);
}

// カメラを前後移動
void CameraBase::MoveForward(float distance) {
	VECTOR dir = VNorm(VSub(target, position));
	position = VAdd(position, VScale(dir, distance));
}

void CameraBase::TargetMoveForward(float distance)
{
	VECTOR dir = VNorm(VSub(target, position));
	target = VAdd(target, VScale(dir, distance));
}

// カメラを左右移動
void CameraBase::MoveRight(float distance) {
	VECTOR forward = VNorm(VSub(target, position));
	VECTOR right = VNorm(VCross(cameraUpVec, forward));
	position = VAdd(position, VScale(right, distance));
	
}

void CameraBase::TargetMoveRight(float distance)
{
	VECTOR forward = VNorm(VSub(target, position));
	VECTOR right = VNorm(VCross(cameraUpVec, forward));
	target = VAdd(target, VScale(right, distance));
}

// カメラを上下移動
void CameraBase::MoveUp(float distance) {
	position = VAdd(position, VScale(cameraUpVec, distance));
}

void CameraBase::TargetMoveUp(float distance)
{
	target = VAdd(target, VScale(cameraUpVec, distance));
}

// カメラを Y 軸回転（水平回転）
void CameraBase::RotateY(float angle) {
	VECTOR forward = VSub(target, position);
	MATRIX rotMat = MGetRotY(angle);
	forward = VTransform(forward, rotMat);
	//target = VAdd(position, forward);
}

// カメラを X 軸回転（視点の上下）
void CameraBase::RotateX(float angle) {
	VECTOR forward = VSub(target, position);
	VECTOR right = VNorm(VCross(cameraUpVec, forward));
	MATRIX rotMat = MGetRotAxis(right, angle);
	forward = VTransform(forward, rotMat);
	//target = VAdd(position, forward);
}

void CameraBase::ShakeTime()
{
	if (shakeState.time > 0) {
		shakeState.time--;
		Shake();
		if (shakeState.time == 0) {
			ShakeFinish();
		}
	}
	if (shakeState.time == -1) {
		Shake();
	}
}

void CameraBase::Shake()
{
	target.x += GetRand((int)shakeState.power * 2) - (int)shakeState.power;
	target.y += GetRand((int)shakeState.power * 2) - (int)shakeState.power;
	if (shakeTarget) {
		position.x = target.x;
		position.y = target.y;
	}
	

	/*float random = shakeState.power / 5;

	target.x += GetRand(random * 2) - random + (int)shakeState.power;
	target.y += GetRand(random * 2) - random + (int)shakeState.power;
	position.x = target.x;
	position.y = target.y;
	shakeState.power *= -1;
	*/
}

void CameraBase::ShakeReady(ShakeState _shake)
{
	if (shakeState.time > 0 || shakeState.time == 1) {
		return;
	}
	shakeState = _shake;
	shakeTarget = true;
}

void CameraBase::DebugModeChange()
{
#ifdef _DEBUG

	if (CheckHitKey(KEY_INPUT_M)) {
		if (!keyPush) {
			if (debugMode == NORMAL_MODE) {
				debugMode = DEBUG_MODE;
				DebugOpsionSet();
			}
			else {
				debugMode = NORMAL_MODE;
			}
			keyPush = true;
		}
	}
	else {
		keyPush = false;
	}

#endif

}
