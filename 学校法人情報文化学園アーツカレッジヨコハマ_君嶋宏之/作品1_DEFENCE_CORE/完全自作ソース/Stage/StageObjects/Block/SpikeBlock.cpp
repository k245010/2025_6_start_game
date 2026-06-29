#include "SpikeBlock.h"
#include "../../../Collision/Collisions/OBBCollision.h"
#include "../../../Gauge/Gauge.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../Common/Sound/SoundID.h"
#include "../../../Common/UI/UIDrawManager/UIDrawManager.h"
#include "../../../Common/Effekseer/EffekseerObjectManager.h"

namespace
{
	const VECTOR2 HP_UI_SIZE			= VECTOR2(30, 6);	// HP描画のゲージサイズ
	constexpr float MAX_INVINCIBLE_TIME = 1.0f;				// 無敵の最大時間	
}

SpikeBlock::SpikeBlock(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : BlockBase(_trans, _modelData, StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK,_hp, _putPlaceKind)
{
	obbColl->SetTagMe(COLLISION_OBJECT_KIND::SPIKE_BLOCK);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY, 5.0f);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER, 5.0f);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ERASER);

	// ToDo:モデルの修正をして、ここでスケール変換をやめる
	// モデルのスケールがY軸に高いので、半分にする
	transform.scale.y /= 2.0f;
	transform.position.y -= (transform.size.y * transform.scale.y) / 2;

	hpMax = hp;

	VECTOR screenPos	= ConvWorldPosToScreenPos(VECTOR3(transform.position.x, transform.position.y + 210, transform.position.z));	// トゲブロックに対する画面座標
	gauge				= new Gauge(VECTOR2(screenPos.x - HP_UI_SIZE.x / 2, screenPos.y), HP_UI_SIZE.x, HP_UI_SIZE.y, 0.0f, hpMax);
	gauge->SetDrawDelay(false);

	// UIDraw関数を登録
	uiFunctionIDNumber	= UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this]() { UIDraw(); });

	// 底面座標
	VECTOR3 bottomPos	= transform.position - VNormY * (transform.GetLenY() / 2);

	// 罠の設置のエフェクト再生
	EffekseerObjectManager::SetEffect(bottomPos, EffekseerObjectManager::EF_TRAP_PUT, 1.5f);
}

SpikeBlock::~SpikeBlock()
{
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	// UIDraw関数を解放
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(uiFunctionIDNumber);
}

void SpikeBlock::Update()
{
	BlockBase::Update();

	// 無敵時間が0以下のとき
	if ((invincibleCount -= Time::GameDeltaTime()) <= 0.0f)
	{
		// 無敵状態をオフ //
		invincibleCount = 0.0f;
		isInvincible	= false;
	}
}

void SpikeBlock::Draw()
{
	BlockBase::Draw();
}

void SpikeBlock::UIDraw()
{
	VECTOR pos = ConvWorldPosToScreenPos(VECTOR3(transform.position.x, transform.position.y + 210, transform.position.z));	// 防御壁をワールド座標から、画面に座標に変換した座標
	pos.x -= HP_UI_SIZE.x / 2;		// 左に座標調整

	// HPがMAXだったら
	if (hp == hpMax)
		return;	// HPを描画しない

	// カメラの裏側の座標だったら
	if (pos.z > 1.0f)
		return;	// HPを描画しない

	// HPゲージの描画
	gauge->Draw(VECTOR2(pos.x, pos.y), HP_UI_SIZE.x, HP_UI_SIZE.y, hp, 0.0f, hpMax, 0x444444, 0xdddddd);
}

bool SpikeBlock::HitBlock(const CollisionHitInfoData& _targetData)
{
	if (_targetData.tag == COLLISION_OBJECT_KIND::ERASER)
	{
		active = false;
		return !obbColl->GetCollisionHitInfoData().onDeleteMe;
	}
	else if (_targetData.tag == COLLISION_OBJECT_KIND::ENEMY)
	{
		if (isInvincible)
			return obbColl->GetCollisionHitInfoData().onDeleteMe;	// 無敵のときは、ダメージ処理を飛ばし return

		// 無敵状態に設定 //
		invincibleCount = MAX_INVINCIBLE_TIME;
		isInvincible	= true;

		hp -= _targetData.GetDamage(obbColl->GetTagMe());

		if (hp <= 0.0f)
		{
			hp		= 0.0f;
			active	= false;

			// トゲブロックが壊れる音を鳴らす
			SoundManager::PlaySE(Sound_ID::SPIKE_BROKEN_SE);

			// 底面座標
			VECTOR3 bottomPos = transform.position - VNormY * (transform.GetLenY() / 2);
			// 崩壊エフェクト再生
			EffekseerObjectManager::SetEffect(bottomPos, EffekseerObjectManager::EF_WALL_BROKEN);

			return !obbColl->GetCollisionHitInfoData().onDeleteMe;
		}
	}
	return obbColl->GetCollisionHitInfoData().onDeleteMe;
}
