#include "Player.h"
#include "../Camera/Camera.h"
#include "../Physics/Physics.h"
#include "../Game/GameController.h"
#include "../Stage/StageManager/StageManager.h"
#include "../Stage/StageObjects/StageObjectBase.h"
#include "../Shot/ShotManager/ShotManager.h"
#include "../Shot/Bullet/BulletManager/BulletManager.h"
#include "../Shot/Bullet/Bullet/Bullet.h"
#include "../Collision/Collisions/SphereCollision.h"
#include "../Collision/Collisions/RayCollision.h"
#include "../Common/Input/Input.h"
#include "../Gauge/Gauge.h"
#include "../Common/Animator/Animator.h"
#include "../Screen.h"
#include "../Stage/Trap/TrapInfo.h"
#include "../Gun/GunDataSerializer/GunDataSerializer.h"
#include "../Common/Sound/SoundID.h"
#include "../Common/Sound/SoundManager.h"
#include "../Common/Function/Function.h"
#include "../Common/Effekseer/EffekseerObjectManager.h"
#include "../Common/JsonSerializer/JsonSerializer.h"
#include "../Common/FileSystemUtils/FileSystemUtils.h"
#include "../Common/UI/UIDrawManager/UIDrawManager.h"
#include "../../ImGui/imgui.h"
#include "../../Library/DebugNew.h"

namespace
{
	constexpr float MIN_MOVE_SPEED		= 100.0f;				// 最低移動速度
	constexpr float MAX_MOVE_SPEED		= 700.0f;				// 最大移動速度
	constexpr float JUMP_FIRST_SPEED	= 22.3f;				// ジャンプの初速
	const VECTOR3 FRICTION				= VECTOR3(80.0f,0.0f, 80.0f);			// 抵抗力
	constexpr float MAX_VIEW			= 90.0f;				// マウス操作時のカメラの最大回転値の度
	constexpr float HP_MAX				= 100.0f;				// 最大HP
	constexpr float MODEL_SIZE			= 100.0f;				// モデルのサイズ
	const VECTOR2 HP_UI_SIZE			= VECTOR2(200, 30);		// HPのUIサイズ
	const VECTOR2 HP_DRAW_POS			=						// HPを描画する画面座標
	VECTOR2
	(
		Screen::WIDTH  - HP_UI_SIZE.x - 35,
		Screen::HEIGHT - 50
	);															
	//const VECTOR2 TRAP_UPGRADING_NUM_UI_SIZE = VECTOR2(100, 15);	// 罠のアップグレード値のUIサイズ

	constexpr float COLL_CHANGE_POS_Y	= 100.0f;				// 球の当たり判定の使用バグである坂道を登れないバグの処置でのY座標変化値
	constexpr float CAM_ADD_POS_Y		= 100.0f;				// カメラの高さ調整用
	const VECTOR3 DEATH_CAM_OFFSET_POS  = VECTOR3(0, 500, -500);// 死亡時のカメラオフセット座標

	constexpr float SHOT_FRONT_VEC_LEN	= 100.0f;				// 銃を撃つときの、正面ベクトルの長さ
	constexpr float SHOT_COOL_TIME		= 1.0f;					// 発射のクールダウン時間
	constexpr float MAX_RESPAWN_TIME	= 5.0f;					// 復活までの時間
	constexpr float MAX_INVINCIBLE_TIME	= 0.7f;					// 無敵の最大時間

	const VECTOR3 DEFAULT_PLAYER_POS	= VECTOR3(0,3000, 0);	// ゲーム開始時のプレイヤーの座標

	const VECTOR3 START_RAY_POS			= VECTOR3(0, 100, 0);		// レイの始点
	const VECTOR3 END_RAY_POS			= VECTOR3(0, -100, 0);		// レイの終点

	
	constexpr float TRAP_ADD_UPGRADE_SPEED			= 0.5f;		// １秒間の罠のアップグレード進行値の加算数% 1.0が100%

	//const VECTOR3 HIT_JUMP_PAD_ADD_VELOCITY		= VECTOR3(10.0f, 30.0f, 10.0f);	// ジャンプパッドにふれたときの、追加ヴェロシティ
	const VECTOR3 JUMP_PAD_VELOCITY_FRICTION	= VECTOR3(30.0f,0.0f, 30.0f);	// ジャンプパッドに乗った時の、ヴェロシティの抵抗力
	constexpr float ADD_VELOCITY_FLAT_SIZE		= 30.0f;						// ジャンプパッドに乗った時のY軸を無視した追加ヴェロシティのサイズ
	constexpr float ADD_VELOCITY_Y				= 20.0f;						// ジャンプパッドに乗った時の追加ヴェロシティのY値

	constexpr float MAX_RELOAD_KEY_DRAW_TIME	= 1.5f;			// リロードキーの最大描画時間

	constexpr float JUDGEMENT_FALL_Y_POS		= -1000.0f;		// 落下した判定とするY座標

	//const std::set<int> TRAP_UPGRADE_KIND =					// アップグレードできる罠の種類
	//{
	//	(int)StageObjectData::STAGE_OBJECT_KIND::TURRET,
	//	//(int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK,
	//};
}

