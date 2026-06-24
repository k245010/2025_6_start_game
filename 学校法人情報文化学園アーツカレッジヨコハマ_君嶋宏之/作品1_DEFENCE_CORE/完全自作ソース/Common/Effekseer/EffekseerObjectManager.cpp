#include "EffekseerObjectManager.h"
#include <EffekseerForDXLib.h>
#include <unordered_map>
#include <assert.h>
#include "../../../Library/DebugNew.h"


namespace
{
	std::unordered_multimap<int, EffekseerObject*> effekseerList;											// EffekseerObjectの管理をするコンテナ
	std::unordered_map<int, int> effekseerHandleList;														// Effekseerのハンドルコンテナ
	std::unordered_map<std::string, EffekseerObjectManager::EFFEKSEER_KIND> effekseerStringFromEnumList;	// EFFEKSEER_KINDの文字列に応じた、EFFEKSEER_KINDのenumを管理するコンテナ
}

void EffekseerObjectManager::Init()
{
	effekseerHandleList =
	{
		{ EF_ENEMY_SPAWN,		LoadEffekseerEffect("data\\effekseer\\enemySpawn.efkefc",		60.0f)},
		{ EF_ENEMY_HIT,			LoadEffekseerEffect("data\\effekseer\\hitEffect.efkefc",		25.0f)},
		{ EF_ENEMY_DEATH,		LoadEffekseerEffect("data\\effekseer\\enemyDeath.efkefc",		25.0f)},
		{ EF_PLAYER_HIT,		LoadEffekseerEffect("data\\effekseer\\hitEffectPlayer.efkefc",	50.0f)},
		{ EF_TRAP_PUT,			LoadEffekseerEffect("data\\effekseer\\trapPut.efkefc",			20.0f)},
		{ EF_TRAP_UPGRADE,		LoadEffekseerEffect("data\\effekseer\\trapUpgrade.efkefc",		25.0f)},
		{ EF_CORE_HIT,			LoadEffekseerEffect("data\\effekseer\\hitEffectCore.efkefc",	40.0f)},
		{ EF_WALL_BROKEN,		LoadEffekseerEffect("data\\effekseer\\wallBroken.efkefc",		35.0f)},
		{ EF_SLIME_SLOW_DOWN,	LoadEffekseerEffect("data\\effekseer\\slimeSlowDown.efkefc",	20.0f)},
		{ EF_EXPLOSION,			LoadEffekseerEffect("data\\effekseer\\explosion.efkefc",		20.0f)},
	};

	for (auto handleItr = effekseerHandleList.begin(); handleItr != effekseerHandleList.end(); handleItr++)
	{
		assert(handleItr->second >= 0);
	}

	for (const auto& effekseerKindStr : EffekseerObjectManager::EFFEKSEER_KIND_STRING_LIST)
	{
		// 文字列とenumをpush
		effekseerStringFromEnumList.emplace(effekseerKindStr.second, effekseerKindStr.first);
	}
}

void EffekseerObjectManager::Release()
{
	DeleteAllEffect();

	for (auto handleItr = effekseerHandleList.begin(); handleItr != effekseerHandleList.end(); handleItr++)
	{
		DeleteEffekseerEffect(handleItr->second);
	}
	effekseerHandleList.clear();
	effekseerStringFromEnumList.clear();
}

void EffekseerObjectManager::PopEffect(EffekseerObject* _effekseerObj)
{
	for (auto effekseerItr = effekseerList.begin(); effekseerItr != effekseerList.end();effekseerItr++)
	{
		if (effekseerItr->second == _effekseerObj)
		{
			effekseerItr->second	= nullptr;
			effekseerItr			= effekseerList.erase(effekseerItr);

			break;	// コンテナからの削除候補が見つかったので
		}
	}
}

void EffekseerObjectManager::DeleteAllEffect()
{
	for (auto effekseerItr = effekseerList.begin();effekseerItr != effekseerList.end();)
	{
		if (effekseerItr->second != nullptr)
		{
			// エフェクトの再生を中断
			effekseerItr->second->StopEffekseer();
			effekseerItr->second->DestroyMe();
			effekseerItr->second = nullptr;

			effekseerItr = effekseerList.erase(effekseerItr);
		}
		else
		{
			effekseerItr++;
		}
	}
}

void EffekseerObjectManager::Update()
{
	for (auto effekseerItr = effekseerList.begin();effekseerItr != effekseerList.end();)
	{
		if (effekseerItr->second == nullptr || effekseerItr->second->DestroyRequested())
		{
			effekseerItr->second	= nullptr;
			effekseerItr			= effekseerList.erase(effekseerItr);
		}
		else
		{
			effekseerItr++;
		}
	}
}

void EffekseerObjectManager::Draw()
{
	//追従させるために、エフェクトの描画の直前で更新処理を走らせる。
	Effekseer_Sync3DSetting();
	UpdateEffekseer3D();

	//エフェクトを描画する
	DrawEffekseer3D();
}

void EffekseerObjectManager::SetEffect(const VECTOR3& _pPos, const int& _id, const float& _spped, const VECTOR3* _parent, const VECTOR3& _change)
{
	if (effekseerHandleList.find(_id) == effekseerHandleList.end())
		assert(false);

	effekseerList.insert(std::make_pair(_id, new EffekseerObject(_pPos, effekseerHandleList[_id], _spped, _parent, _change)));
}

EffekseerObjectManager::EFFEKSEER_KIND EffekseerObjectManager::GetFromEnum(std::string_view _kindStr)
{
	if (effekseerStringFromEnumList.find(std::string(_kindStr)) == effekseerStringFromEnumList.end())
		assert(false && "不正なEFFEKSEER_KINDの文字列です");

	return effekseerStringFromEnumList.at(std::string(_kindStr));
}