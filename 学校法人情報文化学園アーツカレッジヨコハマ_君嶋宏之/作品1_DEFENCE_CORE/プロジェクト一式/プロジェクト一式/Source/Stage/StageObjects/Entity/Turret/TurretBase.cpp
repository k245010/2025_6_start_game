#include "TurretBase.h"
#include "../../../EntityController/EntityController.h"
#include "../../../Trap/TrapInfo.h"
#include "../../../../Camera/Camera.h"
#include "../../../../Collision/Collisions/OBBCollision.h"
#include "../../../../Collision/Collisions/RayCollision.h"
#include "../../../../Gun/Gun/Gun.h"
#include "../../../../Gun/GunDataSerializer/GunDataSerializer.h"
#include "../../../../Common/Effekseer/EffekseerObjectManager.h"
#include "../../../../Common/Sound/SoundManager.h"
#include "../../../../../ImGui/imgui.h"
#include "../../../../Gauge/Gauge.h"
#include "../../../../Screen.h"
#include "../../../../Common/Color/Color.h"
#include "../../../../../Library/DebugNew.h"


#define DEFAULT_SHOT (1)	// 通常発射

namespace
{
	//constexpr float SHOT_SPEED = 10000.0f;						// 発射速度

	const std::set<int> COLL_RAY_TARGET_STAGE_OBJECT_KIND =		// 遮蔽物としてレイでの当たり判定を行うステージオブジェクトの種類コンテナ
	{
		(int)StageObjectData::STAGE_OBJECT_KIND::GROUND_BLOCK,
		(int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK,
	};

	//constexpr float MAX_DRAW_UI_DISTANCE		= 500.0f;			// UIの最大描画距離
	const VECTOR2 TRAP_UPGRADING_NUM_UI_SIZE	= VECTOR2(100, 15);	// 罠のアップグレード値のUIサイズ

	constexpr int MAX_TRAP_LEVEL					= 2;

	constexpr float MAX_TRAP_WARNING_DRAW_SCALE		= 0.9f;			// トラップの警告の最小描画スケール
	constexpr float MIN_TRAP_WARNING_DRAW_SCALE		= 0.1f;			// トラップの警告の最大描画スケール	
	constexpr float TRAP_WARNING_DRAW_SCALE_RATE	= 0.0001f;		// トラップの警告の1.0f距離あたりの、描画の拡縮の大きさ

	constexpr float MIN_TRAP_WARNING_DRAW_DISTANCE	= 150.0f;		// トラップの警告の最小描画距離　近すぎる場合は描画しない

	//constexpr float DEFAULT_SHOT_DISTANCE			= StageInfo::BLOCK_SIZE * 5.0f;	// レベル1の射程距離
}

TurretBase::TurretBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : EntityBase(_trans, _modelData, _kind, _hp, _putPlaceKind)
{
	obbColl->SetTagMe(COLLISION_OBJECT_KIND::TURRET);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ERASER);

	trapUpgradingNumGauge = new Gauge(VECTOR2(0.0f, 0.0f), TRAP_UPGRADING_NUM_UI_SIZE.x, TRAP_UPGRADING_NUM_UI_SIZE.y, 0.0f, 1.0f);
	// ゲージが減るときのゲージの描画をしないように設定
	trapUpgradingNumGauge->SetDrawDelay(false);

	// 発射間隔1秒で登録
	//EntityController::GetEntityController()->SetShotInterval(this, 1.0f);

	shotDistance = TrapInfo::GetViewImpactRadius(_kind,1);

	// 底面座標
	VECTOR3 bottomPos = transform.position - VNormY * (transform.GetLenY() / 2);

	// 罠の設置のエフェクト再生
	EffekseerObjectManager::SetEffect(bottomPos, EffekseerObjectManager::EF_TRAP_PUT, 1.5f);

	// 個々のモデルハンドルで当たり判定をするために、モデルの色を変更するので、元モデルハンドルをコピーしたものを代入する　
	// 一つのモデルハンドルで当たり判定をすると、１オブジェクトのみの当たり判定になってしまう
	hModel = MV1DuplicateModel(hModel);
	// モデルの当たり判定のセットアップのし直し
	SetupCollInfo(hModel, _trans.GetMatrix());

	// 3Dモデルに含まれる1番目(ベース部分)のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 1, Color(0.3f, 0.3f, 0.3f, 0.5f).GetColorF4());