Player::Player()
{
	playerHPGauge			= new Gauge(HP_DRAW_POS, HP_UI_SIZE.x, HP_UI_SIZE.y, 0.0f, HP_MAX);

	transform.position		= DEFAULT_PLAYER_POS;
	transform.size			= VOne * MODEL_SIZE;
	transform.scale			= VOne * 1.7f;

	hModel								= MV1LoadModel("data/models/chara/player/PC.mv1");
	hImage[UI_UPGRADE_COST_LETTER]		= LoadGraph("data/texture/letter/upgrade_cost.png");
	hImage[UI_UPGRADE_COST_NUM_LETTER]	= LoadGraph("data/texture/letter/num_white.png");
	hImage[UI_SLIME_BULLET_NUM_LETTER]	= LoadGraph("data/texture/letter/num_green.png");
	hImage[UI_NORMAL_BULLET_NUM_LETTER]	= LoadGraph("data/texture/letter/num_white.png");
	hImage[UI_SLIME_ICON]				= LoadGraph("data/texture/slime.png");
	hImage[UI_GUN_ICON]					= LoadGraph("data/texture/gun.png");
	hImage[UI_R_KEY]					= LoadGraph("data/texture/r_key.png");
	hImage[UI_1_KEY]					= LoadGraph("data/texture/1_key.png");
	hImage[UI_2_KEY]					= LoadGraph("data/texture/2_key.png");

	for (int i = 0;i < MAX_UI;i++)
		assert(hImage[i] >= 0);

	//モデルを180度回転させてZ+に向かせる
	MV1SetFrameUserLocalMatrix(hModel, 15, MGetRotY(180.0f * DegToRad));//モデル フレーム番号 行列

	animation = new Animator(hModel);
	animation->AddFile(0, "data/models/chara/player/Anim_Neutral.mv1",true,1.0f);

	
	mouse.movePos		= V2IGet(0, 0);
	mouse.pos			= V2IGet(0, 0);
	mouse.rePos			= VZero;

	onDrawCapsule		= false;
	hp					= HP_MAX;

	debugState			= DEBUG_STATE::NORMAL;
	state				= PLAYER_STATE::ACTION;
	attackState			= PLAYER_ATTACK_STATE::SLIME_PATCH_THROW;
	inputPad			= -1;
	getMouse			= true;
	collTrans			= transform;


	physics				= new Physics(transform, velocity, FRICTION, MAX_MOVE_SPEED,true);

	const char* collName	= "PLAYER_BODY_SPHERE";
	sphereColl				= new SphereCollision(&collTrans, COLLISION_OBJECT_KIND::PLAYER, collName, [this](const CollisionHitInfoData& tr) {return HitChara(tr);});
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY);
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::SPIKE_BLOCK);
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::GROUND_BLOCK);
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::WALL_BLOCK);
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::COIN);
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::JUMP_PAD);
	collOwnerNameList.insert(std::make_pair(collName,(int)COLLISION_KIND::SPHERE));

	collName				= "PLAYER_BOTTOM_RAY";
	rayColl					= new RayCollision(&transform, COLLISION_OBJECT_KIND::PLAYER, collName, [this](const CollisionHitInfoData& tr) {return HitChara(tr);});
	rayColl->SetTargetTag(COLLISION_OBJECT_KIND::GROUND_BLOCK);
	rayColl->SetTargetTag(COLLISION_OBJECT_KIND::WALL_BLOCK);
	rayColl->SetRayVec(START_RAY_POS, END_RAY_POS);
	collOwnerNameList.insert(std::make_pair(collName, (int)COLLISION_KIND::RAY));

	hitPositionInfoList.clear();

	// 再生サウンド設定
	//damageSoundID = Sound_ID::PLAYER_DAMAGE_SE;
	//deathSoundID = Sound_ID::PLAYER_DEATH_SE;

	reloadKeyDrawTime	= MAX_RELOAD_KEY_DRAW_TIME;

	// UIの描画関数をUIDrawManagerのコンテナにpush
	uiFunctionIDNumber	= UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this]() { UIDraw(); });
	// 描画順を後に回す
	UIDrawManager::GetUIDrawManagerInstance()->SetDrawOrder(uiFunctionIDNumber, -1);
}

void Player::Init()
{
	gameController	= FindGameObject<GameController>();
	stageManager	= FindGameObject<StageManager>();
	shotManager		= FindGameObject<ShotManager>();
	bulletManager	= FindGameObject<BulletManager>();
	camera			= FindGameObject<Camera>();
	input			= FindGameObject<Input>();

	// サイズを確保する
	guns.resize((int)PLAYER_ATTACK_STATE::MAX);

	for (int i = 0;i < guns.size();i++)
	{
		// 銃の生成とpush
		guns[i] = new Gun(transform, 0, GetGunDataPath(i), this);
	}

	for (auto& gun : guns)
	{
		if (gun != nullptr)
		{
			// Gunクラスの初期化
			gun->Init(shotManager, stageManager);
		}
	}
}

void Player::PushSlimeBulletMax()
{
	if (guns.size() < (int)PLAYER_ATTACK_STATE::MAX)
		return;	// コンテナのサイズが想定よりも小さいので、配列外参照を避ける

	if (guns[(int)PLAYER_ATTACK_STATE::SLIME_PATCH_THROW] == nullptr)
		return;	// nullptrなので以下の処理は行わない

	Gun& slimeGun = *guns[(int)PLAYER_ATTACK_STATE::SLIME_PATCH_THROW];

	slimeGun.SetAmmo(slimeGun.GetGunData().magazineSize);
}

std::string Player::GetGunDataPath(const int& _fileNum)
{
	char setFilePath[CHAR_MAX];
	// ファイル番号に応じたファイルパスの生成
	sprintfDx(setFilePath, "data/gun/player/gun_%d.json", _fileNum);

	return std::string(setFilePath);
}

GunInfo::GunData Player::LoadGunData(const int& _saveFileNum)
{
	// GunDataの読み込み
	return GunDataSerializer::LoadGunData(GetGunDataPath(_saveFileNum));
}

void Player::SaveGunData(const int& _saveFileNum, const GunInfo::GunData& _gunData)
{
	// GunDataの書き出し
	GunDataSerializer::SaveGunData(GetGunDataPath(_saveFileNum), _gunData);
}

Player::~Player()
{
	physics->DestroyMe();

	if (rayColl != nullptr)
	{
		delete rayColl;
		rayColl = nullptr;
	}

	collOwnerNameList.clear();
	hitPositionInfoList.clear();

	if (playerHPGauge != nullptr)
	{
		delete playerHPGauge;
		playerHPGauge = nullptr;
	}

	if (trapUpgradingNumGauge != nullptr)
	{
		delete trapUpgradingNumGauge;
		trapUpgradingNumGauge = nullptr;
	}

	delete animation;

	guns.clear();

	// UIの描画関数をUIDrawManagerのコンテナから削除
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(uiFunctionIDNumber);
}

