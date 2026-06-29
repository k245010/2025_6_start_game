#include "Camera.h"
#include "../Stage/StageManager/StageManager.h"
#include "../Common/PostEffect/PostEffect.h"
#include "../../ImGui/imgui.h"
#include "../../Library/App.h"
#include <assert.h>

namespace
{
	constexpr float SPEED = 2.5f;
	const VECTOR3 DEBUG_PUT_POS = VECTOR3(500.0f, 3500.0f, -500.0f);
	//const VECTOR3 DEBUG_PUT_POS = VECTOR3(1000.0f, 3500.0f, -2000.0f);
	const VECTOR3 DEBUG_LOOK_POS = VECTOR3(6000.0f, 3500.0f, -7000.0f);
	//const VECTOR3 DEBUG_LOOK_POS = VECTOR3(6000.0f, 5000.0f, -7000.0f);
	const VECTOR3 DEBUG_PUT_TARGET = VECTOR3(0.0f,-3404.0f,0.0f);
	//const VECTOR3 DEBUG_POS = VECTOR3(1000.0f, 14000.0f, -2000.0f);
	//const VECTOR3 DEBUG_POS = VECTOR3(1500.0f, 5500.0f, -1300.0f);
}

Camera::Camera()
{
	// NearとFarの表示距離を設定
	SetCameraNearFar(CAMERA_INFO::NEAR_NUM, CAMERA_INFO::FAR_NUM);
	// 視野角の設定
	SetupCamera_Perspective(CAMERA_INFO::VIEW_ANGLE);

	position	= VGet(250, 1294, -335);
	target		= DEBUG_PUT_TARGET;

	state		= STATE::NORMAL;

	//SetDrawOrder(-100000);
}

Camera::~Camera()
{
}

void Camera::Init()
{
	VECTOR3 debugPos = FindGameObject<StageManager>()->GetStageCenterPosition();
	debugPosition = VGet(debugPos.x, DEBUG_PUT_POS.y, debugPos.z);
}

void Camera::Update()
{

#if !USE_POST_EFFECT
	SettingCamera();
#endif

}

void Camera::Draw()
{
	//DrawFormatString(100, 500, 0xff0000, "%d", state);
	//DrawFormatString(100, 550, 0xff0000, "x %lf y %lf z %lf", target.x,target.y,target.z);
	//DrawSphere3D(VNorm(target - position) * 500.0f + position, 70, 16, 0x00ff00, 0xffffff, false);

	//DrawLine3D(position + VECTOR3(0, 50, 0), target + VECTOR3(0,50,0), 0xff00ff);

#if USE_POST_EFFECT
	// NearとFarの表示距離を設定
	SetCameraNearFar(CAMERA_INFO::NEAR_NUM, CAMERA_INFO::FAR_NUM);
	// 視野角の設定
	SetupCamera_Perspective(CAMERA_INFO::VIEW_ANGLE);

	SetCameraPositionAndTarget_UpVecY(position, target);
#endif
}

void Camera::SettingCamera() const
{
	SetCameraPositionAndTarget_UpVecY(position, target);
}

void Camera::Control()
{
	if (CheckHitKey(KEY_INPUT_W))
		target.z += SPEED;
	if (CheckHitKey(KEY_INPUT_S))
		target.z -= SPEED;
	if (CheckHitKey(KEY_INPUT_A))
		target.x -= SPEED;
	if (CheckHitKey(KEY_INPUT_D))
		target.x += SPEED;
	if (CheckHitKey(KEY_INPUT_SPACE))
		target.y += SPEED;
	if (CheckHitKey(KEY_INPUT_LSHIFT))
		target.y -= SPEED;
}

void Camera::FirstPersonCamera(VECTOR3 _playerPos, VECTOR2 _rot)
{
	position	= _playerPos;

	position.y	= position.y + 10;//カメラの高さが低いから
	target		= position;

	//向いてる左右方向（ベクトル）に注視点をずらす
	target.x += sinf(_rot.y);
	target.z += cosf(_rot.y);

	//向いてる上下方向に注視点を代入する
	target.y -= _rot.x;

	SetCameraPositionAndTarget_UpVecY(position, target);
}