	int billboardImageHandlBase = EntityController::GetEntityController()->GetEntityTextureHandle("warning");	// ビルボード画像の元ハンドル
	assert(billboardImageHandlBase >= 0);

	// 画像の一部分だけをハンドルにのせる
	billboardImageHandl[BILL_WARNING] = DerivationGraph(0, 0, 1200 / 3, 768 / 2, billboardImageHandlBase);
	//DeleteGraph(billboardImageHandlBase);	// 使わなくなった元ハンドルを削除

	billboardImageHandlBase		= EntityController::GetEntityController()->GetEntityTextureHandle("f_key");	// ビルボード画像の元ハンドル
	assert(billboardImageHandlBase >= 0);

	// 画像の一部分だけをハンドルにのせる
	billboardImageHandl[BILL_MAINTENANCE_KEY] = DerivationGraph(0, 0, 64 / 2, 32, billboardImageHandlBase);
	//DeleteGraph(billboardImageHandlBase);	// 使わなくなった元ハンドルを削除

	
	imageHandle[IMAGE_BULLET_NUM]			= EntityController::GetEntityController()->GetEntityTextureHandle("num_white");
	imageHandle[IMAGE_UPGRADE_COST_NUM]		= EntityController::GetEntityController()->GetEntityTextureHandle("num_white");
	imageHandle[IMAGE_UPGRADE_COST_LETTER]	= EntityController::GetEntityController()->GetEntityTextureHandle("upgrade_cost");
	imageHandle[IMAGE_MOUSE_RIGHT_CLICK]	= EntityController::GetEntityController()->GetEntityTextureHandle("mouse_right_click");
	imageHandle[IMAGE_WARNING]				= EntityController::GetEntityController()->GetEntityTextureHandle("warning");
}

void TurretBase::GunInit()
{
	if (gun == nullptr)
		return;	// Gunが生成されていなかったらreturn

	// GunのInitを実行
	gun->Init(EntityController::GetEntityController()->GetShotManager(), EntityController::GetEntityController()->GetStageManager());
}

TurretBase::~TurretBase()
{
	if (obbColl != nullptr)
	{
		delete obbColl;
		obbColl = nullptr;
	}

	/*if (rayColl != nullptr)
	{
		delete rayColl;
		rayColl = nullptr;
	}*/

	// DuplicateModelハンドルが削除されていなかったら
	if (!isDeleteDuplicateModel)
	{
		// 元モデルハンドルをコピーしたハンドルなので、個別で削除
		MV1DeleteModel(hModel);
	}

	//for (int i = 0;i < BILL_MAX;i++)
	//	DeleteGraph(billboardImageHandl[i]);

	if (gun != nullptr)
	{
		gun->DestroyMe();
		gun = nullptr;
	}

	if (trapUpgradingNumGauge != nullptr)
	{
		delete trapUpgradingNumGauge;
		trapUpgradingNumGauge = nullptr;
	}

	// ShotManagerから自身を削除
	//EntityController::GetEntityController()->DeleteShotOwner(this); 
}

void TurretBase::Update()
{
	EntityBase::Update();

	isUpgrading = false;	// アップグレードフラグのリセット　
	
	// 発射できるとき
	if (canShot)
	{
		// 弾の発射処理
		ShotUpdate();
	}
}