void Player::Update()
{
	CharaBase::Update();

	// 当たった座標をリセット
	hitPositionInfoList.clear();

	// ジャンプパッドに当たった判定をリセット
	isHitJumpPad = false;

	// ステージマネージャーに座標を毎フレーム送る  描画での簡易カリングで使用
	stageManager->SendPlayerTransform(transform);

	animation->Update();
	// アニメーションの再生
	animation->Play(0);

#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::DragFloat3("Pos", &transform.position.x, 1.0f, -10000.0f, 10000.0f);
	if (ImGui::Button("Reset"))
	{
		transform.position = DEFAULT_PLAYER_POS;
	}
	if (ImGui::Button("Gravity"))
	{
		physics->SetUseGravity(!physics->IsUseGravity());
		physics->SetGravityVec(VZero);
	}
	ImGui::End();
#endif

	// ゲームプレイ中でなかった場合
	if (!gameController->InGame())
		return;	// 以下のプレイヤーの処理を実行しない

	switch (state)
	{
	case Player::PLAYER_STATE::ACTION:

		ActionUpdate();
		break;
	case Player::PLAYER_STATE::DEATH:

		DeathUpdate();
		break;
	case Player::PLAYER_STATE::RESPAWN:

		Respawn();
		break;
	default:
		assert(false);
		break;
	}

#if _DEBUG
	//__ デバッグの切り替え __//

	if (input->GetKeyConfigPut("_D_DEBUG_CAM_CHANGE"))
	{
		int nowDebugState	= (int)debugState;
		nowDebugState++;

		// 進めたintのステート値の余りをDEBUG_STATEの型にキャスト
		debugState			= (DEBUG_STATE)(nowDebugState % (int)DEBUG_STATE::MAX);
		
		// デバック変数に代入
		dPosition			= transform.position;
		dRotation			= transform.rotation;
	}

	if (input->GetKeyConfigPut("_D_DEBUG_POS_TO_PLAYER_POS_SET"))
		transform.position	= dPosition;
	
	if (input->GetKeyConfigPut("_D_DO_GET_MOUSE"))
		getMouse			= !getMouse;

#endif
	//_ 現在坂を上ることができないので、無理やり当たり判定の球の座標をあげる _//

	collTrans				= transform;
	collTrans.position.y	+= COLL_CHANGE_POS_Y;
}

void Player::SetDeadViewRot(VECTOR3& rot, const VECTOR3& mRot)
{
	rot.y += mRot.x;	// 横回転
	rot.x += mRot.y;	// 縦回転

	//_ カメラの向きが真上、および真下を超えていれば補正する _//

	if (rot.x > MAX_VIEW * DegToRad)
		rot.x = MAX_VIEW * DegToRad;

	if (rot.x < -MAX_VIEW * DegToRad)
		rot.x = -MAX_VIEW * DegToRad;
}

const VECTOR3& Player::GetMouse()
{
	// XINPUT_STATE input;
	// GetJoypadXInputState(DX_INPUT_PAD1, &input);

	// マウスポインタの座標を代入
	GetMousePoint(&mouse.pos.x, &mouse.pos.y);

	//_ 画面中央の座標から動いた分の座標を代入 _//

	mouse.movePos.x = mouse.pos.x - Screen::WIDTH_CENTER;
	mouse.movePos.y = mouse.pos.y - Screen::HEIGHT_CENTER;

	// マウスポインタの座標を画面中央にする
	SetMousePoint(Screen::WIDTH_CENTER, Screen::HEIGHT_CENTER);

	// mouseMoveの値を代入する（感度調節のため1000で割る）
	mouse.rePos		= VGet((float)mouse.movePos.x / 1000, (float)mouse.movePos.y / 1000, 0.0f);
	return mouse.rePos;
}

void Player::ActionUpdate()
{
	// ステージから落下した高さにいたら
	if (transform.position.y <= JUDGEMENT_FALL_Y_POS)
		active	= false;

	// アクティブ状態でなかった時
	if (!active)
		state	= PLAYER_STATE::DEATH;

	if (isInvincible)
	{
		// 無敵の最大時間を超えたら
		if ((invincibleCount += Time::GameDeltaTime()) >= MAX_INVINCIBLE_TIME)
		{
			isInvincible	= false;
			invincibleCount = 0.0f;
		}
	}

	//__ 移動操作関係 __//

	if (gameController->GetPlayState() == GameController::PLAY_STATE::FIRST_PERSON)
	{
		VECTOR mouse	= VZero;
		VECTOR3 camPos	= transform.position;

		if (getMouse)
			mouse = GetMouse();

		switch (debugState)
		{
		case Player::DEBUG_STATE::NORMAL:

			SetDeadViewRot(transform.rotation, mouse);


			camPos.y += CAM_ADD_POS_Y;

			camera->FirstPersonCamera(camPos, VECTOR2(transform.rotation.x, transform.rotation.y));

			// 移動
			MoveUpdate();
			// 攻撃
			AttckUpdate();
			// 罠のメンテナンス
			TrapMaintainUpdate();
			// 罠のアップグレード
			TrapUpgradeUpdate();

			//isInvincible = false;

			break;
		case Player::DEBUG_STATE::DEBUG:

			SetDeadViewRot(dRotation, mouse);
			camera->FirstPersonCamera(dPosition, VECTOR2(dRotation.x, dRotation.y));
			DebugMove();

			isInvincible = true;

			break;
		default:
			break;
		}
	}
}

