#include "ObjectManager.h"
#include "GameObject.h"
#include "../Source/Common/ShadowMap/ShadowMap.h"
#include <algorithm>
#include <assert.h>

namespace
{
	std::list<GameObject*>* objects;
	size_t deleteObjectCount;
	bool needSortDraw;
	GameObject* running;
};

//-- 自身での実装 -------------------

/// <summary>
/// 削除順にコンテナをソートする
/// </summary>
void SortByDeleteOrder()
{
	objects->sort([](GameObject* a, GameObject* b) {return a->GetDeleteOrder() > b->GetDeleteOrder(); });
}

//----------------------------------

void ObjectManager::Init()
{
	objects				= new std::list<GameObject*>;
	objects->clear();
	deleteObjectCount	= 0;
	needSortDraw		= false;
	running				= nullptr;
}

void ObjectManager::Update()
{
	for (auto itr = objects->begin(); itr != objects->end(); itr++)
	{
		GameObject* obj = *itr;
		if (obj == nullptr)
			continue;
		if (not obj->DestroyRequested())
		{
			running = obj;
			obj->Update();
			running = nullptr;
		}
		if (obj->DestroyRequested())
		{
			delete obj;
			*itr = nullptr;
		}
	}
	for (auto itr = objects->begin(); itr != objects->end();)
	{
		if (*itr == nullptr)
		{
			itr = objects->erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void ObjectManager::Draw()
{
	std::list<GameObject*> shadowMapDrawObjects;

	if (needSortDraw)
	{
		objects->sort([](GameObject* a, GameObject* b) {return a->GetDrawOrder() > b->GetDrawOrder(); });
		needSortDraw = false;
	}

	for (GameObject* obj : *objects)
	{
		if (obj == nullptr || obj->DestroyRequested())
			continue;	

		obj->Draw();

//-- 自身での実装 -----------------------------------------------------------------------------

		if (!obj->IsUseShadowMap())
			continue;	// シャドウマップを使わないので continue

		// シャドウマップを使うGameObjectをpush
		shadowMapDrawObjects.emplace_back(obj);
	}

	//_ シャドウマップの処理 _//

	// シャドウマップのセットアップ
	ShadowMap::GetShadowMapInstance()->DrawSetup();
	for (const auto& object : shadowMapDrawObjects)
	{
		object->Draw();
	}
	// シャドウマップへの描画終了
	ShadowMap::GetShadowMapInstance()->DrawEnd();
	// シャドウマップを使った描画開始
	ShadowMap::GetShadowMapInstance()->UseShadowMapSetting();
	for (const auto& object : shadowMapDrawObjects)
	{
		object->Draw();
	}
	// シャドウマップの使用を解除
	ShadowMap::GetShadowMapInstance()->StopUseShadowMapSetting();

//-------------------------------------------------------------------------------------------
}

void ObjectManager::Release()
{
	SortByDeleteOrder();

	while (objects->size() > 0)
	{
		auto itr = objects->begin();
		if (*itr != nullptr)
		{
			delete *itr;
		}
		objects->erase(itr);
	}

	objects->clear();
	delete objects;
	objects = nullptr;
}

void ObjectManager::Push(GameObject* obj)
{
	objects->push_back(obj);
	needSortDraw = true;
}

void ObjectManager::SortByDrawOrder()
{
	needSortDraw = true;
}

void ObjectManager::Pop(GameObject* obj)
{
	assert(running != obj);

	for (auto itr = objects->begin(); itr != objects->end(); itr++)
	{
		if (*itr == obj)
		{
			*itr = nullptr;
		}
	}
	deleteObjectCount++;
}

size_t& ObjectManager::GetDeleteObjectCount()
{
	return deleteObjectCount;
}

void ObjectManager::DeleteAllGameObject()
{
	assert(running == nullptr);

	SortByDeleteOrder();

	for (auto itr = objects->begin(); itr != objects->end(); itr++)
	{
		GameObject* obj = *itr;
		if (not obj->IsDontDestroy())
		{
			delete obj;
			*itr = nullptr;
		}
	}
	for (auto itr = objects->begin(); itr != objects->end();)
	{
		if (*itr == nullptr) 
		{
			itr = objects->erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

const std::list<GameObject*>& ObjectManager::GetAllObject()
{
	return *objects;
}
