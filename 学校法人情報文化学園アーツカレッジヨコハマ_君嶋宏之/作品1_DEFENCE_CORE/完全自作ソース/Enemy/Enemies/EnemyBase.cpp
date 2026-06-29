#include "EnemyBase.h"
#include "../EnemyManager/EnemyManager.h"
#include "../EnemyInfo.h"
#include "../../Enemy/EnemyDataSerializer/EnemyDataSerializer.h"
#include "../../Navigation/NavigationManager/NavigationManager.h"
#include "../../Collision/Collisions/SphereCollision.h"
#include "../../Collision/Collisions/RayCollision.h"
#include "../../Player/Player.h"
#include "../../Physics/Physics.h"
#include "../../Game/GameController.h"
#include "../../Gauge/Gauge.h"
#include "../../Common/Animator/Animator.h"
#include "../../Common/Sound/SoundID.h"
#include "../../Common/UI/UIDrawManager/UIDrawManager.h"
#include "../../Common/Effekseer/EffekseerObjectManager.h"
#include "../../Screen.h"
#include "../../../ImGui/imgui.h"

namespace
{
	const float MOVE_ROTATE_SPEED				= 5.0f * DegToRad;	// 移動回転のスピード (度)
	constexpr float NEXT_POINT_DISTANCE_MIN		= 10.0f;			// この距離未満になった場合、次のナビポイントナンバーへ進む
	const VECTOR2 HP_UI_SIZE					= VECTOR2(50, 10);	// HPゲージのサイズ
	constexpr float MAX_INVINCIBLE_TIME			= 1.0f;				// 無敵の最大時間	
	constexpr float MAX_TIME_SLOW_DOWN_EFFECT	= 1.0f;				// 行動速度低下のエフェクトの再生最大時間
	constexpr float DEFAULT_ACTION_RATE			= 1.0f;				// デフォルトの行動速度倍率
	constexpr float HIT_SLIME_ACTION_RATE		= 0.7f;				// スライムパッチにふれたときの行動速度倍率　デフォルトよりも遅くなる
	constexpr float STACK_BREAK_COLLISION_SIZE	= 10.0f;			// スタック解消の当たり判定の直径	
	constexpr float ATTACK_COLLISION_SIZE		= 10.0f;			// 攻撃の当たり判定の直径	
}

EnemyBase::EnemyBase(EnemyManager* _enemyManager, const ENEMY_KIND& _kind) : CharaBase()
{
	assert(_enemyManager != nullptr);
	enemyManager		= _enemyManager;
	enemyKind			= _kind;

	//stackBreakTransform.size	= VOne * STACK_BREAK_COLLISION_SIZE;
	attackTransform.size		= VOne * ATTACK_COLLISION_SIZE;

	sphereColl					= new SphereCollision(&bodyTransform, COLLISION_OBJECT_KIND::ENEMY, "ENEMY_SPHERE", [this](const CollisionHitInfoData& tr) {return HitChara(tr);});
	//stackBreakSphereColl		= new SphereCollision(&stackBreakTransform, COLLISION_OBJECT_KIND::ERASER, "ENEMY_BREAK_BLOCK", nullptr);
	//stackBreakSphereColl->SetTargetTag(COLLISION_OBJECT_KIND::WALL_BLOCK);
	attackRayCollision			= new RayCollision(&attackTransform, COLLISION_OBJECT_KIND::ENEMY_ATTACK, "ENEMY_ATTACK_RAY", nullptr, true);
	// 初期は攻撃状態でないので、非アクティブにする
	attackRayCollision->SetActive(false);

	// 再生サウンド設定
	damageSoundID		= Sound_ID::ENEMY_DAMAGE_SE;
	deathSoundID		= Sound_ID::ENEMY_DEATH_SE;

	// UIDraw関数を登録
	uiFunctionIDNumber	= UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this] { UIDraw(); });

	// 自身をマネージャーに登録
	enemyManager->Push(this);
}