void Player::MoveUpdate()
{
	//inputPad			= GetJoypadInputState(DX_INPUT_KEY_PAD1);
	VECTOR3 setVelocity = VZero;									// 設定ヴェロシティ
	MATRIX rotYMatrix	= MGetRotY(transform.rotation.y);			// Yの回転マトリックス

	const float SPEED	= MIN_MOVE_SPEED * Time::GameDeltaTime();

	if (input->GetKeyConfigPush("MOVE_FRONT"))
		setVelocity += VECTOR3(0, 0,  SPEED) * rotYMatrix;

	if (input->GetKeyConfigPush("MOVE_BACK"))
		setVelocity += VECTOR3(0, 0, -SPEED) * rotYMatrix;

	if (input->GetKeyConfigPush("MOVE_RIGHT"))
		setVelocity += VECTOR3(SPEED,  0, 0) * rotYMatrix;
	
	if (input->GetKeyConfigPush("MOVE_LEFT"))
		setVelocity += VECTOR3(-SPEED, 0, 0) * rotYMatrix;

	if (onGround)
	{
		if (input->GetKeyConfigPut("MOVE_UP"))
			setVelocity += VECTOR3(0, JUMP_FIRST_SPEED, 0);
	}
	// ヴェロシティの追加
	velocity += setVelocity;

	VECTOR3 flatVec		= physics->GetFlatVelocity();	// Y軸を0としたヴェロシティを取得
	float maxDisDelta	= MAX_MOVE_SPEED * Time::GameDeltaTime();

	// 最大速度を超えていたらおさめる
	if (flatVec.SquareSize() > (maxDisDelta * maxDisDelta))
	{
		VECTOR3 norm	= flatVec.Normalize();
		velocity.x		= norm.x * maxDisDelta;
		velocity.z		= norm.z * maxDisDelta;
	}
	// 地面上にいるフラグをリセット　ToDo　よりわかりやすい方法を考える
	onGround = false;
}

void Player::AttckUpdate()
{
	//__ PLAYER_ATTACK_STATEの変更 __//

	int wheelMove			= input->GetMouseInput()->GetMouseWheelFrame();	// 動いたマウスホイール値
	const int MAX_GUN_LIST	= (int)guns.size() - 1;							// 罠の種類の最大数

	// ホイールが動いていたら
	if (wheelMove != 0)
	{
		// 1 or -1に値を収める
		wheelMove = std::clamp(wheelMove, -1, 1);
		// ホイール値のプラスマイナスを逆にして、ユーザーが操作しやすいようにする
		wheelMove *= -1;

		if (wheelMove < 0)
		{
			// 値をプラスして変更
			attackState = (PLAYER_ATTACK_STATE)((int)attackState + wheelMove < 0 ? MAX_GUN_LIST : (int)attackState - 1);
		}
		else
		{
			// 値をマイナスして変更
			attackState = (PLAYER_ATTACK_STATE)((int)attackState + wheelMove > MAX_GUN_LIST ? 0 : (int)attackState + 1);
		}
	}

	if (input->GetKeyConfigPut("SELECT_1"))
		attackState = PLAYER_ATTACK_STATE::SLIME_PATCH_THROW;

	if (input->GetKeyConfigPut("SELECT_2"))
		attackState = PLAYER_ATTACK_STATE::NORMAL_BULLET_SHOT;

	// ショットの処理を実行
	ShotUpdate((int)attackState);
}

void Player::ShotUpdate(const int& _gunConIndex)
{
	assert(guns.size() - 1 >= _gunConIndex && _gunConIndex >= 0);

	if (guns[_gunConIndex] == nullptr)
		return;	// nullptrなので以下の処理は行わない

	isShotFail = false;

	if (input->GetKeyConfigPut("RELOAD"))
	{
		// 銃をリロードする
		guns[_gunConIndex]->Reload();
	}

	if (input->GetKeyConfigPut("SHOT"))
	{
		VECTOR3 shotDir		= VECTOR3(0, 0, 1) * transform.GetRotMatrix();	// 弾の発射方向
		VECTOR3 shotOffset	= VZero;										// 弾の発射座標のオフセット座標
		// カメラの高さに合わせる
		shotOffset.y		= CAM_ADD_POS_Y;
		// 少し前にずらす
		shotOffset			+= shotDir * SHOT_FRONT_VEC_LEN;

		// 弾切れの時
		if (guns[_gunConIndex]->GetAmmo() <= 0)
		{
			// 弾切れのサウンドを再生　
			SoundManager::PlaySE(Sound_ID::BULLET_EMPTY_SE);
		}

		// 弾の発射
		if (!guns[_gunConIndex]->Shot(shotOffset, shotDir))
		{
			// 発射に失敗したのでフラグをtrue
			isShotFail = true;
		}
	}
}

void Player::TrapMaintainUpdate()
{
	VECTOR3 startPos	= camera->GetPosition();								// レイの始点
	VECTOR3 dir			= VNorm(camera->GetTarget() - startPos);				// レイの方向
	VECTOR3 endPos		= dir * TrapInfo::MAX_TRAP_INTERACT_DISTANCE + startPos;// レイの終点

	// プレイヤーが見ている方向に罠があり、メンテナンスができる状態か判定
	const StageObjectBase* canMaintainStageObj = stageManager->RaycastGetCanMaintainTrap(startPos, endPos);

	// メンテナンスができないとき
	if (canMaintainStageObj == nullptr)
		return;	// 処理を飛ばす

	if (input->GetKeyConfigPush("TRAP_MAINTAIN"))
	{
		// メンテナンスをする
		stageManager->TrapMaintaining(canMaintainStageObj, 1.0f);
	}
}

