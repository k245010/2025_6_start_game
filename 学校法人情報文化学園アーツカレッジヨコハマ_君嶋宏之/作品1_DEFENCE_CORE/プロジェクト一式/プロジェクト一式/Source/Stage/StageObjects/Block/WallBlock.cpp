#include "WallBlock.h"
#include "../../BlockController/BlockController.h"
#include "../../../Collision/Collisions/SphereCollision.h"
#include "../../../Collision/Collisions/OBBCollision.h"
#include "../../../Collision/Collisions/ModelCollision.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../Common/Sound/SoundID.h"
#include "../../../Gauge/Gauge.h"
#include "../../../Common/Effekseer/EffekseerObjectManager.h"
#include "../../../Common/UI/UIDrawManager/UIDrawManager.h"
#include <assert.h>

namespace
{
	const VECTOR2 HP_UI_SIZE = VECTOR2(30, 6);				// HP描画のゲージサイズ

	const std::set<Sound_ID::SOUND_ID> damageSoundIDList =	// 防御壁のダメージサウンドIDコンテナ
	{
		Sound_ID::WALL_DAMAGE1_SE,
		Sound_ID::WALL_DAMAGE2_SE
	};
}

WallBlock::WallBlock(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : BlockBase(_trans, _modelData, StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK,_hp,_putPlaceKind)
{
	transform.size = VOne;

	obbColl->SetTagMe(COLLISION_OBJECT_KIND::WALL_BLOCK);
	//obbColl->SetTargetTag(COLLISION_OBJECT_KIND::WAY_POINT);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ERASER);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::TURRET_RAY);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::BULLET);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY_ATTACK);

	modelColl	= new ModelCollision(&transform, COLLISION_OBJECT_KIND::WALL_BLOCK, hModel, "WALL_MODEL_COLLISION");
	modelColl->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);

	hpMax		= hp;
	lastHp		= hp;

	VECTOR3 screenPos	= ConvWorldPosToScreenPos(VECTOR3(transform.position.x, transform.position.y + 210, transform.position.z));	// 壁ブロックに対する画面座標
	gauge				= new Gauge(VECTOR2(screenPos.x - HP_UI_SIZE.x / 2, screenPos.y), HP_UI_SIZE.x, HP_UI_SIZE.y, 0.0f, hpMax);
	gauge->SetDrawDelay(false);

	// UIDraw関数を登録
	uiFunctionIDNumber	= UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this]() { UIDraw(); });
	
	COLOR_F materialEmissiveColor = GetColorF(0.3f, 0.3f, 0.3f, 0.5f);	// モデルのマテリアルの発光カラー

	// 3Dモデルに含まれる0番目のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 0, materialEmissiveColor);

	// 底面座標
	VECTOR3 bottomPos = transform.position - VNormY * (transform.GetLenY() / 2);
	// 罠の設置のエフェクト再生
	EffekseerObjectManager::SetEffect(bottomPos, EffekseerObjectManager::EF_TRAP_PUT,1.5f);
}

WallBlock::~WallBlock()
{
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	if (modelColl != nullptr)
	{
		delete modelColl;
		modelColl = nullptr;
	}

	// UIDraw関数を解放
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(uiFunctionIDNumber);
}

void WallBlock::Update()
{
	BlockBase::Update();

	// 1フレーム前のHPと現在HPが違っていたら
	if (lastHp != hp)
	{
		// HPが減っていたらナビゲーションを更新してナビゲーションコストを再計算する
		BlockController::GetBlockController()->NavigationAreaBoxHPChange(&transform, (int)hp);
		lastHp = hp;
	}
}

void WallBlock::Draw()
{
	BlockBase::Draw();
	
	

	/*
	float line		= 0.4f;
	VECTOR2 drawPos = VECTOR2(pos.x, pos.y);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBoxAA(drawPos.x, drawPos.y, drawPos.x + HP_UI_SIZE.x, drawPos.y + HP_UI_SIZE.y, GetColor(100, 100, 100), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawBoxAA(drawPos.x - HP_UI_SIZE.x / 2, drawPos.y, drawPos.x + (HP_UI_SIZE.x / hpMax) * hp, drawPos.y + HP_UI_SIZE.y, GetColor(0, 255, 0), true);
	DrawBoxAA(drawPos.x - HP_UI_SIZE.x / 2 - line, drawPos.y - line, drawPos.x + HP_UI_SIZE.x + line, drawPos.y + HP_UI_SIZE.y + line, GetColor(150, 150, 150), false, (float)line * 1.7f);
	*/
	//DrawSphere3D(transform.position, BLOCK_SIZE / 2, 16, 0x008800, 0xffffff, false);
}

void WallBlock::UIDraw()
{
	VECTOR pos	= ConvWorldPosToScreenPos(VECTOR3(transform.position.x, transform.position.y + 210, transform.position.z));	// 防御壁をワールド座標から、画面に座標に変換した座標
	pos.x		-= HP_UI_SIZE.x / 2;		// 左に座標調整

	// HPがMAXだったら
	if (hp == hpMax)
		return;	// HPを描画しない

	// カメラの裏側の座標だったら
	if (pos.z > 1.0f)
		return;	// HPを描画しない

	// HPゲージの描画
	gauge->Draw(VECTOR2(pos.x, pos.y), HP_UI_SIZE.x, HP_UI_SIZE.y, hp, 0.0f, hpMax, 0x444444, 0xdddddd);
}

bool WallBlock::HitBlock(const CollisionHitInfoData& _targetData)
{
	if (_targetData.tag == COLLISION_OBJECT_KIND::ERASER)
	{
		active = false;
		return !obbColl->GetCollisionHitInfoData().onDeleteMe;
	}
	else if (_targetData.tag == COLLISION_OBJECT_KIND::ENEMY_ATTACK)
	{
		hp -= _targetData.GetDamage(obbColl->GetTagMe());

		if (hp <= 0.0f)
		{
			hp		= 0.0f;
			active	= false;

			// 防御壁が壊れる音を鳴らす
			SoundManager::PlaySE(Sound_ID::WALL_BROKEN_SE);

			// 底面座標
			VECTOR3 bottomPos = transform.position - VNormY * (transform.GetLenY() / 2);
			// 防御壁の崩壊エフェクト再生
			EffekseerObjectManager::SetEffect(bottomPos, EffekseerObjectManager::EF_WALL_BROKEN);

			return !obbColl->GetCollisionHitInfoData().onDeleteMe;
		}

		// 防御壁のダメージ音をランダムな種類で鳴らす
		SoundManager::PlaySERandom(damageSoundIDList);
	}
	return obbColl->GetCollisionHitInfoData().onDeleteMe;
}

