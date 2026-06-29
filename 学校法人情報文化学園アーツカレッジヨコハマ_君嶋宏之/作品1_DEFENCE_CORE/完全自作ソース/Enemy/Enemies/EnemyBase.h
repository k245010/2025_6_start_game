#pragma once
#include "../../CharaBase/CharaBase.h"
#include "../EnemyKind.h"
#include "../EnemyInfo.h"
#include "../../Common/Function/Function.h"
#include <vector>


class RayCollision;
class Animator;
class Physics;
class EnemyManager;
class Gauge;
/// <summary>
/// 敵の基底クラス
/// </summary>
class EnemyBase : public CharaBase
{
public:

	/// <summary>
	///								コンストラクタ
	/// </summary>
	/// <param name="_enemyManager">EnemyManagerクラスポインタ	</param>
	/// <param name="_kind">		敵の種類					</param>
	EnemyBase(EnemyManager* _enemyManager, const ENEMY_KIND& _kind);
	
	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="_pos">				召喚座標						</param>
	/// <param name="_hModel">			モデル							</param>
	/// <param name="_enemyManager">	EnemyManagerクラスポインタ		</param>
	/// <param name="_kind">			敵の種類						</param>
	EnemyBase(const VECTOR3& _pos,const int& _hModel, EnemyManager* _enemyManager,const ENEMY_KIND& _kind);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="trans"> トランスフォーム </param>
	/// <param name="_hModel"> モデルハンドル </param>
	///EnemyBase(const Transform& trans, const int& _hModel);
	
	virtual ~EnemyBase();

	/// <summary>
	/// enemyManagerのクラスポインタの初期化
	/// </summary>
	/// <param name="_enemyManager"> enemyManagerのクラスポインタ </param>
	void Init(EnemyManager* _enemyManager = nullptr);

	void Update()	override;
	void Draw()		override;

	/// <summary>
	///								敵データを設定する
	/// </summary>
	/// <param name="_enemyData">	敵データ		</param>
	void SetEnemyData(const EnemyInfo::EnemyData& _enemyData);

	/// <summary>
	///				敵のデータを返す
	/// </summary>
	/// <returns>	敵のデータ	</returns>
	const EnemyInfo::EnemyData& GetEnemyData() const { return enemyData; }

	/// <summary>
	///				敵の種類を返す
	/// </summary>
	/// <returns>	敵の種類			</returns>
	const ENEMY_KIND& GetEnemyKind() const { return enemyKind; }

	/// <summary>
	/// 進むべきWayPointのナンバーを管理するコンテナを返す
	/// </summary>
	/// <returns> 進むべきWayPointのナンバーを管理するコンテナ </returns>
	std::vector<int> GetNavPointNumber() const { return navPointNumbers; }
	
	/// <summary>
	/// 進むべきWayPointのナンバーを管理するコンテナをクリア
	/// </summary>
	void DeleteNavPointNumber();

	/// <summary>
	/// 座標を返す
	/// </summary>
	/// <returns> 座標 </returns>
	const VECTOR3& GetPosition() const { return transform.position; }

	/// <summary>
	/// ヴェロシティを返す
	/// </summary>
	/// <returns> ヴェロシティ </returns>
	const VECTOR3& GetVelocity() const { return velocity; }
	
	/// <summary>
	/// 敵の状態
	/// </summary>
	enum class ENEMY_STATE
	{
		STAY = 0,	// 待機
		SEARCH,		// 経路を確保
		MOVE,		// 確保した経路を進む
		ATTACK,		// 攻撃
	};

	/// <summary>
	/// 敵の移動の状態
	/// </summary>
	enum class ENEMY_MOVE_STATE
	{
		SET = 0,	// 次に進む座標をセット
		MOVE,		// セットした座標に進む
	};

	/// <summary>
	/// 敵の状態を設定
	/// </summary>
	/// <param name="_state"> 敵の状態 </param>
	void SetState(const ENEMY_STATE& _state) { state = _state; }

	/// <summary>
	/// 敵の移動の状態を設定
	/// </summary>
	/// <param name="_state"> 移動の状態 </param>
	void SetMoveState(const ENEMY_MOVE_STATE& _state) { moveState = _state; }

	/// <summary>
	/// 経路探索情報のリセット
	/// </summary>
	void ResetSearchInfo() 
	{ 
		moveState		= ENEMY_MOVE_STATE::SET;
		navPointIndex	= 0;
		DeleteNavPointNumber();
	}

	/// <summary>
	/// 経路探索を終了(変数リセット)する
	/// </summary>
	void NavgationEnd();

