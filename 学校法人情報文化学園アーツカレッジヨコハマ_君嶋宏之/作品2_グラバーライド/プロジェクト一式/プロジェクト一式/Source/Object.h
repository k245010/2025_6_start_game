#pragma once
#include "../Library/gameObject.h"
#include "commonFind.h"
#include "slowMostionManager.h"
#include "gravityManager.h"
#include "gameManager.h"
#include "loadManager.h"
#include "struct.h"
#include "rgb.h"
//重力方向への速度
static const VECTOR BASE_GRAVITY_VELOCITY = VGet(0, -10, 0);

class CollisionBase;
class SphereCollision;
class OBBCollision;
class EffectManager;
class CommonFind;
class LoadManager;
class Object : public GameObject
{
public:
	Object(SceneBase* _scene);
	virtual ~Object();
	void Update()override;
	void Draw()override;

	VECTOR GetPosition()const { return position; }
	VECTOR GetScale()const { return scale; }
	VECTOR GetSize() const{ return size; }
	VECTOR GetRotation()const { return rotate; }
	virtual MATRIX GetMatrix();
	MATRIX GetAftimageMatrix(VECTOR _position,VECTOR _rotate,VECTOR _scale);
	VECTOR GetVelocity()const { return velocity; }
	//MATRIX GetMatrix(VECTOR _pos);
	float GetGravityRot()const { return gravityRotation; }
	bool GetGActive() const { return gActive; }
	GameManager::GAME_STATE GetGameState() { return gameState; }

	virtual MATRIX GetRotateMatrix();
	MATRIX GetRotateMatrix(VECTOR _rotate);
	HitStopState GetHitStopState() const { return hitStop; }
	bool GetsSlowMosion() { return debugStop; }
	float GetObjectRate() { return objectGravityRate; }
	//MATRIX GetRotateAxisMatrix();

	VECTOR Transform(VECTOR addPos);//velocityを回転をつけて移動させたいときに使う関数
	VECTOR Transform(VECTOR _velocity,VECTOR addPos);//velocityを回転をつけて移動させたいときに使う関数

	bool IsActive() const { return active; }//生きてるかどうかの判定

	void SlowModeChange(bool _slow) { debugSlowMode = _slow; }//デバック用スローモードの開始と終了

	//void SetGravityRotation();
	void SetGravityRotation(GravityState _state, float _worldGravity);
	void GravityAddSpeed(GravityState _state);

	void GravityMove();
	virtual void GravityMove(GravityState _state);

	void Init(int _hModel,VECTOR _position, VECTOR _scale, VECTOR _rotate,int _type,int _collisionId);
	void Init(VECTOR _position, VECTOR _scale, VECTOR _rotate);

	int GetObjectType() const { return type; }

	VECTOR GetGravityVelocity() const { return gravityVelocity; }

	void CollisionForSetPosition(VECTOR set) { position = set; }

	/// <summary>
	/// 当たった時の処理
	/// </summary>
	/// <param name="me">自分の当たり判定のタグ</param>
	/// <param name="target">相手の当たり判定のタグ</param>
	virtual void CollisionEvent(CollisionBase& me, CollisionBase& target);

	bool HitStopStart(HitStopState _hit);
	bool ShakeStart(ShakeState _shake,bool _moveCan);
	void HitStopShake();
	void StopShake();

	void ControlVibrationStart(ControlVibration _vibration);

	int GameStateChange(GameManager::GAME_STATE _state);

	virtual int GroundLanding();//地面についているときに行う処理

	void SetVisionMove(bool _vision) { visionMove = _vision; }
	bool GetVisionMove() { return visionMove; }

	int GetHmodel() const { return hModel; }
	int GetCollisionId()const { return collisionId; }
	void SetActive(bool _active) { active = _active; }

	void RemoveObject();
	void RemoveSlowObject();

	void CollisionForSetVelocity(VECTOR _vel) { velocity = _vel; }
	void CollisionForSetGravityVelocity(VECTOR _vel) { gravityVelocity = _vel; }

protected:

	int hModel;
	VECTOR position;
	VECTOR scale;
	VECTOR rotate;//charaBaseの継承先になるとこれは描画用の角度になる。
	VECTOR size;

	CommonFind* commonFind;
	bool active;

	LoadManager* loadManager;
	SlowMostionManager* slowMostionManager;

	bool debugSlowMode;
	bool debugStop;
	int slowModeCounter;

	VECTOR gravityVelocity;
	float gravityRotation;
	GravityManager* gravityManager;
	bool canGravityDirection[G_DIRECTION_NUM];//8方向それぞれに重力が働いたとき反応するか
	int type;

	VECTOR gravityAdd;
	//重力の影響を受けるか
	bool gActive;


	SphereCollision* sphereCollision;
	OBBCollision* obbCollision;

	float gravityAddRate;//物の重力移動の加速を変える
	float objectGravityRate;//物の重力移動の速さを変える

	Rgb rgb;

	/// <summary>
	/// shakeとhitStopの違いとしては
	/// shakeは重力の影響を受ける
	/// hitStopは重力の影響を受けない
	/// 共通部分はキー操作が効かなくなるところ（しっかりと指定しないといけない）
	/// </summary>
	HitStopState hitStop;
	ShakeState shake;
	VECTOR shakePos;
	ControlVibration vibration;
	
	EffectManager* effectManager;

	GameManager::GAME_STATE gameState;

	VECTOR velocity;

	GravityMoveMax moveMax;

	bool visionMove;//視界内に入っていないと一切動かなくなる

	VECTOR vec;
	float alpha;
	int collisionId;
	bool moveCan;
};