EnemyBase::EnemyBase(const VECTOR3& _pos, const int& _hModel,EnemyManager* _enemyManager, const ENEMY_KIND& _kind) : EnemyBase(_enemyManager,_kind)
{
	assert(_enemyManager != nullptr);
	enemyManager		= _enemyManager;
	transform.position	= _pos;

	// 指定のモデルと同じモデル基本データを使用してモデルを作成し、代入する
	hModel				= MV1DuplicateModel(_hModel);

	// シャドウマップを使用する
	SetUseShadowMapFlag(true);
}

void EnemyBase::Init(EnemyManager* _enemyManager)
{
	enemyManager = _enemyManager;

	assert(enemyManager != nullptr);

	// 敵の出現エフェクト再生
	EffekseerObjectManager::SetEffect(transform.position, EffekseerObjectManager::EF_ENEMY_SPAWN,0.25f);
}

EnemyBase::~EnemyBase()
{
	navPointNumbers.clear();

	if (animation != nullptr)
	{
		delete animation;
		animation = nullptr;
	}

	if (attackRayCollision != nullptr)
	{
		delete attackRayCollision;
		attackRayCollision = nullptr;
	}

	/*if (stackBreakSphereColl != nullptr)
	{
		delete stackBreakSphereColl;
		stackBreakSphereColl = nullptr;
	}*/

	if (enemyHPGauge != nullptr)
	{
		delete enemyHPGauge;
		enemyHPGauge = nullptr;
	}

	MV1DeleteModel(hModel);

	// UIDraw関数を解放
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(uiFunctionIDNumber);

	// 敵データのリリース
	enemyData.ReleaseContainer();

	// EnemyMnaagerのインスタンスが存在するとき
	if (EnemyManager::IsActiveInstance())
	{
		physics->DestroyMe();
		// 自身をマネージャーから解放
		enemyManager->Pop(this);
	}
}

void EnemyBase::SetEnemyData(const EnemyInfo::EnemyData& _enemyData)
{
	enemyData		= _enemyData;
	hp				= _enemyData.hpMax;
	moveSpeed		= _enemyData.moveData.walkSpeed;

	transform.size	= _enemyData.modelSizeData.size;
	transform.scale = _enemyData.modelSizeData.scale;

	for (const auto& bodyColl : _enemyData.collisionData.bodyCollTargetKindList)
	{
		//						当たり判定をする相手	与えるダメージ量
		sphereColl->SetTargetTag(bodyColl.first, bodyColl.second);
	}

	for (const auto& attackTarget : enemyData.collisionData.attackCollTargetKindList)
	{
		//						当たり判定をする相手	与えるダメージ量
		//attackSphereColl->SetTargetTag(attackTarget.first, attackTarget.second);
		
		//						        当たり判定をする相手  与えるダメージ量
		attackRayCollision->SetTargetTag(attackTarget.first, attackTarget.second);
	}

	if (physics != nullptr)
	{
		// Velocityの最大サイズを設定
		physics->SetMaxDistance(moveSpeed);
	}

	if (enemyHPGauge == nullptr)
	{
		// HPゲージを生成
		enemyHPGauge = new Gauge(VECTOR2(0.0f, 0.0f), HP_UI_SIZE.x, HP_UI_SIZE.y, 0.0f, hp);
	}
	else
	{
		// HPゲージの最大値を設定
		enemyHPGauge->SetMaxNum(hp);
	}
	// HP裏側のゲージのアルファ値を設定  0~255の範囲
	enemyHPGauge->SetDrawBackgroundColorAlpha(150);

	// アニメーションの再生速度を設定
	animation->SetPlaySpeed((int)ENEMY_STATE::MOVE, baseMoveSpeedToAniRate * enemyData.moveData.walkSpeed);
}

