#pragma once
#include "../Library/gameObject.h"
#include "LoadData.h"

class CameraBase : public GameObject
{
public:

	enum DebugCamera
	{
		NORMAL_MODE,
		DEBUG_MODE,
	};

	CameraBase(SceneBase* _scene);
	~CameraBase();

	void Update()override;
	void Draw()override;

	void Apply();
	// カメラを前後移動
	void MoveForward(float distance);
	void TargetMoveForward(float distance);
	// カメラを左右移動
	void MoveRight(float distance);
	void TargetMoveRight(float distance);
	// カメラを上下移動
	void MoveUp(float distance);
	void TargetMoveUp(float distance);
	// カメラを Y 軸回転（水平回転）
	void RotateY(float angle);
	// カメラを X 軸回転（視点の上下）
	void RotateX(float angle);

	void ShakeTime();
	void Shake();
	void ShakeReady(ShakeState _shake);
	virtual void ShakeFinish() { ; }

	void DebugModeChange();
	virtual void DebugOpsionSet() { ; }

	VECTOR3 GetTargetPosition()const { return target; }
	VECTOR3 GetPosition()const { return position; }
protected:

	VECTOR target; // 注視点
	VECTOR position; // カメラの位置
	float nearClip;    // ニアクリップ面
	float farClip;     // ファークリップ面
	float fov;         // 視野角（ラジアン）
	float rotation;//カメラの向き(上方向をどっちにするか)
	VECTOR cameraUpVec;//カメラの上方向の指定
	ShakeState shakeState;

	DebugCamera debugMode;
	bool keyPush;
	VECTOR2I debugPosition;
	float rate;
	bool shakeTarget;
};