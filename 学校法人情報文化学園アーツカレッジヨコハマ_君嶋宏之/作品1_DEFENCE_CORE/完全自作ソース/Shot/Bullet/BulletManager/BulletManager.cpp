#include "BulletManager.h"
#include "../Bullet/Bullet.h"
#include "../../../Stage/StageManager/StageManager.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../Common/Sound/SoundID.h"
#include "../../../../ImGui/imgui.h"
#include "../../../../Library/DebugNew.h"

bool BulletManager::isActive = false;

BulletManager::BulletManager()
{
	bullets.clear();
	hitBulletDatas.clear();

	BulletManager::isActive = true;
}

BulletManager::~BulletManager()
{
	BulletManager::isActive = false;

	// コンテナの削除
	DeleteAllBullet();
}

void BulletManager::Init(StageManager* _stageManager)
{
	assert(_stageManager != nullptr);
	stageManager = _stageManager;
}

void BulletManager::Update()
{
	// 当たった弾のデータをリセット
	hitBulletDatas.clear();

	for (auto itr = bullets.begin(); itr != bullets.end();)
	{
		Bullet& bullet = **itr;

		if (*itr != nullptr)
		{
			// 当たっていたら
			if (bullet.IsHit())
			{
				// データを保存
				hitBulletDatas.emplace_back(bullet.GetBulletData());
			}

			if (!bullet.IsActive())
			{
				bullet.DestroyMe();
				itr = bullets.erase(itr);
				continue;	// 削除したので continue
			}
		}
		itr++;
	}

#ifdef _DEBUG

	//ImGui::Begin("Bullet");
	//ImGui::Text("num %d", (int)bullets.size());
	//ImGui::End();
#endif // _DEBUG

}

void BulletManager::Draw()
{
}

bool BulletManager::CreateBullet(const BulletInfo::BulletData& _bulletData, const VECTOR3& _dir)
{
	Bullet* bulletInstance = new Bullet(BulletInfo::BulletData(_bulletData), _dir, stageManager);	// 弾の生成
	
	bullets.emplace_back(bulletInstance);
	
	return true;
}

void BulletManager::DeleteAllBullet()
{
	for (auto itr = bullets.begin(); itr != bullets.end();)
	{
		Bullet* bullet = *itr;

		if (bullet != nullptr)
		{
			bullet->DestroyMe();
			itr = bullets.erase(itr);
			continue;	// 削除したので continue 
		}
		itr++;
	}

	hitBulletDatas.clear();
}