void EnemyBase::Update()
{
	// 非アクティブだったら
	if (!active)	
	{
		// 自身を削除
		DestroyMe();
		return;	// 自身の削除を行たので、以下処理は行わない
	}

	CharaBase::Update();
	animation->Update();

	// 行動速度低下のエフェクト再生時間が最大以上だったら
	if ((slowDownEffectTime += Time::GameDeltaTime()) >= MAX_TIME_SLOW_DOWN_EFFECT)
	{
		// エフェクト再生時間のリセット
		slowDownEffectTime = 0.0f;
	}

	//_ 体の当たり判定のトランスフォームを設定 _//

	bodyTransform				= transform;
	// 中心座標がモデルの足元なので、体のY軸のサイズの半分、座標を上に上げる
	bodyTransform.position.y	+= transform.GetLenY() / 2;

	//_ スタックを解消する判定のトランスフォームを設定 _//

	//stackBreakTransform.position	= transform.position;
	// 中心座標がモデルの足元なので、体のY軸のサイズの半分、座標を上に上げる
	//stackBreakTransform.position.y	+= transform.GetLenY() / 2;

	// 現在のフレーム処理前の状態を記録
	ENEMY_STATE currentState = state;

	// 無敵時間が0以下のとき
	if ((invincibleCount -= Time::GameDeltaTime()) <= 0.0f)
	{
		invincibleCount = 0.0f;
		isInvincible	= false;
	}

	bool doAnimetion			= true;							// アニメーションの再生をしているかどうか
	float currentFrame			= animation->GetCurrentFrame();	// アニメーションの現在のフレーム
	float attackMaxFrame		= animation->GetMaxFrame() / 3;	// 攻撃アニメーションの最大フレーム数
	const VECTOR3* targetPos	= nullptr;						// 進行する目的地の座標ポインタ
	
	switch (state)
	{
	case ENEMY_STATE::STAY:
		
		moveState = ENEMY_MOVE_STATE::SET;
		targetPos = GetGoalPositionPtr();

		if (targetPos == nullptr)
			break;	// 目的座標が存在しなかったら

		goalPosition = *targetPos;

		animation->Play((int)state);

		state = ENEMY_STATE::SEARCH;
		break;
	case ENEMY_STATE::SEARCH:

		DeleteNavPointNumber();
		targetPos		= GetGoalPositionPtr();

		if (targetPos == nullptr)
			break;	// 目的座標が存在しなかったら

		goalPosition	= *targetPos;

		// 経路探索をして結果のルートのナビポイントナンバーを代入
		navPointNumbers = std::move
		(
			enemyManager->GetTargetMoveNavPointContainer
			(
				transform.position,										// 現在の座標
				goalPosition,											// 目的地座標
				(int)enemyKind,											// エージェントの種類
				moveSpeed,												// 移動速度
				enemyData.navigationData.nodeUseKind,					// 使うノードの種類		地面/壁/全て
				enemyData.navigationData.navigationMode,				// ナビゲーションモード
				EnemyInfo::GetBlockedObjectDamagePerSecond(enemyData)	// DPS
			)
		);

		// ルートのナビポイントが設定されていたら
		if (navPointNumbers.size() > 0)
		{
			// ナビポイントの数が1つで、そのナビポイントが前回と同じだったら
			if (navPointNumbers.size() == 1 && (navPointNumbers[0] == copyNavPointNumber))
			{
				state = ENEMY_STATE::STAY;
				break;	// 待機状態にしたので、以下の移動処理に移行しない
			}
			navPointIndex		= 0;
			copyNavPointNumber	= navPointNumbers[navPointIndex];
			state				= ENEMY_STATE::MOVE;
		}
		// 移動できないとき
		else
		{
			// 経路探索の終了
			NavgationEnd();
		}
		break;
	case ENEMY_STATE::MOVE:

		// 移動処理
		Move();
		break;
	case ENEMY_STATE::ATTACK:

		if (navPointNumbers.empty())
		{
			state = ENEMY_STATE::SEARCH;
			break;	// 攻撃方向を決めるナビポイントが存在しないので、break
		}

		// 攻撃状態に入った直後または、攻撃のインターバルのカウントが攻撃インターバル以上だったら
		if (lastState != state || (attackIntervalCount += (Time::GameDeltaTime() * actionRate)) > enemyData.attackData.attackInterval)
		{
			// 攻撃処理
			Attack();
			attackIntervalCount = 0.0f;
		}
	
		// 攻撃アニメーションの最大フレーム数を超えていたら
		if (currentFrame > attackMaxFrame)
		{
			// アニメーションフレームを初めに戻す
			animation->SetCurrentFrame(0.0f);
		}
		break;
	default:
		assert(false);
		break;
	}

	int id = state == ENEMY_STATE::SEARCH ? (int)ENEMY_STATE::STAY : (int)ENEMY_STATE::MOVE;

	int animationID = (int)ENEMY_STATE::MOVE;	// 再生するアニメーションID
	// ルートを探索しているとき
	if (state == ENEMY_STATE::SEARCH)
	{
		animationID = (int)ENEMY_STATE::STAY;
	}

	// アニメーションを再生
	animation->Play(id);

	// 行動速度倍率をもとに戻す
	actionRate	= DEFAULT_ACTION_RATE;
	// 現在のフレーム処理前の状態を記録
	lastState	= currentState;
}

