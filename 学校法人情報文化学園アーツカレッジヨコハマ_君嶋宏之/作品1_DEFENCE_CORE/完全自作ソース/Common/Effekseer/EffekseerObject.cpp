#include "EffekseerObject.h"
#include "EffekseerObjectManager.h"
#include <EffekseerForDXLib.h>
#include <assert.h>

namespace 
{
	constexpr float TIMER = 0.5f;	// 0.5秒
}

EffekseerObject::EffekseerObject(const VECTOR3& pPos, const int& _data, const float& _spped, const VECTOR3* pParentPos,const VECTOR3& _changePos) : parentPos(pParentPos)
{
	position		= pPos;
	changePos		= _changePos;
	appearanceTime	= TIMER;
	speed			= _spped;
	hEffekseerData		= _data;
	hPlayingEffekseer	= PlayEffekseer3DEffect(hEffekseerData);
	assert(hPlayingEffekseer >= 0);
}

EffekseerObject::~EffekseerObject()
{
	EffekseerObjectManager::PopEffect(this);
}

void EffekseerObject::Update()
{
	// parentPosが空であれば生成した場所にエフェクトを生成
	if (parentPos == nullptr)
		SetPosPlayingEffekseer3DEffect(hPlayingEffekseer, position.x, position.y, position.z);
	else // parentPosに値が入っていれば、その場所にエフェクトを追従する
		SetPosPlayingEffekseer3DEffect(hPlayingEffekseer, parentPos->x + changePos.x, parentPos->y + changePos.y, parentPos->z + changePos.z);

	// エフェクトの再生速度を設定する
	SetSpeedPlayingEffekseer3DEffect(hPlayingEffekseer, speed);

	// 一定時間たったらエフェクトを破棄する
	if ((appearanceTime -= Time::GameDeltaTime()) <= 0) 
	{
		// エフェクト再生の中断
		StopEffekseer3DEffect(hPlayingEffekseer);
		DestroyMe();
	}
}

void EffekseerObject::StopEffekseer()
{
	// エフェクト再生の中断
	StopEffekseer3DEffect(hPlayingEffekseer);
}