void TurretBase::ShotUpdate()
{
	// 弾切れの時
	if (gun->GetAmmo() <= 0)
		canShot = false;

#if DEFAULT_SHOT
	
	if (!EntityController::GetEntityController()->IsEnemySummon())
		return;	// 敵が存在しなかったら、発射処理を行わない
#endif
	Transform enemyTrans	= EntityController::GetEntityController()->GetNearEnemyTransform(transform.position);
	enemyTrans.position.y	+= (enemyTrans.scale.y * enemyTrans.size.y) / 2;		// 敵の座標が足元なので、体の大きさの半分上に座標を上げる
	
	if (VSquareSize(transform.position - enemyTrans.position) >= shotDistance * shotDistance)
		return;	// 射程範囲外だったら、発射処理を行わない

	if (fabsf(enemyTrans.position.y - transform.position.y) > ((enemyTrans.GetLenY() / 2) + (gun->GetGunData().bulletData.size / 2)))
		return;	// 敵とタレットとで、敵のY軸の半径と発射する弾の大きさを考慮しても、弾が当たらない高さの差があったら、発射処理を行わない

	VECTOR3 velocity		= enemyTrans.position - transform.position;

	//_ 回転 _//

	VECTOR3 vec				= VECTOR3(velocity.x, 0.0f, velocity.z);
	transform.rotation.y	= atan2f(vec.x, vec.z);

	shotRay					= vec;

	VECTOR3 shotDir			= VECTOR3(0, 0, 1) * MGetRotY(transform.rotation.y);					// レイの方向(発射方向)
	VECTOR3 startRayPos		= transform.position + shotDir * (transform.scale.x * transform.size.x);// 自身の半径分スタートレイ座標を前にする　自身がステージオブジェクトなので当たり判定で当たってしまうのを防ぐ

#if DEFAULT_SHOT

	// 発射のインターバルが0の時
	//if ((shotIntervalCount -= Time::GameDeltaTime()) <= 0.0f)
	{
		// 遮蔽物がなかったら
		if (!EntityController::GetEntityController()->CheckRaycastStageObject(startRayPos, enemyTrans.position, COLL_RAY_TARGET_STAGE_OBJECT_KIND))
		{
			// リロード中でなかったら
			if (!gun->IsReloading())
			{
				// 発射 
				gun->Shot(VZero, shotDir);
			}
		}
		//shotIntervalCount	= shotInterval;
	}
	
	
#else
	//__ 発射処理 __//

	constexpr float SHOT_SPEED = 15000.0f;				// 発射スピード 秒
	VECTOR3 shotPos = transform.position;

	EntityController::GetEntityController()->Shot(shotPos, shotRay * shotSpeed, BULLET_KIND::NORMAL, this);
#endif
}

void TurretBase::Draw()
{
	EntityBase::Draw();

	// 発射できないとき
	if (!canShot)
	{
		// メンテナンス警告の描画
		MaintainWarnigDraw();
		// メンテナンスキーの描画
		MaintainKeyDraw();
	}
}

void TurretBase::MaintainWarnigDraw()
{
	float billboardScale		= 300.0f;	// ビルボード描画スケール
	VECTOR3 drawPos				= transform.position + VECTOR3(0.0f, transform.GetLenY() / 2 + 30.0f, 0.0f);	// 描画用座標
	VECTOR3 drawBillBoardPos	= drawPos + VECTOR3(0.0f, 170.0f, 0.0f);	// ビルボード描画座標

	// 弾が発射できないことを警告する画像をビルボード描画
	DrawBillboard3D(drawBillBoardPos, 0.5f, 0.5f, billboardScale, 0.0f, billboardImageHandl[BILL_WARNING], true);
}

void TurretBase::MaintainKeyDraw()
{
	
	VECTOR3 playerPosition		= EntityController::GetEntityController()->GetPlayerTransform().position;	// プレイヤー座標
	const float squareDistance	= VSquareSize(playerPosition - transform.position);							// プレイヤーとの距離の二乗

	if (squareDistance > TrapInfo::MAX_TRAP_INTERACT_DISTANCE * TrapInfo::MAX_TRAP_INTERACT_DISTANCE)
		return;	// 距離が離れていたら return

	float billboardScale		= 100.0f;	// ビルボード描画スケール
	VECTOR3 drawPos				= transform.position + VECTOR3(0.0f, transform.GetLenY() / 2 + 30.0f, 0.0f);	// 描画用座標
	VECTOR3 drawBillBoardPos	= drawPos;	// ビルボード描画座標

	// メンテナンスをするkeyを表す画像をビルボード
	DrawBillboard3D(drawBillBoardPos, 0.5f, 0.5f, billboardScale, 0.0f, billboardImageHandl[BILL_MAINTENANCE_KEY], true);
}

