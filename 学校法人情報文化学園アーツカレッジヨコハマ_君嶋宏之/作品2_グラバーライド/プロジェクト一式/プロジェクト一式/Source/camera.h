#pragma once
#include "cameraBase.h"
#include "gameManager.h"
#include "player.h"

//class Player;
class GravityManager;

static VECTOR BASE_CAMERA_UP_VEC = { 0,1,0 };//初期状態でのカメラの上方向のベクトル
static int CAMERA_ROT_TIME = 20;//何フレームでカメラを今の向きから変更した向きに移動するか
static const float CAMERA_Z = -5610;
static const float RIGHT_LEFT_POS = 580.0f;
static const float RIGHT_ADD = 150.0f;
static const float LEFT_LEFT_POS = 1190.0f;
static const float LEFT_ADD = 160.0f;

class Camera : public CameraBase
{
public:

	struct CameraMoveStartPosition
	{
		float rightMax;
		float leftMax;
		float upMax;
		float downMax;
	};

	enum CameraState
	{
		NORMAL_CAMERA,
		GRAVITY_MOVE_CAMERA,
		HIT_CAMERA
	};

	enum CameraMoveModeSide
	{
		RIGHT_CAMERA,
		LEFT_CAMERA,
	};

	struct CameraPositionMode
	{
		CameraMoveModeSide side;
	};

	Camera(SceneBase* _scene);
	~Camera();
	void Draw()override;
	void Update()override;


	float GetCameraRot()const { return rotation; }

	int CameraStateChange(CameraState _state);

	void PlayerPositionChange();

	void PlayingBeforeCamera();

	void ShakeFinish()override;
	void DebugOpsionSet()override;
	void GravityModeCamera();

	void CameraYMove();
	void PositionSet();

	void NormalCameraChange();

	void ShakeCamera();
	bool CameraPlayerCollision(float _pos1,float _pos2,float _pos3);

	void CameraNoMoveRange();
	/*void ScreenSideMaxPosSet(float _leftPos, float _rightPos);
	void ScreenSideMaxPosSet(float _leftPos, float _rightPos,float add);
	void ScreenVerticalMaxPosSet(float _upPos, float _downPos);
	void ScreenVerticalMaxPosSet(float _upPos, float _downPos, float add);*/

	MATRIX GetCameraMatrix();
	MATRIX GetCameraPlayerMatrix();

	void CollsionMoveEaseIn();

	void AttackCameraStart();
	void AttackCamera();

	void PlayBeforeCamera();

	void GoalAnimCamera();
	void GoalAfterCamera();

	void CameraTarget();

private:

	//プレイヤーのGravityRotationを基準にカメラの向きを変更する
	void SetCameraRot();

	//VECTOR upVector;   // 上方向ベクトル

	Player* player;


	VECTOR playerPosition;


	VECTOR targetPosition; // 移動先のカメラ位置
	VECTOR targetTarget;   // 移動先の注視点
	float lerpSpeed;       // 補間速度（0.0 ~ 1.0）

	CameraState cameraState;

	CameraState keepState;
	GravityManager* gravityManager;

	GameManager::GAME_STATE gameState;
	int beforeCounter;

	int stageNum;
	VECTOR moveCameraAddPosition;
	VECTOR moveCameraSetPosition;
	float smoothCounter;
	CharaBase::Direction dire;
	//bool oneFrame;
	VECTOR keepPosition;
	float worldGravity;
	VECTOR allowPos;
	GravityState playerChangeGravityKeep;
	VECTOR smoothStartPosition;
	int debugCounter;
	float gravityKeep;
	VECTOR keepVector;
	float cameraAtan;
	bool gravityNo;
	//CameraMoveStartPosition startPosition;
	CameraPositionMode cameraMode;
	bool rockSide;
	bool rockVertical;
	bool cameraDirectionChange;
	VECTOR rockPosition;
	int goalTime;

	float beforeRotation;
	VECTOR collsionPlusPosition;
	bool coll;
	VECTOR baseMoveMaxPosition;
	float collsionMoveStepCounter;
	bool reverce;
	VECTOR collBeforePositionKeep;
	bool firstColl;
	VECTOR playerTargetDist;
	VECTOR baseDist;
	float changeCounter;
	VECTOR nowPosition;
	float hitCounter;
	float plus;
	VECTOR posKeep;
	bool change;
	bool sub;
	float stopCounter;
	bool playerCameraAreaHit;

	bool cameraAreaBefore;
	bool changeMode;
};