void Player::TrapUpgradeUpdate()
{
	VECTOR3 startPos	= camera->GetPosition();								// レイの始点
	VECTOR3 dir			= VNorm(camera->GetTarget() - startPos);				// レイの方向
	VECTOR3 endPos		= dir * TrapInfo::MAX_TRAP_INTERACT_DISTANCE + startPos;// レイの終点

	// プレイヤーが見ている方向に罠があり、アップグレードができる状態か判定
	const StageObjectBase* upgradeStageObj = stageManager->RaycastGetCanUpgradTrap(startPos, endPos);

	// アップグレードができないとき
	if (upgradeStageObj == nullptr)
	{
		canTrapUpgrade	= false;
		isTrapUpgrading = false;
		return;	// 処理を飛ばす
	}
	
	canTrapUpgrade		= true;
	// 座標を記録する
	upgradeTrapPosition = upgradeStageObj->GetTransform().position;
	// ステージオブジェクトの種類を記録する
	upgradeTrapKind		= (int)upgradeStageObj->GetStageObjectKind();

	int beforeTrapLevel	= stageManager->GetTrapLevel(upgradeStageObj);	// アップグレード処理前のレベル
	int nextTrapLevel	= beforeTrapLevel + 1;							// アップグレード時のレベル

	bool canUpgrade		= true;	// アップグレードができるかどうか

	// 現在のコイン数ではアップグレードができないとき
	if (TrapInfo::GetUpgradeCost(upgradeStageObj->GetStageObjectKind(), nextTrapLevel) > gameController->GetHoldCoin())
		canUpgrade = false;

	if (input->GetKeyConfigPush("TRAP_UPGRADE"))
	{
		// アップグレードができないとき
		if (!canUpgrade)
		{
			isTrapUpgrading = false;
			// コイン不足状態を設定
			gameController->SetInsufficientCoin();

			// Donotサウンドが再生されていなかったら
			if (!SoundManager::IsPlaySound(Sound_ID::DO_NOT_SE))
			{
				// Donotサウンド再生
				SoundManager::PlaySE(Sound_ID::DO_NOT_SE);
			}
		}
		else
		{
			// 罠のアップグレード値を増やす
			trapUpgradingReturnNum	= stageManager->TrapUpgrade(upgradeStageObj, TRAP_ADD_UPGRADE_SPEED * Time::GameDeltaTime());
			isTrapUpgrading			= true;

			int afterTrapLevel		= stageManager->GetTrapLevel(upgradeStageObj);	// アップグレード処理後のレベル

			// レベルアップしていたら　アップグレードが完了したら
			if (nextTrapLevel == afterTrapLevel)
			{
				// コインを減らす
				gameController->SubHoldCoin(TrapInfo::GetUpgradeCost(upgradeStageObj->GetStageObjectKind(), afterTrapLevel));
			}
		}
	}
	else
	{
		isTrapUpgrading			= false;
	}
}

void Player::DeathUpdate()
{
	// 死亡してからの経過時間が復活の時間を超えたら
	if ((respawnCount += Time::GameDeltaTime()) > MAX_RESPAWN_TIME)
	{
		state				= PLAYER_STATE::RESPAWN;
		respawnCount		= 0.0f;
		velocity			= VZero;
	}

	const VECTOR3* corePos	= stageManager->GetCorePosition();

	if (corePos == nullptr)
		return;	// 処理を飛ばす

	camera->SetPosition(*corePos + DEATH_CAM_OFFSET_POS);
	camera->SetTarget(*corePos);
}

void Player::Respawn()
{
	transform.position	= DEFAULT_PLAYER_POS;
	hp					= HP_MAX;
	active				= true;

	state				= PLAYER_STATE::ACTION;
}

void Player::DebugMove()
{
	VECTOR3 setVelocity = VZero;
	VECTORXZ vec		= VECTORXZ();

	float SPEED			= MAX_MOVE_SPEED * Time::DeltaTime();

	if (CheckHitKey(KEY_INPUT_LCONTROL))
	{
		// スピードを速くする
		SPEED *= 2.0f;
	}

	if (input->GetKeyConfigPush("MOVE_FRONT"))
		setVelocity = VGet(0.0f, 0.0f, SPEED);

	if (input->GetKeyConfigPush("MOVE_BACK"))
		setVelocity = VGet(0.0f, 0.0f, -SPEED);

	if (input->GetKeyConfigPush("MOVE_LEFT"))
	{
		//_ 左かに歩き _//

		vec.x		= sinf(-90 * DegToRad) * SPEED;
		vec.z		= cosf(-90 * DegToRad) * SPEED;
		setVelocity += VGet(vec.x, 0, vec.z);
	}

	if (input->GetKeyConfigPush("MOVE_RIGHT"))
	{
		//_ 右かに歩き _//

		vec.x		= sinf(90 * DegToRad) * SPEED;
		vec.z		= cosf(90 * DegToRad) * SPEED;
		setVelocity += VGet(vec.x, 0, vec.z);
	}

	if (input->GetKeyConfigPush("MOVE_UP"))
		dPosition.y += SPEED;

	if (input->GetKeyConfigPush("_D_MOVE_DOWN"))
		dPosition.y -= SPEED;

	dVelocity		= setVelocity;

	//Yの横回転を取得
	MATRIX mRotY	= MGetRotY(dRotation.y);

	//velocityベクトルの向きを向いている方向にする
	dVelocity		= dVelocity * mRotY;//VTransform(回す対象の座標、中心とする回転)

	//座標にvelocityベクトルを加える
	dPosition		+= dVelocity;
}

bool Player::HitChara(const CollisionHitInfoData& _targetData)
{
	if (_targetData.tag == COLLISION_OBJECT_KIND::GROUND_BLOCK || _targetData.tag == COLLISION_OBJECT_KIND::WALL_BLOCK)
	{
		// 押し戻し
		PushBackOnCollision(_targetData);
		return false;
	}
	else if (_targetData.tag == COLLISION_OBJECT_KIND::COIN)
	{
		gameController->AddHoldCoin();
		return false;
	}
	else if (_targetData.tag == COLLISION_OBJECT_KIND::JUMP_PAD)
	{
		// ジャンプパッドに当たってなかったら
		if (!isHitJumpPad)
		{
			VECTOR3 flatNorm = physics->GetFlatVelocity().Normalize();	// Y軸を無視したヴェロシティの正規化ベクトル

			VECTOR3 addVelocity	= flatNorm * ADD_VELOCITY_FLAT_SIZE;	// 追加ヴェロシティ
			//addVelocity.y		= ADD_VELOCITY_Y;

			// 追加ヴェロシティを設定する
			physics->SetAddVelocity(addVelocity, JUMP_PAD_VELOCITY_FRICTION);

			// ジャンプパッドに当たったので、上に上がる	// Y軸のヴェロシティは重力の方で、抵抗値が存在するので、直接ヴェロシティを設定
			velocity.y = ADD_VELOCITY_Y;

			// ジャンプパッドに当たったので、フラグをtrue
			isHitJumpPad = true;
		}
		return false;
	}
	else
	{
		if (!isInvincible)
		{
			CharaBase::HitChara(_targetData);
			// 無敵をオン
			isInvincible = true;
			// ヒットエフェクトを再生
			EffekseerObjectManager::SetEffect(VZero, EffekseerObjectManager::EF_PLAYER_HIT, 4.5f, &transform.position, VECTOR3(0, 100, 0));
		}
	}
	return false;
}