void TurretBase::BulletInfoDraw()
{
	VECTOR3 drawPos = ConvWorldPosToScreenPos(VECTOR3(transform.position.x, transform.position.y + 50.0f, transform.position.z));	// トラップ座標を画面座標に変更した座標

	// カメラの裏側の座標だったら
	if (drawPos.z > 1.0f)
		return;	// 描画をしない

	float playerToTrapSquareDis = VSquareSize(EntityController::GetEntityController()->GetPlayerTransform().position - transform.position);	// 罠からプレイヤーまでの距離の二乗

	// 罠から離れすぎていたら
	if (playerToTrapSquareDis >= (TrapInfo::MAX_TRAP_INFO_DRAW_DISTANCE * TrapInfo::MAX_TRAP_INFO_DRAW_DISTANCE))
		return;	// 描画をしない

	//_ アップグレードコスト数の描画 _//

	VECTOR2I bulletNumSize	= VECTOR2I(794 / 10, 159 / 2);		// 弾数の描画一桁のサイズ
	int interval			= 794 / 17;							// 数字の描画間隔
	int ammoNum				= gun->GetAmmo();					// 残弾数
	int magazineNum			= gun->GetGunData().magazineSize;	// マガジン数
	float drawScale			= 1.0f;								// 描画の大きさ					
 
	// コスト数の描画
	DrawRotaNum(drawPos.x - 50.0f, drawPos.y, ammoNum, interval, imageHandle[IMAGE_BULLET_NUM], bulletNumSize.x, bulletNumSize.y, 0, drawScale, 0.0f, 2);

	// / の描画
	DrawRectRotaGraphF(drawPos.x, drawPos.y, 0, bulletNumSize.y, bulletNumSize.x, bulletNumSize.y, drawScale, 0.0, imageHandle[IMAGE_BULLET_NUM], true);

	// マガジン数
	DrawRotaNum(drawPos.x + 80.0f, drawPos.y, magazineNum, interval, imageHandle[IMAGE_BULLET_NUM], bulletNumSize.x, bulletNumSize.y, 0, drawScale, 0.0f, 2);
}

