#pragma once
#include "noGravityEnemyBase.h"
#include "anime.h"

enum MoveType
{
	NO_TYPE = -1,
	NO_MOVE,
	HORIZONTAL,
	VERTICAL,
};

namespace
{
	const float FLY_ENEMY_SIZE = 400.0f;//エネミーの当たり判定の大きさ
	const float FLY_ENEMY_HORIZONTAL_LEAN = 10.0f;//水平方向に移動する浮遊敵で進行方向に機体どれだけ傾けるか(度数法で入力)
	//上下方向(X軸)への傾きは分かりづらいので水平より大きめに設定した方が良い
	const float FLY_ENEMY_VERTICAL_LEAN = 20.0f;//垂直方向に移動する浮遊敵で進行方向に機体どれだけ傾けるか(度数法で入力)

	//水平方向の敵は180°回転するので長めに取った方が良い
	const float HORIZONTAL_TURN_TIME_MAX = 3.0f;//水平方向に移動する浮遊敵の回転時間(秒)
	const float VIRTICAL_TURN_TIME_MAX = 1.0f;//垂直方向に移動する浮遊敵の回転時間(秒)

	const float FLY_ENEMY_DAMAGE_TIME_MAX = 1.5f;//ダメージを受けた時の演出時間
	const float FLY_ENEMY_DAMAGE_TURN_NUM = 2.0f;//ダメージを受けた時の演出で何回転させるか
	const VECTOR FLY_ENEMY_DAMAGE_MOVE_MAX = { 0,-2500,3000 };//ダメージを受けた時にどれだけ座標を移動させるか
	const VECTOR FLY_ENEMY_DEATH_EFFECT_OFFSET = { 0,0,-500 };//デス時のエフェクトをプレイヤーの座標から見ていくつの場所に表示するか
	const VECTOR FLY_ENEMY_DAMAGE_MOVE_RAND_MAX = { 1.3f,1.3f,1.3f };//浮遊敵のダメージ吹っ飛び座標の乱数の最大幅
	const VECTOR FLY_ENEMY_DAMAGE_MOVE_RAND_MIN = { 0.7f,0.7f,0.7f };//浮遊敵のダメージ吹っ飛び座標の乱数の最小幅
	const float FLY_ENEMY_DEATH_EFFECT_CREATE_RATE = 0.8f;//ダメージ演出の何割まで進んだら死亡エフェクトを発生させるか
}

class FlyEnemy :public NoGravityEnemyBase
{
public:
	FlyEnemy(SceneBase* _scene);
	virtual ~FlyEnemy();
	void Update()override;
	void Draw()override;

	void CollisionEvent(CollisionBase& me, CollisionBase& _target)override;

	enum VirticalMoveDirection
	{
		UP,
		DOWN,
	};

private:

	void AnimeInit();
	void DirectionReverse()override;

	MoveType moveType;
	VirticalMoveDirection virDir;//縦の移動方向
	VECTOR defaultRotation;//初期状態(生成時)の回転角度
	VECTOR drawRotation;//描画用の回転

	float turnTimer;//移動方向変更時の回転用のタイマー
	float damageTimer;//ダメージを受けたときの演出用タイマー
	float rotateRand;//ダメージを受けた時の回転の乱数幅
	VECTOR damageVecRand;//ダメージを受けた時の動きの乱数幅
	VECTOR damageMoveTargetPos;//ダメージを受けた時の動きの目標座標

	VECTOR totalDamageMoveVec;//ダメージを受けた時に動いた合計量
	bool deathEffectFlg;//デス時のエフェクトをCreateしたかどうか

	enemyFlyAnime* anime;
};