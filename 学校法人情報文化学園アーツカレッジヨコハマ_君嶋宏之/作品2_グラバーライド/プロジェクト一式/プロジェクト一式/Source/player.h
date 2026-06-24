#pragma once
#include "charaBase.h"
#

static const int PLAYER_HP_MAX = 3;

static const int MAP_LEFT_WIDTH_POS[5] = { -4000,-4000,-10000,-10000,-10000};
static const int MAP_RIGHT_WIDTH_POS[5] = { 10000,30000,30000,10000,10000 };
static const int MAP_UP_WIDTH_POS[5] = { 10000,1000,10000,10000,10000 };
static const int MAP_DOWN_WIDTH_POS[5] = { 0,-50000,-30000,-10000,-10000 };
static const int ATTACK_AFTIMAGE_NUM = 30;
static const int GRAVITY_MOVE_AFTIMAGE_NUM = 5;
static const float ALPHA_AFTIMAGE_GRAVITY_MOVE = 0.05f;
static const float ALPHA_AFTIMAGE_ATTACK = 0.3f;
static const float DEG_Z_MOVE = 2.0f;
static const int ALOOW_NUM = 20;

class PlayerUi;
class SphereCollision;
class RayCollision;
class Camera;
class StageObjectManager;
class SoundManager;
class BackGroundAloow;
class StageScore;
class Shadow;

class Player : public CharaBase
{
public:

	enum PlayerMode
	{
		NORMAL_MODE,
		GRAVITY_CHANGE_MODE,
		GRAVITY_MOVE_MODE,
		RAIL_MOVE,
	};

	enum PlayerAttackState
	{
		NO_ATTACK,
		ATTACK1,
		HIT,
	};

	enum LoseState
	{
		No_Lose,
		Die,
		Lose,
		Respown,
	};

	enum ChangeGravityState
	{
		NO_CHANGE,
		SAME_GRAVITY,
		CHANGE,
	};

	enum GoalState
	{
		PlayingAfterMove,
		Product,
	};

	Player(SceneBase* _scene);
	~Player();
	void Update()override;
	void Draw()override;
	void Move();//移動
	void Jump();//ジャンプ
	void GravityChange();//プレイヤーのモードを変更している

	void GravityMode();//重力モードの時の方向選択箇所
	void GravityMove(float _guageNum);//重力の移動
	void GravityMoveRotateChange();//重力の移動の時の角度の変更を徐々に向ける
	void GravityMove(GravityState _state)override;

	void NormalModeStateChange();//ノーマルモードに戻るときに必要な処理（重力移動モードからノーマルモードへ戻るなど）
	void GracityChangeModeChange();
	void GracityMoveModeChange();
	void GravityMoveFinish();

	void AttackReady();//攻撃の始まりのKEY指定の箇所
	void Attack();//アタックしてるときに通る処理

	/*void AttackHit();
	void AttackHitMove();
	void AttackHitMoveReset();*/

	int GroundLanding(bool _can);//地面についているときに行う処理

	PlayerMode GetPlayerMode() { return mode; }
	void GuageAmontReset();//ゲージのヘリの加速値のリセット

	void DebugPlayer();//デバック機能0,1,2,3,4ボタン使用

	void CollisionEvent(CollisionBase& me, CollisionBase& target) override;

	bool Goal() const;//ゴールをしているかどうかの判定

	bool NoGround();

	void PlayerWorldChange(CollisionBase& target,bool _move);

	void SpickDamageMove();
	void SpickMoveReset();
	void AmongPointReset();
	bool SetAmongPosition(VECTOR _vector,float _angle) {
		if (amongPointPosition.x == _vector.x && amongPointPosition.y == _vector.y && amongPointPosition.z == _vector.z) {
			return false;
		}
		amongPointPosition = _vector;
		amongWorldGravity = gravityManager->AngleToGravityStateGet(_angle,false);
		if (amongWorldGravity == NO_GRAVITY) {
			amongWorldGravity = gravityManager->GetWorldGravityState();
		}
		return true;
	}