void EnemyBase::Move()
{
	// 攻撃ステートの移行をリクエスト
	RequestChangeStateAttack();

	const VECTOR3* navPointPosPtr	= nullptr;								// ナビポイントの座標ポインタ
	VECTOR3 targetVec				= targetPosition - transform.position;	// 次のナビポイントまでのベクトル
	float halfModelSize				= transform.GetLenX() / 2;				// モデルの半径

	switch (moveState)
	{
	case ENEMY_MOVE_STATE::SET:
		
		// コンテナの要素数に対して、インデックスの値が不正だったら
		if (navPointNumbers.size() < navPointIndex + 1)
		{
			state = ENEMY_STATE::SEARCH;
			break;	// 経路を確保するステートに変更したので、break
		}
		navPointPosPtr = enemyManager->GetNavPointPosition(navPointNumbers[navPointIndex]);

		// 座標が見つからないとき
		if (navPointPosPtr == nullptr)
		{
			state = ENEMY_STATE::SEARCH;
			break;	// 経路を確保するステートに変更したので、break
		}
		targetPosition	= *navPointPosPtr;
		moveState		= ENEMY_MOVE_STATE::MOVE;
		break;
	case ENEMY_MOVE_STATE::MOVE:

		// 次のナビポイントに到達したら
		if (targetVec.SquareSize() < NEXT_POINT_DISTANCE_MIN * NEXT_POINT_DISTANCE_MIN)
		{
			// 目的地に到達したら
			if (navPointIndex == navPointNumbers.size() - 1)
			{
				if (!navAlways)
				{
					// 経路探索の終了
					NavgationEnd();
				}
				else
				{
					state = ENEMY_STATE::SEARCH;
				}
				moveState = ENEMY_MOVE_STATE::SET;
				break;	// 目的地に到達したので、break
			}
			else
			{
				moveState = ENEMY_MOVE_STATE::SET;
			}
			// インデクスを進める　ナビポイントナンバーのコンテナの最大の添え字以内に収める
			navPointIndex = navPointIndex >= navPointNumbers.size() - 1 ? navPointNumbers.size() - 1 : navPointIndex + 1;
			break;		// インデクスを変更したので、break
		}

		// ヴェロシティの設定
		velocity = targetVec.Normalize() * ((moveSpeed * Time::GameDeltaTime()) * actionRate);
		break;
	default:
		assert(false);
		break;
	}

	// velocityが0でないとき
	if (!VAllSameF(velocity, 0.0f))
	{
		// 回転処理
		MoveRotate();
	}

	// アニメーション速度の設定 
	animation->SetPlaySpeed(actionRate);

	// ヴェロシティのサイズ率の設定
	velocity *= actionRate;
}

void EnemyBase::MoveRotate()
{
	//_ 進む方向へ回転 _//

	VECTOR3 front	= VECTOR3(0, 0, 1) * transform.GetRotMatrixY();	// 正面ベクトル
	VECTOR3 right	= VECTOR3(1, 0, 0) * transform.GetRotMatrixY();	// 右ベクトル
	VECTOR3 target	= VNorm(transform.position - targetPosition);	// 移動先へのベクトル

	float frontToTargetDot	= VDot(front.Normalize(), target.Normalize());	// 正面に対して移動先のベクトルの、内積値
	float angle				= atan2f(target.x, target.z);					// 移動先に向くまでの回転値

	// 正面に対して移動先のベクトルの差が、MOVE_ROTATE_SPEED以下のとき
	if (frontToTargetDot >= cosf(MOVE_ROTATE_SPEED))
	{
		// 移動先のベクトルに合わせる
		transform.rotation.y = angle;
	}
	// 移動先が右側のとき
	else if (VDot(right, target) > 0)
	{
		// 右へ回転
		transform.rotation.y += MOVE_ROTATE_SPEED;
	}
	// 移動先が左側のとき
	else
	{
		// 左へ回転
		transform.rotation.y -= MOVE_ROTATE_SPEED;
	}
}