	/// <summary>
	/// 常に経路探索するかのフラグの設定
	/// </summary>
	/// <param name="set"> 設定値 </param>
	void SetNavgationAlways(bool set) { navAlways = set; }

	/// <summary>
	/// 経路探索してたどり着く相手を返す
	/// </summary>
	/// <returns> 経路探索してたどり着く相手 </returns>
	const EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND& GetNavTargetObjKind() const { return enemyData.navigationData.navigationTargetKind; }

	/// <summary>
	/// 経路探索してたどり着く相手を設定
	/// </summary>
	void SetNavTargetObj(const EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND& _navTr) { enemyData.navigationData.navigationTargetKind = _navTr; }

	/// <summary>
	///							移動ルートを描画するか設定
	/// </summary>
	/// <param name="_isDraw">	true:描画する / false:描画しない	</param>
	void SetIsDrawRoot(bool _isDraw) { isDrawRoot = _isDraw; }

protected:

	/// <summary>
	/// キャラクターが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	bool HitChara(const CollisionHitInfoData& _targetKind) override;

	/// <summary>
	/// UI描画
	/// </summary>
	void UIDraw();

	/// <summary>
	/// 移動する処理
	/// </summary>
	virtual void Move();

	/// <summary>
	/// 敵の移動の回転処理をする
	/// </summary>
	void MoveRotate();

	/// <summary>
	/// 攻撃する処理
	/// </summary>
	void Attack();

	/// <summary>
	/// 攻撃ステートの移行をリクエストする
	/// </summary>
	void RequestChangeStateAttack();

	/// <summary>
	/// ゴール目標の座標ポインタを返す ToDo 敵制御のコントローラーが担うか検討
	/// </summary>
	/// <returns> ゴール目標の座標ポインタ </returns>
	const VECTOR3* GetGoalPositionPtr();

	EnemyManager* enemyManager				= nullptr;	// EnemyManagerのクラスポインタ
	Animator* animation						= nullptr;	// アニメーションクラスのクラスポインタ
	Physics* physics						= nullptr;	// 物理クラスのクラスポインタ
	Gauge* enemyHPGauge						= nullptr;	// 敵のHPゲージ
	//SphereCollision* stackBreakSphereColl	= nullptr;	// 自身がブロックに埋もれないように、ブロックを壊しスタックを解消する球の当たり判定クラスポインタ　攻撃の球の当たり判定とは別
	RayCollision* attackRayCollision		= nullptr;	// 

	EnemyInfo::EnemyData enemyData;			// 敵のデータ

	Transform bodyTransform;				// 体の当たり判定のトランスフォーム
	//Transform stackBreakTransform;			// スタックを解消する当たり判定のトランスフォーム
	Transform attackTransform;				// 攻撃当たり判定のトランスフォーム
	float attackIntervalCount	= 0.0f;		// 攻撃のインターバルカウント 

	ENEMY_STATE state			= ENEMY_STATE::SEARCH;		// 敵の状態
	ENEMY_STATE lastState		= ENEMY_STATE::SEARCH;		// １フレーム前の敵の状態
	ENEMY_KIND enemyKind		= ENEMY_KIND::MAX;			// 敵の種類
	ENEMY_MOVE_STATE moveState	= ENEMY_MOVE_STATE::SET;	// 敵の移動の状態

	std::vector<int> navPointNumbers;		// 進むべきナビポイントナンバーを管理するコンテナ
	size_t navPointIndex		= 0;		// 進むべきナビポイントナンバーを管理するコンテナのインデックス
	int copyNavPointNumber		= -1;		// 前回search結果のnumberのコピー
	VECTOR3 targetPosition		= VZero;	// 次に進む座標
	VECTOR3 goalPosition		= VZero;	// ゴールの座標
	int goalNavPointNumber		= -1;		// ゴールのナビポイントナンバー

	float actionRate			= 1.0f;		// 行動速度率
	float slowDownEffectTime	= 0.0f;		// 行動速度低下エフェクトの再生経過時間
	float moveSpeed				= 0.0f;		// 敵の移動速度

	int uiFunctionIDNumber			= -1;	// UIDraw関数に対するIDナンバー
	float baseMoveSpeedToAniRate	= 0.0f;	// 歩行する速度に対しての歩きのアニメーションレート

	float invincibleCount		= 0.0f;		// 無敵カウント
	bool isInvincible			= false;	// 無敵フラグ

	//_ デバッグ

	bool navAlways = false;					// 常に探索するかどうか
	
	bool isDrawRoot = false;						// ルートの描画フラグ
	int drawRootingCount = 0;					// ルートを描画しているカウント
	int drawRootAlpha = 0;						// ルート描画のalpha値
};