void TurretBase::CanNotShotDraw()
{
	//VECTOR3 drawPos = ConvWorldPosToScreenPos(VECTOR3(transform.position.x, transform.position.y, transform.position.z));

	//// カメラの表側の座標だったら
	//if (drawPos.z > 1.0f)
	//{
	//	if (drawPos.x < 0 && drawPos.x > Screen::WIDTH &&
	//		drawPos.y < 0 && drawPos.y > Screen::HEIGHT)
	//		return;	// 画面内にタレットがある場合、以下の描画処理をしないでreturn
	//}

	//drawPos.y = Screen::HEIGHT_CENTER;

	const Transform& playerTransform	= EntityController::GetEntityController()->GetPlayerTransform();
	VECTOR2 drawPos						= VECTOR2(0.0f, (float)Screen::HEIGHT_CENTER);

	float viewAngle = CAMERA_INFO::VIEW_ANGLE / 1.3f;	// 視野角　オブジェクトが少しでも、カメラに写ったら、カメラ内と判定するために、値を調整

	// カメラ外のとき
	if (!InCameraTarget(playerTransform, viewAngle,transform.position) == 1)
	{
		//_ 描画座標の設定 _//

		// 右側にタレットがあったら
		if (IsRightTarget(playerTransform, transform.position))
		{
			drawPos.x = Screen::WIDTH - 50.0f;
		}
		// 左側にタレットがあったら
		else
		{
			drawPos.x = 0.0f + 50.0f;
		}

		float trapToPlayerDistance = VSize(playerTransform.position - transform.position);	// トラップとプレイヤーの距離　
		
		if (trapToPlayerDistance < MIN_TRAP_WARNING_DRAW_DISTANCE)
			return;	// プレイヤーとの距離が近すぎる場合は、描画しないで、return

		float drawScale	= MAX_TRAP_WARNING_DRAW_SCALE - (TRAP_WARNING_DRAW_SCALE_RATE * trapToPlayerDistance);	// 距離に応じたスケール	遠ければ小さい

		// 値を収める
		drawScale = std::clamp(drawScale, MIN_TRAP_WARNING_DRAW_SCALE, MAX_TRAP_WARNING_DRAW_SCALE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		// 警告画像の描画
		DrawRectRotaGraphF(drawPos.x, drawPos.y, 1200 / 3, 0, 1200 / 3, 768 / 2, drawScale, 0.0, imageHandle[IMAGE_WARNING], true);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void TurretBase::TrapUpgradInfoDraw()
{
	// アップグレードができる状態でないとき
	if (!CanUpgrade())
		return;	// 描画処理を行わない

	VECTOR3 drawPos	= ConvWorldPosToScreenPos(transform.position);
	//drawPos.x		-= TRAP_UPGRADING_NUM_UI_SIZE.x / 2;		// 左に座標調整

	// カメラの裏側の座標だったら
	if (drawPos.z > 1.0f)
		return;	// 描画をしない

	float costScale				= 0.45f;	// コスト値の描画スケール

	float playerToTrapSquareDis = VSquareSize(EntityController::GetEntityController()->GetPlayerTransform().position - transform.position);	// 罠からプレイヤーまでの距離の二乗

	// 罠から離れすぎていたら
	if (playerToTrapSquareDis >= (TrapInfo::MAX_TRAP_INFO_DRAW_DISTANCE * TrapInfo::MAX_TRAP_INFO_DRAW_DISTANCE))
		return;	// 描画をしない

	// マウスの右クリックを描画
	DrawRotaGraphF(drawPos.x - 170.0f, drawPos.y, 0.05f, 0.0, imageHandle[IMAGE_MOUSE_RIGHT_CLICK], true);

	// UPGRADE_COST の文字の描画
	DrawRotaGraphF(drawPos.x - 10.0f, drawPos.y, costScale, 0.0, imageHandle[IMAGE_UPGRADE_COST_LETTER], true);

	//_ アップグレードコスト数の描画 _//

	VECTOR2I costNumSize	= VECTOR2I(794 / 10, 159 / 2);			// コスト数の描画一桁のサイズ
	int interval			= 794 / 35;								// 数字の描画間隔
	int costNum				= TrapInfo::GetUpgradeCost(kind, 2);	// アップグレードコスト

	// コスト数の描画
	DrawRotaNum(drawPos.x + 180.0f, drawPos.y, costNum, interval, imageHandle[IMAGE_UPGRADE_COST_NUM], costNumSize.x, costNumSize.y, 0, costScale, 0.0f, 2);

	// 罠をアップグレードしている状態でなかったら
	if (!IsUpgrading())
		return;	// ゲージを描画しない

	// アップグレードゲージの描画
	trapUpgradingNumGauge->Draw(VECTOR2(drawPos.x, drawPos.y + 20.0f), TRAP_UPGRADING_NUM_UI_SIZE.x, TRAP_UPGRADING_NUM_UI_SIZE.y, upgradingNum, 0.0f, 1.0f, 0x555555, 0xFFE26A);
}

float TurretBase::Maintaining(const float& _add)
{
	// メンテナンス値を進める
	maintainNum += _add;

	// メンテナンス進行値が最大だったら
	if (maintainNum >= 1.0f)
	{
		if (!canShot)
		{
			// メンテナンスのサウンドを再生
			SoundManager::PlaySE(Sound_ID::TRAP_MAINTENANCE_SE);
		}

		canShot		= true;
		// メンテナンス値を最大に設定
		maintainNum	= 1.0f;
	}
	return maintainNum;
}

float TurretBase::Upgrading(const float& _add)
{
	// すでに最大レベルだったら
	if (trapLevel >= MAX_TRAP_LEVEL)
		return -1.0f;

	isUpgrading		= true;
	upgradingNum	+= _add;

	if (upgradingNum >= 1.0f)
	{
		trapLevel++;
		upgradingNum = 0.0f;

		// レベルアップしたら発射距離を上げる
		shotDistance = TrapInfo::GetViewImpactRadius(kind, trapLevel);

		const std::vector<int>& hModels = EntityController::GetEntityController()->GetModelHandle(kind);	// 自身の全てのモデルコンテナ
		assert(trapLevel <= hModels.size());

		//_ レベルに合わせてモデルハンドルを変更 _//
		
		// 使わなくなったDuplicateModelハンドルを削除
		MV1DeleteModel(hModel);
		// DuplicateModelハンドルを削除したのでフラグをtrue
		isDeleteDuplicateModel	= true;
		// 新ハンドルの代入	level1はindex0へlevel2はindex1へハンドルを入れていく
		hModel					= hModels[trapLevel - 1];

		// 3Dモデルに含まれる1番目(ベース部分)のマテリアルの発光カラーを設定する
		MV1SetMaterialEmiColor(hModel, 1, Color(0.15f, 0.15f, 0.15f, 0.0f).GetColorF4());

		// アップグレードエフェクト再生
		EffekseerObjectManager::SetEffect(transform.position, EffekseerObjectManager::EF_TRAP_UPGRADE);

		// アップグレードのサウンド再生
		SoundManager::PlaySE(Sound_ID::TRAP_UPGRADE_SE);
	}
	return upgradingNum;
}

bool TurretBase::CanUpgrade()
{
	// 罠のレベルが最大かどうかで返す
	return (trapLevel < MAX_TRAP_LEVEL);
}

bool TurretBase::HitEntity(const CollisionHitInfoData& _targetData)
{
	if (_targetData.tag == COLLISION_OBJECT_KIND::ERASER)
	{
		active = false;
		return !obbColl->GetCollisionHitInfoData().onDeleteMe;
	}
	return false;
}