void EnemyBase::Attack()
{
	// コンテナの最大インデックス値をnavPointIndex + 1が超えていたら
	if (navPointIndex + 1 >= navPointNumbers.size())
		return;	// 処理を行わない

	VECTOR3 attackTarget	= *enemyManager->GetNavPointPosition(navPointNumbers[navPointIndex + 1]);

	VECTOR3 attackDir		= VNorm(transform.position - attackTarget);
	VECTOR3 attackRot		= attackDir * MGetRotY(0.0f);
	transform.rotation.y	= atan2f(attackRot.x, attackRot.z);

	VECTOR3 attackVec		= VECTOR3(0, 0, -1) * enemyData.attackData.attackReach;	// 攻撃のベクトル
	// ベクトルを回転
	attackVec				= attackVec * transform.GetRotMatrix();					

	// 座標を設定
	attackTransform.position	= transform.position;
	// 攻撃座標を足元座標から、少し上げて当たるようにする
	attackTransform.position.y	+= transform.GetLenY() / 3;

	/*if (attackSphereColl != nullptr)
	{
		delete attackSphereColl;
		attackSphereColl = nullptr;
	}*/

	if (attackRayCollision != nullptr)
	{
		delete attackRayCollision;
		attackRayCollision = nullptr;
	}
	
	//attackSphereColl = new SphereCollision(&attackTransform, COLLISION_OBJECT_KIND::ENEMY_ATTACK, "ENEMY_ATTACK_SPHERE", nullptr, true);
	attackRayCollision = new RayCollision(&attackTransform, COLLISION_OBJECT_KIND::ENEMY_ATTACK, "ENEMY_ATTACK_RAY", nullptr, true);
	// 攻撃レイの設定
	attackRayCollision->SetRayDirection(VZero, attackVec);
	// 一番近くで当たったオブジェクトのみ、登録したHitCara関数をコールバックする
	//attackRayCollision->SetUseNearestCollisionFlag(true);

	for (const auto& attackTr : enemyData.collisionData.attackCollTargetKindList)
	{
		// 当たり判定する相手の設定	   当たり判定をする相手	与えるダメージ量
		//attackSphereColl->SetTargetTag(attackTr.first, attackTr.second);

		// 当たり判定する相手の設定	   当たり判定をする相手	与えるダメージ量
		attackRayCollision->SetTargetTag(attackTr.first, attackTr.second);
	}

	const NODE_BLOCK_STATE* blockedState = enemyManager->GetNavPointBlockState(navPointNumbers[navPointIndex]);	// 現在のナビポイントの潰されているかの状態を取得

	if (blockedState != nullptr)
	{
		// ナビポイントが潰されていなかったら
		if (*blockedState != NODE_BLOCK_STATE::BLOCKED)
		{
			state = ENEMY_STATE::MOVE;
		}
	}

#if GAME_FIRST_PERSON
	attackSphereColl->SetTargetTag(COLLISION_OBJECT_KIND::CORE_BLOCK, 1.0f);
#endif
}