	void DamageMoveReady(VECTOR _velo,int _time);
	void DamageMove();
	void DamageMoveReset();

	PlayerAttackState GetPlayerAttackState() const { return attackState; }
	bool GetGroundRand()const { return groundRand; }

	void PlayerInit(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type,int _collisionId);
	

	void LoseMoveStart(GameManager::LoseSceneSelect _kind,const int _time);
	void ReSpown();
	void Dead();
	void DieStateChange();

	void AnimeInit()override;
	LoseState GetLoseState() { return loseState; }

	bool GetJumpIng() { return jump; }
	VECTOR GetJumpKeepPosition() { return jumpKeepPosition; }
	/*VECTOR GetCameraAloowPos() {
		allowPos = centerPosition;
		float DrawAngle = allowAngle - DX_PI_F / 2;
		allowPos.x += cosf(DrawAngle) * 1500.0f;
		allowPos.y += sinf(DrawAngle) * 1500.0f;
		return allowPos;
	}*/

	GravityState GetStartGravityMode() { return startGravityMode; }
	GravityState GetBeforeGravityMode() { return beforeGravityMode; }

	void AnimePlayChange(PlayerAnimesionKind _kind);
	ChangeGravityState GetChangeGravity() { return changeGravity; }

	void SetAftimageNum();
	void ResetAftimage();

	void RightNoPlayingMove();
	void LeftNoPlayingMove();
	
	void ZMove();
	void NoPlayingZModeInit();
	GoalState GetGoalState() { return goalState; }

	void BumperHitInit(VECTOR _bumperPos);
	void BumperHitMove();
	

	int GameStateChange(GameManager::GAME_STATE _state);
	void HealHp();
	void HealGravityGuage();
	//void PlayEffect4();
	void RailMoveReady(CollisionBase& target);
	void RailMove();

	void CompulsionGravityInit();
	void GoalStart();
	void GoalProductStart();
	void GuageHeal();
	void GaugeHealInit(float _time,float _waitTime);
	void GaugeHealInit(float _time, float _waitTime,float _num);
	void GuageHealReset();
	bool GoalProductFinish() { return goalProductCounter <= 0; }

	void PlayAfterStart();
	void HpZero();

	void TargetCameraRay(const VECTOR _t) { cameraTarget = _t; }
	VECTOR GetRayTarget() { return position; }
	VECTOR GetCameraCollPos() { return cameraMovePositionKeep; }

	bool GetCameraHit() { return cameraHit; }
	void CameraHitRe(){ cameraHit = false;}
	void WalkReset();
	float GetHitCounter() { return enemyHitAddCounter; }
	bool GetDamageCamera() { return damageCamera; }
	void DamageCamera() { damageCamera = false; }
	bool GetEnemyHit() { return enemyHit; }
	void EnemyHitReset() { enemyHit = false; }

	void InteriaStart(VECTOR _amont, float _max);
	void InteriaReset();
	void InteriaGravity();
	bool GetPlayStart() { return playStart; }
	void CameraSe();
	void DieProduct();

private:

	float jumpSpeed;
	int jumpNum;
	//bool keyPush;

	//bool gravityChangeKeyPush;

	PlayerMode mode;
	PlayerAttackState attackState;
	
	PlayerUi* playerUi;
	float gravityAfterRotate;
	int gravityAfterChangeRotateCounter;

	float rotateZAdd;

	GravityState gravityMode;
	InputManager* inputManager;
	//bool attackKeyPush;
	int attackTime;

	SphereCollision* playerLandSphereCollision;
	SphereCollision* playerHeadSphereCollision;
	RayCollision* playerRayCollision;

	float guadeDown;
	float attackGuadeDown;
	float guageDownBase;

	bool goal;
	float gravityInfluence;
	Camera* camera;
	//bool spickMove;
	VECTOR velocityKeep;
	float spickMoveAngle;