void Player::PushBackOnCollision(const CollisionHitInfoData& _targetData)
{
	if (!_targetData.hitPointData)
		return;	// 当たった場所の情報がなかったら

	std::vector<VECTOR3> hitPosCon	= _targetData.hitPointData.value().hitPosition;	// あたった座標コンテナ
	std::vector<VECTOR3> hitNormCon = _targetData.hitPointData.value().hitNormList;	// あたった法線コンテナ
	VECTOR3 thisPos					= transform.position;							// キャラクターの座標
	VECTOR3 hitPos					= VZero;										// あたった座標
	VECTOR3 normSum					= VZero;										// 法線の合計値
	float thisRadius				= 0.0f;											// キャラクターの半径
	VECTOR3 pushDir					= VZero;										// 押し返し方向ベクトル
	float pushLen					= 0.0f; 										// 押し返しの長さ
	VECTOR3 pushVec					= VZero;										// 押し返しベクトル
	
	if (collOwnerNameList.find(_targetData.targetOwnerName) == collOwnerNameList.end())
		assert(false && "当たり判定オブジェクトの名前が一致しませんでした");
	
	// プレイヤーが持っている当たり判定オブジェクトの種類を判定
	switch ((COLLISION_KIND)collOwnerNameList[_targetData.targetOwnerName])
	{
	case COLLISION_KIND::SPHERE:
		
		// 球の押し返し
		PushBackOnCollisionSphere(hitPosCon, hitNormCon);
		break;
	case COLLISION_KIND::OBB:
		break;
	case COLLISION_KIND::RAY:

		hitPos		= hitPosCon[0];	// 当たった座標を代入  レイの当たり判定は当たった場所が一か所

		if (thisPos.y < hitPos.y)
			pushVec = (thisPos + END_RAY_POS) - hitPos;
		else
			pushVec = (thisPos - START_RAY_POS) - hitPos;

		// 重力をリセット
		//physics->SetGravityVec(VZero);
		// 地面の上にいる
		onGround	= true;

		// 重力によって下にヴェロシティのY軸がマイナスの時かつY軸ヴェロシティに対して加える力がほぼ等しいとき	ほぼ等しくないと、空中に高く押し出されることがある
		if (velocity.y < 0.0f)
		{
			constexpr float ERROR_Y_VEC = 5.0f;	// Y軸比較の許容誤差
			// veloctiy値がpushVecよりも、大きいまたは、ほぼ同じだったら
			if (FSame(fabsf(velocity.y), fabsf(pushVec.y), ERROR_Y_VEC) || fabsf(velocity.y) >= fabsf(pushVec.y))
			{
				// velocityに押し返しヴェクトルを加える
				physics->AddForce(VScale(pushVec, -1.0f));
			}
		}
		break;
	case COLLISION_KIND::MODEL:
		break;
	default:
		assert(false);
		break;
	}
	
	// 押し戻す
	transform.position -= pushVec;
}

void Player::PushBackOnCollisionSphere(const std::vector<VECTOR3>& _hitPosCon, const std::vector<VECTOR3>& _hitNormCon)
{
	std::unordered_map<int, VECTOR3> saveHitNormCon;	// 押し返し処理に使う当たった法線を保存
	int hitNromIndex = 0;								// 当たった法線コンテナのインデックス

	for (auto hitNormItr = _hitNormCon.begin();hitNormItr != _hitNormCon.end();hitNormItr++, hitNromIndex++)
	{
		for (auto& saveHitNorm : saveHitNormCon)
		{
			// すでに同じ法線方向が保存されていたら
			if (VAllSame(saveHitNorm.second,*hitNormItr))
				continue;	// 法線方向の追加をしない
		}
		// 異なった法線方向とインデックスの保存
		saveHitNormCon.emplace(hitNromIndex, *hitNormItr);
	}

	VECTOR3 thisPos		= transform.position;		// 自身の座標
	float thisRadius	= transform.GetLenX() / 2;	// 自身の半径
	float pushLen		= 0.0f;						// 押し返す量
	VECTOR3 pushVec		= VZero;					// 押し返すヴェクトル

	for (const auto& saveHitNorm : saveHitNormCon)
	{
		VECTOR3 hitPos	= _hitPosCon[saveHitNorm.first];
		VECTOR3 hitNorm = saveHitNorm.second;

		thisPos			= transform.position;

		// yを無視 //
		hitPos.y	= 0.0f;
		hitNorm.y	= 0.0f;
		thisPos.y	= 0.0f;

		//_ 押し戻す値の計算 _//

		pushVec = hitPos - thisPos;

		// 押し返しヴェクトルのxとzが0のとき
		if (FSame(pushVec.x, 0.0f) && FSame(pushVec.z, 0.0f))
			continue;	// 押し返し処理を飛ばす

		pushLen = VSize(pushVec) - thisRadius;
		pushVec = hitNorm * pushLen;		// 半径を考慮して押し返しを計算

		transform.position -= pushVec;

		if (fabsf(velocity.x) < fabsf(pushVec.x))	
			velocity.x = 0.0f;
		else
			velocity.x -= pushVec.x;

		if (fabsf(velocity.z) < fabsf(pushVec.z))
			velocity.z = 0.0f;
		else
			velocity.z -= pushVec.z;
	}
}

void Player::Draw()
{
	if (gameController->GetPlayState() == GameController::PLAY_STATE::OBJECT_PUT && state == PLAYER_STATE::ACTION)
	{
		// 描画用に回転をリセット //
		transform.rotation.x = 0.0f;
		transform.rotation.z = 0.0f;

		CharaBase::Draw();
	}
}