void EnemyBase::RequestChangeStateAttack()
{
	//_ 進行方向に遮断物があった時の処理 _//

	//if (moveState != ENEMY_MOVE_STATE::SET)
	//	return;	// 敵の移動の状態が次に進む座標をセットしている時だったら return

	if (navPointNumbers.size() <= navPointIndex + 1 && navPointNumbers.empty())
		return;	// 次のナビポイントがないとき
	
	const VECTOR3* nextPosition = enemyManager->GetNavPointPosition(navPointNumbers[navPointIndex]);	// 次のナビポイント座標
	VECTOR3 addPosition			= VECTOR3(0.0f, transform.GetLenY() / 2, 0.0f);							// 攻撃を出す高さを上げるための追加座標

	if (nextPosition == nullptr)
		return;	// 次のナビポイント座標ポインタがnullptrの時

	float nextSquareDistance	= VSquareSize(transform.position - *nextPosition);						// 次のナビポイントまでの距離の二乗
	float squaredAttackReach	= enemyData.attackData.attackReach * enemyData.attackData.attackReach;	// 攻撃リーチの距離の二乗

	// 攻撃リーチよりも次のナビポイントまでの距離が遠かったら
	if (nextSquareDistance > squaredAttackReach)
		return;

	// 進む先に防御壁があったら
	if (enemyManager->CheckRaycastStageObject(transform.position + addPosition, *nextPosition + addPosition, std::set<int>{(int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK}))
	{
		state = ENEMY_STATE::ATTACK;
		return;	// 攻撃ステートになったので、return
	}
}

const VECTOR3* EnemyBase::GetGoalPositionPtr()
{
	switch (enemyData.navigationData.navigationTargetKind)
	{
	case EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND::CORE:

		// コアの座標ポインタを返す
		return enemyManager->GetCorePositionPtr();
		break;
	case EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND::PLAYER:

		// プレイヤーの座標ポインタを返す
		return enemyManager->GetPlayerPositionPtr();
		break;
	default:
		assert(false);
		break;
	}
	return nullptr;
}

bool EnemyBase::HitChara(const CollisionHitInfoData& _targetData)
{
	//if (!active)
	//	return true;	// すでに非アクティブだったら return

	bool isDeath		= false;									// 死亡しているかどうか
	VECTOR3 centerPos	= transform.position + VECTOR3(0, 100, 0);	// エフェクトの再生のための中心座標
	std::vector<VECTOR3> hitPosCon;									// 当たった座標コンテナ

	// 当たった場所の情報があったら
	if (_targetData.hitPointData)
	{
		// 当たった座標コンテナの代入
		hitPosCon = _targetData.hitPointData.value().hitPosition;
	}

	switch (_targetData.tag)
	{
	case COLLISION_OBJECT_KIND::SPIKE_BLOCK:

		if (isInvincible)
			break;	// 無敵のときは、ダメージ処理を飛ばす

		invincibleCount = MAX_INVINCIBLE_TIME;
		isInvincible	= true;

		// 下も続けて処理

	case COLLISION_OBJECT_KIND::BULLET:
	case COLLISION_OBJECT_KIND::BOMB_EXPLOSION:
		
		isDeath = CharaBase::HitChara(_targetData);

		// 死亡していたら
		if (isDeath)
		{
			// アクティブフラグをオフにして、敵を削除
			active = false;

			//_ コインをドロップする _//

			int dropCoinNum = enemyData.dropCoinData.dropNum;	// ドロップするコイン数
			
			for (int i = 0;i < dropCoinNum;i++)
			{
				// コインのドロップ
				enemyManager->DropCoin(transform.position);
			}
			// 死亡エフェクトの再生
			EffekseerObjectManager::SetEffect(centerPos, EffekseerObjectManager::EF_ENEMY_DEATH);
			break;	// 死亡したので、以下のヒットエフェクト処理を飛ばす
		}
		
		// ヒットエフェクトの再生
		EffekseerObjectManager::SetEffect(centerPos, EffekseerObjectManager::EF_ENEMY_HIT,4.5f,&transform.position,VECTOR3(0,100,0));
		break;
	case COLLISION_OBJECT_KIND::SLIME_PATCH:

		// 行動速度低下
		actionRate = HIT_SLIME_ACTION_RATE;

		// スライムによる行動速度低下のエフェクト再生が終わっていたら
		if (slowDownEffectTime <= 0.0f)
		{
			// スライムによる行動速度低下のエフェクト再生
			EffekseerObjectManager::SetEffect(transform.position, EffekseerObjectManager::EF_SLIME_SLOW_DOWN,1.0f);
		}
		break;
	case COLLISION_OBJECT_KIND::PLAYER:
		break;
#if !GAME_FIRST_PERSON
	case COLLISION_OBJECT_KIND::CORE_BLOCK:
#endif
		// コアに触れた瞬間、アクティブフラグをオフにして、敵を削除
		active = false;
		break;
	default:
		break;
	}
	return isDeath;
}

void EnemyBase::Draw()
{
	CharaBase::Draw();

#ifdef _DEBUG

#else
	//assert(false && "敵ルートの描画が常にオンです");

#endif // _DEBUG


	if (!isDrawRoot)
		return;	// ルートの描画をしないので return

	for (size_t i = navPointIndex;!navPointNumbers.empty() && i < navPointNumbers.size() - 1;i++)
	{
		const VECTOR3* posNow	= enemyManager->GetNavPointPosition(navPointNumbers[i]);		// 現在座標
		const VECTOR3* posNext	= enemyManager->GetNavPointPosition(navPointNumbers[i + 1]);	// 次の座標

		if (posNow == nullptr || posNext == nullptr)
			break;	// nullptrなので break

		VECTOR3 pos1 = *posNow;		// カプセルの始点
		VECTOR3 pos2 = *posNext;	// カプセルの終点
		// 描画が見えやすいように描画のy座標を高くする
		pos1.y += 10.0f;
		pos2.y += 10.0f;

		float radius = 10.0f;		// カプセルの半径

		// カプセルでルートの描画
		DrawCapsule3D(pos1, pos2, radius, 2, 0xff0000, 0xffffff, true);
	}
}

void EnemyBase::UIDraw()
{
	VECTOR3 screenPos = ConvWorldPosToScreenPos(VECTOR3(transform.position.x, transform.position.y + 210, transform.position.z));	// ワールド座標を画面の座標に変換した画面座標

	// HPがMAXだったら
	if (hp == enemyData.hpMax)
		return;	// HPを描画しない

	// カメラの裏側の座標だったら
	if (screenPos.z > 1.0f)
		return;	// HPを描画しない

	//float line			= 0.5f;									// HPの枠の太さ
	VECTOR2 drawPos		= VECTOR2(screenPos.x, screenPos.y);
	int hpGaugeColor	= GetColor(0, 255, 0);					// HPゲージの色　デフォルトは、黄緑色

	// HPが半分以下だったら
	if (hp <= enemyData.hpMax / 2)
	{
		// HPが半分以下、1/4以上だったら
		if (hp >= enemyData.hpMax / 4)
		{
			hpGaugeColor = GetColor(243, 108, 33);	// HPゲージをオレンジ色にする
		}
		// HPが1/4未満だったら
		else
		{
			hpGaugeColor = GetColor(255, 0, 0);		// HPゲージを赤色にする
		}
	}

#if 1
	// HPゲージの色を再設定
	enemyHPGauge->SetDefaultFillColor(hpGaugeColor);
	enemyHPGauge->Draw(drawPos, hp);
#else
	// HPの下敷きの描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBoxAA(drawPos.x, drawPos.y, drawPos.x + HP_UI_SIZE.x, drawPos.y + HP_UI_SIZE.y, GetColor(100, 100, 100), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// HPのゲージの描画
	DrawBoxAA(drawPos.x, drawPos.y, drawPos.x + (HP_UI_SIZE.x / enemyData.hpMax) * hp, drawPos.y + HP_UI_SIZE.y, hpGaugeColor, true);
	// HPの枠の描画
	DrawBoxAA(drawPos.x - line, drawPos.y - line, drawPos.x + HP_UI_SIZE.x + line, drawPos.y + HP_UI_SIZE.y + line, GetColor(150, 150, 150), false, (float)line * 1.7f);
#endif
}

void EnemyBase::DeleteNavPointNumber()
{
	if (!navPointNumbers.empty())
		navPointNumbers.clear();
}

void EnemyBase::NavgationEnd()
{
	DeleteNavPointNumber();
	state			= ENEMY_STATE::ATTACK;
	navPointIndex	= 0;
}

