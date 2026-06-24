#include "CharaBase.h"
#include "../Collision/Collisions/SphereCollision.h"
#include "../Common/Sound/SoundManager.h"
#include "../Common/Sound/SoundID.h"

CharaBase::CharaBase()
{
}

CharaBase::CharaBase(const Transform& trans, const int& _hModel) : Object3D(trans,_hModel)
{
}

CharaBase::~CharaBase()
{
	if (sphereColl != nullptr)
	{
		delete sphereColl;
		sphereColl = nullptr;
	}

	if (attackSphereColl != nullptr)
	{
		delete attackSphereColl;
		attackSphereColl = nullptr;
	}
}

void CharaBase::Update()
{
	Object3D::Update();
}

bool CharaBase::HitChara(const CollisionHitInfoData& _targetData)
{
	const float ME_DAMAGE = _targetData.GetDamage(sphereColl->GetTagMe());	// 当たった時の自身が受けるダメージ量

	// 当たったときのダメージ量が設定されていたら
	if (ME_DAMAGE > 0.0f)
	{
		if (hp > 0.0f)
		{
			hp		-= ME_DAMAGE;
		}

		if (hp <= 0.0f)
		{
			active	= false;
			hp		= 0.0f;

			// 死亡サウンドが設定されていた時
			if (deathSoundID > 0)
			{
				// 死亡サウンドを再生
				SoundManager::PlaySE((Sound_ID::SOUND_ID)deathSoundID);
			}
			return true;
		}
		// ダメージサウンドが設定されていた時
		if (damageSoundID > 0)
		{
			// ダメージサウンドを再生
			SoundManager::PlaySE((Sound_ID::SOUND_ID)damageSoundID);
		}
	}
	return false;
}
