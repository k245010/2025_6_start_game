#include "SlimePatch.h"
#include "../../../Screen.h"
#include "../../../Collision/Collisions/OBBCollision.h"
#include "../../StageObjectData.h"

namespace
{
	constexpr float MAX_LIFE_TIME		= 10.0f;	// 最大生存時間(秒)
	constexpr float BIGGER_TIME			= 1.5f;		// 拡大しきるまでの時間(秒)
	constexpr float BIGGER_RADIUS_SIZE	= 200.0f;	// 最大半径
	constexpr float MAX_INVINCIBLE_TIME = 1.0f;		// 最大無敵時間(秒)
	constexpr float DECAY_HP_ONE_SECOND	= 1.0f;		// １秒間に自然にHPが減少する量

	//constexpr float SMALLER_RADIUS_SIZE	= 50.0f;	// 最大半径
}

SlimePatch::SlimePatch(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : EntityBase(_trans, _modelData, StageObjectData::STAGE_OBJECT_KIND::SLIME_PATCH, _hp,_putPlaceKind)
{
	//_ スライムモデル用初期化 _//
	
	transform.size			= VOne * 8.36f;
	transform.size.y		= 16.72f;
	transform.scale			= VOne * (StageInfo::BLOCK_SIZE / 8.36f);
	transform.scale.y		= 10.0f;

	float sizeX				= transform.size.x * transform.scale.x;		// 初期X軸の大きさ

	// 正方形でなかったら
	if (sizeX != transform.size.z * transform.scale.z)
		assert(false && "スライムパッチのX、Zのサイズ比率が正しくありません");

	// サイズが最終的なサイズよりも大きかったら
	if (sizeX / 2 > BIGGER_RADIUS_SIZE)
		assert(false && "サイズが大きすぎます");

	float firstRadius	= sizeX / 2;								// 初期の半径
	float sumAddRadius	= (BIGGER_RADIUS_SIZE - firstRadius) * 2;	// 足される値合計
	addScale			= sumAddRadius / transform.size.x;			// 加算するスケール値
	biggerScaleRate		= addScale / BIGGER_TIME;					// 1秒間の拡大率を計算

	initScaleY			= transform.scale.y;

	obbColl->SetTagMe(COLLISION_OBJECT_KIND::SLIME_PATCH);			// 当たり判定で自身を設定
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY);			// 当たり判定で相手の設定

	// モデルは半透明にしておく
	MV1SetOpacityRate(hModel, 0.5f);

	SetDrawOrder(-99);
}

SlimePatch::~SlimePatch()
{
}

void SlimePatch::Update()
{
	EntityBase::Update();

	if (hp <= 0.0f)
	{
		active = false;
		return;
	}

	switch (scaleState)
	{
	case SlimePatch::SCALE_STATE::SCALE_UP:

		if (!ModelScaleUpUpdate())
			isInvincible = false;
		return;
		break;
	case SlimePatch::SCALE_STATE::SCALE_DOWN:
		
		// HPが0以下になったら
		if (!ModelScaleDownUpdate())
		{
			active = false;
			return;
		}
		break;
	case SlimePatch::SCALE_STATE::STAY:
		break;
	default:
		assert(false);
		break;
	}

	if (isInvincible)
	{
		if ((invincibleCount += Time::GameDeltaTime()) > MAX_INVINCIBLE_TIME)
		{
			isInvincible	= false;
			invincibleCount = 0.0f;
			scaleState		= SCALE_STATE::STAY;
		}
	}
	else
	{
		if (isHit)
		{
			isInvincible	= true;
			scaleState		= SCALE_STATE::SCALE_DOWN;
		}
	}
}

void SlimePatch::Draw()
{
	// 影を作らないようにライトの影響をOFFにする
	SetUseLighting(false);
	EntityBase::Draw();
	SetUseLighting(true);
}

bool SlimePatch::ModelScaleUpUpdate()
{
	summonTime += Time::GameDeltaTime();

	// 拡大しきっていたら
	if (summonTime > BIGGER_TIME)
	{
		// 縮小状態へ遷移
		scaleState = SCALE_STATE::SCALE_DOWN;
		return false;
	}

	//_ モデルの拡大 _//

	transform.scale.x += biggerScaleRate * Time::GameDeltaTime();
	transform.scale.z += biggerScaleRate * Time::GameDeltaTime();

	return true;
}

bool SlimePatch::ModelScaleDownUpdate()
{
	//_ subHpSum値から現在のHPを減少 _//

	if (invincibleCount <= 0.0f)
	{
		// 1フレームに減るHP量を計算
		subHpFrame	= (subHpSum / MAX_INVINCIBLE_TIME) * Time::GameDeltaTime();
		subHpSum	= 0.0f;
	}
	hp -= subHpFrame;

	//_ 召喚から徐々にHPを自然減少 _//

	//hp -= DECAY_HP_ONE_SECOND * Time::GameDeltaTime();

	//_ モデルの縮小 _//

	float percentScale	= initScaleY / 100;													// １%に対する大きさ
	float nowPercent	= (hp / (float)StageObjectData::GetStageObjectHPConfig(kind)) * 100;	// HPの値に対する大きさの%

	// マイナスの%は0にする //
	if (nowPercent <= 0.0f)
		nowPercent = 0.0f;

	transform.scale.y	= percentScale * nowPercent;

	if (hp <= 0.0f)
		return false;
	return true;
}

bool SlimePatch::HitEntity(const CollisionHitInfoData& _targetData)
{
	if (isInvincible)
		return obbColl->GetCollisionHitInfoData().onDeleteMe;

	/*if (_targetData.tag == COLLISION_OBJECT_KIND::ERASER)
	{
		active = false;
		return !obbColl->GetCollisionHitInfoData().onDeleteMe;
	}*/

	if (_targetData.tag == COLLISION_OBJECT_KIND::ENEMY)
	{
		subHpSum	+= _targetData.GetDamage(obbColl->GetTagMe());
		isHit		= true;

		if (hp <= 0.0f)
		{
			hp		= 0.0f;
			active	= false;
			return !obbColl->GetCollisionHitInfoData().onDeleteMe;
		}
	}
	return obbColl->GetCollisionHitInfoData().onDeleteMe;
}
