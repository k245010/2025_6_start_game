#pragma once
#include "charaBase.h"
class EnemyBase : public CharaBase
{
public:
	enum EnemyState
	{
		ENEMY_WAIT,
		ENEMY_MOVE,
		ENEMY_ATTACK,
		ENEMY_HITSTOP,
		ENEMY_DAMAGE,
		ENEMY_DEATH,
	};

	EnemyBase(SceneBase* _scene);
	~EnemyBase();

	void Update()override;
	void Draw()override;

	void EnemyInit(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId);
	virtual void EnemyInit(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId,int _eType);

protected:

	//ダメージステートに移動する時に呼ぶ関数
	void StateInDamage();
	void CollisionEvent(CollisionBase& me, CollisionBase& _target)override;
	//向いている方向を反転する
	virtual void DirectionReverse();
	//ENEMY_HITSTOP時の処理
	void EnemyHitStop();
	//ENEMY_DEATH時の処理
	void EnemyDeath(VECTOR _offset={0,0,0});
	//一定時間移動したら反転して移動する処理
	void RoundTripMove(VECTOR _moveVec);
	//スケールの初期値を変更する
	void MultScaleInit(VECTOR _scale);
	bool doMultScaleInit;//既にMultScaleを呼んだかどうか

	int counter;
	float speed;

	int moveTime;

	const int DEATH_TIMER_MAX_DEFAULT = 20;//死亡アニメーションのフレーム数
	const int DAMAGE_TIMER_MAX_DEFAULT = 30;//ヒットストップの時間数(MOVEかDAMAGEに遷移)
	const int MOVE_CHANGE_TIMER_MAX_DEFAULT = 300;//移動方向を変えるまでの最大時間
	const float SPEED_DEFAULT = 5.0f;//移動速度の通常値
	const float MOVE_CHANGE_DECELERATION_START_RATIO = 0.75f;//最大時間の何割になったら減速を始めるのか
	const float MOVE_DECELERATION_RATIO = 0.05f;//移動反転時の減速の減加速度
	const float MOVE_DECELERATION_BEFORE_SPEED_RATIO = 0.25f;//前フレームの移動速度をどれだけ現在のフレームの移動速度に乗せるか
	const int WAIT_TIMER_MAX_DEFAULT = 20;//待機時間
	const int HIT_STOP_TIME_MAX = 30;
	const float HIT_STOP_POWER = 100.0f;

	int hitStopTimer;
	int hitStopTimerMax;
	int deathTimer;
	int deathTimerMax;

	int moveChangeTimer;
	int moveChangeTimerMax;

	int waitTimer;
	int waitTimerMax;

	float moveDecel;

	SphereCollision* enemyLandSphereCollision;

	EnemyState state;
	int inEnemyType;//エネミーをABCDEで分けた後に更に動きを細分化するもの

	VECTOR beforeDamagePosition;//ダメージを受ける前の座標
	VECTOR beforeDamageRotation;//ダメージを受ける前の角度
};