void Player::UIDraw()
{
	if (debugState == DEBUG_STATE::NORMAL && gameController->InGame() && gameController->GetPlayState() == GameController::PLAY_STATE::FIRST_PERSON)
	{
		// レティクル
		DrawCircle(Screen::WIDTH_CENTER, Screen::HEIGHT_CENTER, 2, 0xffffff, true);

		// 弾情報の描画
		BulletInfoDraw(attackState);

		// リロードキーの描画
		ReloadDraw(attackState);
	}

	// HPの描画
	HPDraw();

	if (state == PLAYER_STATE::DEATH)
	{
		VECTOR2I respawnLetterUnderDrawPos	= VECTOR2I(Screen::WIDTH_CENTER - 48, Screen::HEIGHT_CENTER - 70);	// リスポーン文字の描画座標(下)
		VECTOR2I respawnLetterDrawPos		= VECTOR2I(Screen::WIDTH_CENTER - 46, Screen::HEIGHT_CENTER - 70);	// リスポーン文字の描画座標(上)

		// リスポーンまで　の描画 //
		DrawFormatString(respawnLetterUnderDrawPos.x, respawnLetterUnderDrawPos.y, 0x111111, "リスポーンまで");
		DrawFormatString(respawnLetterDrawPos.x, respawnLetterDrawPos.y, 0xeeeeee, "リスポーンまで");

		float respawnCountDown = MAX_RESPAWN_TIME - respawnCount;

		VECTOR2I respawnTimeUnderDrawPos	= VECTOR2I(Screen::WIDTH_CENTER - 32, Screen::HEIGHT_CENTER - 50);	// リスポーンタイムの描画座標(下)
		VECTOR2I respawnTimeDrawPos			= VECTOR2I(Screen::WIDTH_CENTER - 30, Screen::HEIGHT_CENTER - 50);	// リスポーンタイムの描画座標(上)

		SetFontSize(32);
		// リスポーン時間の描画 //
		DrawFormatString(respawnTimeUnderDrawPos.x, respawnTimeUnderDrawPos.y, 0x111111, "%.2lf", respawnCountDown);
		DrawFormatString(respawnTimeDrawPos.x, respawnTimeDrawPos.y, 0xeeeeee, "%.2lf", respawnCountDown);
		SetFontSize(DEFAULT_FONT_SIZE);
	}
}

void Player::HPDraw()
{
	int hpGaugeColor = GetColor(0, 255, 0);

	// HPが半分以下だったら
	if (hp <= HP_MAX / 2)
	{
		// HPが1/4以下だったら
		if (hp <= HP_MAX / 4)
			hpGaugeColor = GetColor(255, 0, 0);		// HPをオレンジ色にする
		else
			hpGaugeColor = GetColor(243, 108, 33);	// HPをオレンジ色にする
	}

	// デフォルトのゲージの色を設定
	playerHPGauge->SetDefaultFillColor(hpGaugeColor);
	// HPUIの描画
	playerHPGauge->Draw(hp);
}

void Player::BulletChangeKeyDraw()
{
	float changeKeyDistance				= 120.0f;	// 弾の変更キー同士の距離
	VECTOR2 changeKeyCenterDrawPos		= VECTOR2(450.0f, Screen::HEIGHT - 40.0f);								// 弾の変更キーの - を中心とした描画座標
	VECTOR2 leftSideChangeKeyDrawPos	= VECTOR2(changeKeyCenterDrawPos.x - changeKeyDistance / 2, changeKeyCenterDrawPos.y);	// 左側の変更キーの弾の描画座標
	VECTOR2 rightSideChangeKeyDrawPos	= VECTOR2(changeKeyCenterDrawPos.x + changeKeyDistance / 2, changeKeyCenterDrawPos.y);	// 右側の変更キーの弾の描画座標
	float keyDrawScale					= 2.0f;		// 弾の変更キーの描画スケール

	// 1キーの描画
	DrawRectRotaGraphF(leftSideChangeKeyDrawPos.x, leftSideChangeKeyDrawPos.y, 0, 0, 32, 32, keyDrawScale, 0.0f, hImage[UI_1_KEY], true);

	// - の描画 //
	VECTOR2 lineSize = VECTOR2(15.0f, 4.0f);	// 線の縦横幅
	DrawBoxAA(changeKeyCenterDrawPos.x - lineSize.x, changeKeyCenterDrawPos.y - lineSize.y, changeKeyCenterDrawPos.x + lineSize.x, changeKeyCenterDrawPos.y + lineSize.y, 0xffffff, true);

	// 2キーの描画
	DrawRectRotaGraphF(rightSideChangeKeyDrawPos.x, rightSideChangeKeyDrawPos.y, 0, 0, 32, 32, keyDrawScale, 0.0f, hImage[UI_2_KEY], true);
}

void Player::BulletInfoDraw(const PLAYER_ATTACK_STATE& _playerAttackState)
{
	assert(guns.size() - 1 >= (int)_playerAttackState);
	
	// 発射する弾の変更キーの描画
	BulletChangeKeyDraw();

	if (guns[(int)_playerAttackState] == nullptr)
		return;	// nullptrなので以下の処理は行わない

	Gun& gun						= *guns[(int)_playerAttackState];

	VECTOR2 drawCenterPos			= VECTOR2(130.0f, Screen::HEIGHT - 40.0f);				// 描画の / を中心とした描画座標1
	VECTOR2 nowHoldBulletDrawPos	= VECTOR2(drawCenterPos.x - 45.0f, drawCenterPos.y);	// 現在の弾数の描画座標
	VECTOR2 maxHoldBulletDrawPos	= VECTOR2(drawCenterPos.x + 75.0f, drawCenterPos.y);	// 最大の弾数の描画座標
	int nowHoldBulletZeroNum		= 2;													// 現在の弾数の前ゼロのゼロの数
	int maxHoldBulletZeroNum		= 1;							// 最大の弾数の描画座標
	float bulletNumDrawScale		= 1.0f;							// 弾数の描画スケール
	VECTOR2I bulletNumSize			= VECTOR2I(794 / 10, 159 / 2);	// 弾数の描画一桁のサイズ
	int interval					= 794 / 18;						// 数字の描画間隔
	int gunBulletNumImage			= -1;							// 銃の弾の数字画像ハンドル

	VECTOR2 iconDrawPos	= VECTOR2(drawCenterPos.x + 170.0f, drawCenterPos.y);	// アイコンの描画座標
	float iconDrawScale = 0.1f;													// アイコンの描画スケール
	int iconImage		= -1;													// アイコンの画像ハンドル
	

	switch (_playerAttackState)
	{
	case PLAYER_ATTACK_STATE::SLIME_PATCH_THROW:
		
		gunBulletNumImage = hImage[UI_SLIME_BULLET_NUM_LETTER];
		iconImage		  = hImage[UI_SLIME_ICON];
		break;
	case PLAYER_ATTACK_STATE::NORMAL_BULLET_SHOT:

		gunBulletNumImage = hImage[UI_NORMAL_BULLET_NUM_LETTER];
		iconImage		  = hImage[UI_GUN_ICON];
		break;
	default:
		assert(false);
		break;
	}

	// 現在の弾数の描画
	DrawRotaNum(nowHoldBulletDrawPos.x, nowHoldBulletDrawPos.y, gun.GetAmmo(), interval, gunBulletNumImage, bulletNumSize.x, bulletNumSize.y, 0, bulletNumDrawScale, 0.0f, nowHoldBulletZeroNum);
	
	// / の描画
	DrawRectRotaGraphF(drawCenterPos.x, drawCenterPos.y, 0, bulletNumSize.y, bulletNumSize.x, bulletNumSize.y, bulletNumDrawScale, 0.0, gunBulletNumImage, true);
	
	// 最大弾数の描画
	DrawRotaNum(maxHoldBulletDrawPos.x, maxHoldBulletDrawPos.y, gun.GetGunData().magazineSize, interval, gunBulletNumImage, bulletNumSize.x, bulletNumSize.y, 0, bulletNumDrawScale, 0.0f, maxHoldBulletZeroNum);
	
	// アイコンの描画
	DrawRotaGraphF(iconDrawPos.x, iconDrawPos.y, iconDrawScale, 0.0f, iconImage, true);
}