	int attackHitTime;
	float attackAngle;
	float attackDistanceZ;
	float attackDistanceY;
	bool jumpDown;

	VECTOR amongPointPosition;
	VECTOR damageVelocity;
	int damageMoveTime;

	StageObjectManager* stageObjectManager;
	GravityState amongWorldGravity;
	bool groundRand;
	VECTOR startPosition;
	float loseMoveTime;
	VECTOR loseAddPosition;

	int sizeCounter;

	SoundManager* sound;
	int soundCounter;
	bool walk;
	int loseTime;
	VECTOR effectNormalSize;
	VECTOR effectNormalRotate;

	playerAnime* anime;
	int productCounter;
	LoseState loseState;
	VECTOR2F effect2DSize;
	VECTOR2F effect2DRotate;
	VECTOR jumpKeepPosition;
	GravityState startGravityMode;
	GravityState beforeGravityMode;
	playerAnime::Animesion previewAnimeInformation;
	playerAnime::Animesion beforeAnimeInformation;

	float blendTime;
	float walkAnimeSpeedCounter;
	ChangeGravityState changeGravity;
	VECTOR aftimagePosition[ATTACK_AFTIMAGE_NUM];
	VECTOR aftimageRotate[ATTACK_AFTIMAGE_NUM];
	int aftimageNum;
	float aftimageAlpha;
	VECTOR zMoveGravityRot;

	//void (*Player::movePointer)(void);
	typedef void (Player::* Pointer)();
	Pointer movePointer;
	float degZMove;
	int goalProductCounter;
	GoalState goalState;

	int bumperTime;

	VECTOR bumperAdd;
	float bumperStep;
	float bumperStepAdd;
	float bumperNoCollsionCounter;
	
	float loseAngle;
	VECTOR loseFinalPosition;
	float loseMoveAdd;
	VECTOR loseCenter;

	VECTOR railMoveAdd;
	bool rail;
	VECTOR railRotateKeep;
	float changeRailTime;
	bool railNo;
	VECTOR railPos;
	float railCounter;
	bool compulsionGravity;
	bool beforeCompulsionGravity;
	float guageHealNum;
	float guageHealCounter;
	float guageWaitCounter;
	int startEffectCounter;
	bool noGravityArea;
	GravityState aloowKeepState;
	int effectNum;
	VECTOR spickRotate;
	Direction keepDire;

	StageScore* stageScore;
	float walkSpeedAdd;

	int noLandEffectTime;
	VECTOR cameraTarget;

	bool rayHit;
	VECTOR cameraMovePositionKeep;
	bool cameraHit;
	VECTOR sendPos;
	float againGravityCounter;
	VECTOR slide;

	float rateKeep;
	float slideCounter;
	float slideCounterMax;
	bool stopJump;
	bool groundJump;
	VECTOR shadowPosition;
	VECTOR lastGroundPosition;
	VECTOR gravitySlide;
	float gravitySlideCounter;
	float gravitySlideCoutnerMax;
	float gravitySliderEaseInRate;
	bool healG;
	float enemyHitAddCounter;
	float gravityMaxSpeed;
	float enemyHitCounterPlus;
	bool damageCamera;

	VECTOR rayP;
	Shadow* shadow;
	bool enemyHit;
	VECTOR goalPosition;
	VECTOR goalNowPos;
	float goalTime;
	float guageDownNumPlus;
	VECTOR sliderKeep;
	bool playStart;
	float dieCounter;
	VECTOR damageVelocityKeep;
	float gravityNoChangeModeCounter;

	int controlAButtonImage[RAIL_MOVE];
	int controlBButtonImage[RAIL_MOVE];
	VECTOR2F aButtonPosition[RAIL_MOVE];
	VECTOR2F bButtonPosition[RAIL_MOVE];
	int rTriggerImage;
	int index;
	int animeNum;

	int aButtonImage;
	int bButtonImage;
	int rtImage;
	bool gravityLand;

	bool uiDraw;
};