void Player::ReloadDraw(const PLAYER_ATTACK_STATE& _playerAttackState)
{
	if (_playerAttackState == PLAYER_ATTACK_STATE::SLIME_PATCH_THROW)
		return;	// 通常の銃でしかリロードを行わないので、return

	assert(guns.size() - 1 >= (int)_playerAttackState);

	if (guns[(int)_playerAttackState] == nullptr)
		return;	// nullptrなので以下の処理は行わない

	VECTOR2 centerDrawPos			= VECTOR2(Screen::WIDTH_CENTER, Screen::HEIGHT_CENTER + 60.0f);	// 起点となる中心座標
	VECTOR2 reloadTimeDrawPos		= VECTOR2(centerDrawPos.x - 25.0f, centerDrawPos.y);
	VECTOR2 reloadTimeUnderDrawPos	= VECTOR2(reloadTimeDrawPos.x + 2.0f, reloadTimeDrawPos.y + 21.5f);

	float reloadTime			= guns[(int)_playerAttackState]->GetReloadTime();	// 現在のリロード時間

	// マガジン内に弾がなくなったら
	if (guns[(int)_playerAttackState]->GetAmmo() <= 0)
	{
		// Gunの発射に失敗したら
		if (isShotFail)
		{
			// 描画するリロード時間のリセット
			reloadKeyDrawTime = 0.0f;
		}

		if (reloadTime <= 0.0f)
		{
			// リロードキーの描画時間内だったら
			if ((reloadKeyDrawTime += Time::GameDeltaTime()) <= MAX_RELOAD_KEY_DRAW_TIME)
			{
				int alpha							= (int)((MAX_RELOAD_KEY_DRAW_TIME - (reloadKeyDrawTime / MAX_RELOAD_KEY_DRAW_TIME)) * 255);	// 文字のアルファ値　時間が経つにつれて、薄くなる					
				VECTOR2 reloadLetterUnderDrawPos	= VECTOR2(reloadTimeDrawPos.x - 15.0f, reloadTimeDrawPos.y + 0.15f);	// リロード文字の描画座標(下)
				VECTOR2 reloadLetterDrawPos			= VECTOR2(reloadTimeDrawPos.x - 17.0f, reloadTimeDrawPos.y);			// リロード文字の描画座標(上)
				VECTOR2 reloadKeyDrawPos			= VECTOR2(centerDrawPos.x, centerDrawPos.y + 50.0f);					// リロードキーの画像描画座標

				// 描画のアルファ値の変更
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				
				SetFontSize(20);

				// リロード 文字の描画 //
				DrawFormatStringF(reloadLetterUnderDrawPos.x, reloadLetterUnderDrawPos.y, 0x222222, "リロード");
				DrawFormatStringF(reloadLetterDrawPos.x, reloadLetterDrawPos.y, 0xffffff, "リロード");

				// 描画のアルファ値をデフォルトに戻す
				SetFontSize(DEFAULT_FONT_SIZE);

				// リロードするRキーを描画
				DrawRectRotaGraphF(reloadKeyDrawPos.x, reloadKeyDrawPos.y, 0, 0, 32, 32, 1.9f, 0.0f, hImage[UI_R_KEY], true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	}

	// リロード中だったら
	if (guns[(int)_playerAttackState]->IsReloading())
	{

		SetFontSize(30);

		// 残りのリロード時間の描画 // 
		DrawFormatStringF(reloadTimeUnderDrawPos.x, reloadTimeUnderDrawPos.y, 0x222222, "%.1lf", guns[(int)_playerAttackState]->GetGunData().reloadTime - reloadTime);
		DrawFormatStringF(reloadTimeDrawPos.x, reloadTimeDrawPos.y + 20.0f, 0xffffff, "%.1lf", guns[(int)_playerAttackState]->GetGunData().reloadTime - reloadTime);

		VECTOR2 reloadingLetterUnderDrawPos = VECTOR2(reloadTimeDrawPos.x - 32.0f, reloadTimeDrawPos.y + 0.15f);	// リロード中文字の描画座標(下)
		VECTOR2 reloadingLetterDrawPos		= VECTOR2(reloadTimeDrawPos.x - 30.0f, reloadTimeDrawPos.y);			// リロード中文字の描画座標(上)

		SetFontSize(20);
		// リロード中 文字の描画 //
		DrawFormatStringF(reloadingLetterUnderDrawPos.x, reloadingLetterUnderDrawPos.y, 0x222222, "リロード中");
		DrawFormatStringF(reloadingLetterDrawPos.x, reloadingLetterDrawPos.y, 0xffffff, "リロード中");

		SetFontSize(DEFAULT_FONT_SIZE);
	}
}