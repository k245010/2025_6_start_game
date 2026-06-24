#include "stageObjectManager.h"
#include <assert.h>
#include "object.h"
#include "../Library/sceneManager.h"
#include "camera.h"
#include <algorithm>

#include "stageObject.h"
#include "itemObject.h"

#include "point.h"
#include "loadManager.h"
#include "player.h"
#include "walkEnemy.h"
#include "flyEnemy.h"
#include "switchObject.h"
#include "switch.h"
#include "irondoor.h"
#include "goal.h"

#include "gravityArea.h"
#include "breakableBlock.h"

#include "../ImGui/imgui.h"
#include "coin.h"

#include "Screen.h"
#include "bumper.h"
#include "cameraArea.h"
#include "switchMoveObject.h"
#include "potion.h"
#include "gravityBrock.h"
#include "enemyBase.h"

StageObjectManager::StageObjectManager(SceneBase* _scene) :GameObject(_scene)
{
   currentCp = 0;
   checkPointCount = 0;
   isCollisionId = false;

   areaFlg = false;
   coinNum = 0;
 
}

StageObjectManager::~StageObjectManager()
{
	for (Object* ob : objectList) {
		ob->DestroyMe();
		ob = nullptr;
	}
	objectList.clear();

}

void StageObjectManager::Update()
{



	for (auto e = objectList.begin(); e != objectList.end();) {
		if (!(*e)->IsActive()) {
			(*e)->RemoveObject();
			(*e)->RemoveSlowObject();
			(*e)->DestroyMe();
			(*e) = nullptr;
			e = objectList.erase(e);
		}
		else {
			++e;
		}
	}
#if _DEBUG
	ImGui::Begin("stageObjectManagerConfig");
	if (ImGui::Checkbox("areaFlg", &areaFlg)) {
		for (auto e = objectList.begin(); e != objectList.end(); ++e) {
			if ((*e)->GetObjectType() == (int)ObjectType::GRAVITY_AREA || (*e)->GetObjectType() == (int)ObjectType::AIR_BARRIER || (*e)->GetObjectType() == (int)ObjectType::CAMERA_AREA) {
				GetScene()->SetDrawFlg((*e), areaFlg);
			}
		}
	}
	//ImGui::Begin("stageObjectManager");
	ImGui::Text("cpCount:[%d]", currentCp);
	//DrawFormatString(1800, 300, 0xffffff, );


	// ミニマップ画像の描画
	/*SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBoxAA(Screen::WIDTH - MINIMAP.x, Screen::HEIGHT - MINIMAP.y, Screen::WIDTH + MINIMAP.x, Screen::HEIGHT + MINIMAP.y, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);*/
	ImGui::Checkbox("collisionId", &isCollisionId);
	ImGui::Text("objectListSize%d", objectList.size());
	ImGui::End();
#endif // _DEBUG

	

}

void StageObjectManager::Draw()
{

#if _DEBUG

	for (auto& ob : objectList) {
		if (isCollisionId) {

			VECTOR drawPos = ConvWorldPosToScreenPos(ob->GetPosition());
			DrawFormatStringF(drawPos.x, drawPos.y, 0xffffff, "%d", ob->GetCollisionId());
		}


		//if (ob->GetObjectType() == (int)ObjectType::ENEMY_E) {
		//	//VECTOR2F ep=VECTOR2F(MINIMAP.x+(ob->GetPosition().x/4.0f), MINIMAP.y - ob->GetPosition().z * 3.0f);
		//	VECTOR minimapVec = ob->GetPosition();
		//	minimapVec = VSub(minimapVec,);
		//	DrawCircle(pp.x,pp.y, 100.0f, 0xff0000);
		//}


	}

	//DrawFormatStringF(200.0f, 240.0f, 0xffffff, "objectListSize%d", objectList.size());

#endif // _DEBUG


	
}

int StageObjectManager::CreateTerraObject(const VECTOR& _pos, const VECTOR& scale, const VECTOR& rotation, const int& type, const int& collisionId,const std::vector<int>_slot)
{
	for (Object* ob : objectList) {
		float distance = VSize(VSub(ob->GetPosition(), _pos));
		if (distance < 10.0f)
		{
			return -1;

		}
	}

	Object* ob = nullptr;

	switch (type)
	{
	case (int)ObjectType::COIN:
		
		ob = GetScene()->CreateGameObject<Coin>();
		coinNum++;
		break;
	case(int)ObjectType::HEAL_ITEM:
	case(int)ObjectType::GHEAL_ITEM:
		ob = GetScene()->CreateGameObject<PotionItem>();
		break;
	case (int)ObjectType::GRAVITY_AREA:
		ob = GetScene()->CreateGameObject<GravityArea>();
		break;
	case (int)ObjectType::BREAKABLE:
		ob = GetScene()->CreateGameObject<BreakableBlock>();
		break;
	case  (int)ObjectType::POINT:
		ob = GetScene()->CreateGameObject<Point>();
		break;
	case (int)ObjectType::SWITCH:
		ob = GetScene()->CreateGameObject<Switch>();
		break;
	case (int)ObjectType::IRON_DOOR:
		ob = GetScene()->CreateGameObject<IronDoor>();
		break;
	case (int)ObjectType::GOAL:
		ob = GetScene()->CreateGameObject<Goal>();
		break;
	case(int)ObjectType::BUMPER:
		ob = GetScene()->CreateGameObject<Bumper>();
		break;
	case (int)ObjectType::CAMERA_AREA:
		ob = GetScene()->CreateGameObject<CameraArea>();
		break;
	case (int)ObjectType::GRAVITY_A:
		ob = GetScene()->CreateGameObject<GravityBrock>();
		break;
	default:
		ob = GetScene()->CreateGameObject<StageObject>();
		break;

	}
	GetScene()->SetDrawOrder(ob, 90);

	int handle = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<LoadManager>()->GetHandle(std::to_string(type));
	
		switch (type)
		{
		case (int)ObjectType::POINT:
			dynamic_cast<Point*>(ob)->Init(handle, _pos, scale, rotation, type, collisionId, checkPointCount);
			checkPointCount++;
			break;
		case (int)ObjectType::SWITCH:
			dynamic_cast<Switch*>(ob)->Init(handle, _pos, scale, rotation, type, collisionId, _slot[0], this);
			break;
		case (int)ObjectType::IRON_DOOR:
			dynamic_cast<IronDoor*>(ob)->Init(handle, _pos, scale, rotation, type, collisionId, _slot[0]);
			break;
		case(int)ObjectType::GOAL:
			dynamic_cast<Goal*>(ob)->Init(handle, _pos, scale, rotation, type, collisionId);
			break;
		case (int)ObjectType::GRAVITY_AREA:
			dynamic_cast<GravityArea*>(ob)->Init(handle, _pos, scale, rotation, type, collisionId,_slot[0]);
			//GetScene()->SetDrawOrder(ob, 105);
			break;
		case (int)ObjectType::BUMPER:
			dynamic_cast<Bumper*>(ob)->Init(handle, _pos, scale, rotation, type, collisionId);
			break;
		case (int)ObjectType::CAMERA_AREA:
			dynamic_cast<CameraArea*>(ob)->Init(handle, _pos, scale, rotation, type, collisionId,_slot[0],VGet(_slot[1], _slot[2], _slot[3]));
			break;
		default:
			dynamic_cast<StageObject*>(ob)->Init(handle, _pos, scale, rotation, type, collisionId);
			break;
		}

	
	objectList.emplace_back(ob);
	if (not areaFlg) {
		if (type == (int)ObjectType::GRAVITY_AREA || type == (int)ObjectType::AIR_BARRIER||type==(int)ObjectType::CAMERA_AREA) {
			GetScene()->SetDrawFlg(ob, false);
		}
	}
	return 1;
}

int StageObjectManager::CreateCharaObject(const VECTOR& _pos, const VECTOR& scale, const VECTOR& rotation, const int& type, const int& collisionId, const std::vector<int>_slot)
{
	for (Object* ob : objectList) {
		float distance = VSize(VSub(ob->GetPosition(), _pos));
		if (distance < 10.0f)
		{
			return -1;

		}
	}

	Object* ob = nullptr;

	switch (type)
	{
	case (int)ObjectType::PLAYER:
		ob = GetScene()->CreateGameObject<Player>();
		break;
	case (int)ObjectType::WALK_ENEMY:
		ob = GetScene()->CreateGameObject<WalkEnemy>();
		break;
	case (int)ObjectType::FLY_ENEMY:
		ob = GetScene()->CreateGameObject<FlyEnemy>();
		break;

	default:
		break;

	}
	

	int handle = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<LoadManager>()->GetHandle(std::to_string(type));

		if (type == (int)ObjectType::PLAYER) {
			dynamic_cast<Player*>(ob)->PlayerInit(handle, _pos, scale, rotation, type, collisionId);
			GetScene()->SetDrawOrder(ob, 100);
		}
		else {
			dynamic_cast<EnemyBase*>(ob)->EnemyInit(handle, _pos, scale, rotation, type, collisionId,_slot[0]);
			dynamic_cast<EnemyBase*>(ob)->DirectionChange(DX_PI_F / 2, CharaBase::RIGHT);
			GetScene()->SetDrawOrder(ob, 99);
		}
	
	objectList.emplace_back(ob);

	return 1;
}



int StageObjectManager::GetObjectType(const Object* ob) const
{
	for (auto itr = objectList.begin(); itr != objectList.end(); ++itr)
	{
		Object* object = *itr;
		if (object == ob)
		{
			return object->GetObjectType();
		}
	}
	return -1;
}

VECTOR StageObjectManager::GetObjectPosition(const Object* ob)
{
	for (auto itr = objectList.begin(); itr != objectList.end(); ++itr)
	{
		Object* object = *itr;
		if (object == ob)
		{
			return object->GetPosition();
		}
	}
	return VGet(0.0f, 0.0f, 0.0f);
}


void StageObjectManager::DeleteObject()
{
	for (auto itr = objectList.begin(); itr != objectList.end();itr++)
	{
		(*itr)->DestroyMe();
			(*itr) = nullptr;
	}
	objectList.clear();

	
}

void StageObjectManager::DeletePlayerOtherObject()
{
	for (auto itr = objectList.begin(); itr != objectList.end();)
	{
		if ((*itr)->GetObjectType() != (int)ObjectType::PLAYER) {
			(*itr)->RemoveObject();
			(*itr)->RemoveSlowObject();
			(*itr)->DestroyMe();
			(*itr) = nullptr;
			itr= objectList.erase(itr);
		}
		else {
			++itr;
		}
	}
}

void StageObjectManager::DeletePointToCoinToPlayerOtherObject()
{
	for (auto itr = objectList.begin(); itr != objectList.end(); )
	{
		if ((*itr)->GetObjectType() != (int)ObjectType::POINT&& (*itr)->GetObjectType() != (int)ObjectType::PLAYER&& (*itr)->GetObjectType() != (int)ObjectType::COIN) {
			(*itr)->RemoveObject();
			(*itr)->RemoveSlowObject();
			(*itr)->DestroyMe();
			(*itr) = nullptr;
			itr = objectList.erase(itr);
		}
		else {
			++itr;
		}
		
	}
}







ObjectList& StageObjectManager::GetObjectList()
{
	return objectList;
}

void StageObjectManager::SetObjectList(ObjectList& list)
{
	objectList=list;
}

void StageObjectManager::NextCheckPoint()
{
	currentCp++;
}

VECTOR StageObjectManager::GetCurrentCpPosition() const
{
	for (auto& ob : objectList) {
		if (ob->GetObjectType() == (int)ObjectType::POINT) {
			
			if (currentCp == dynamic_cast<Point*>(ob)->GetCheckPointId()) {
				//奥にチェックポイントを置くと奥で復活してしまうためすこしまえにする。
				return VECTOR3(ob->GetPosition().x, ob->GetPosition().y, ob->GetPosition().z - 100.0f);
			}
		}
	}
	return VGet(0.0f, 0.0f, 0.0f);
}

int StageObjectManager::GetSwitchObjectId()
{
	for (auto& ob : objectList) {
		if (ob->GetObjectType() ==(int)ObjectType::IRON_DOOR) {

			return dynamic_cast<SwitchObject*>(ob)->GetSwitchId();
			
		}
	}
	return -1;
}

VECTOR3 StageObjectManager::GetCameraAreaPullAddPosition(int cameraAreaid)
{
	for (auto itr = objectList.begin(); itr != objectList.end(); ++itr) {
		if ((*itr)->GetObjectType() == (int)ObjectType::CAMERA_AREA) {
			if (dynamic_cast<CameraArea*>(*itr)->GetCameraId() == cameraAreaid) {
				return (*itr)->GetPosition();
			}
		}
	}
	return VZero;
}

void StageObjectManager::SetSwitchObjectState(int id, int state)
{
	for (auto itr = objectList.begin(); itr != objectList.end(); ++itr) {
		if ((*itr)->GetObjectType() == (int)ObjectType::IRON_DOOR) {
			if (auto* i = dynamic_cast<SwitchMoveObject*>(*itr)) {
				if (i->GetSwitchId() == id) {
					i->SetSwitchMoveObjectState(state);
				}
			}
		}

	}

}


void StageObjectManager::DeleteObject(Object* ob)
{
    for (auto itr = objectList.begin(); itr != objectList.end(); )
    {
        if (*itr == ob)
        {
            (*itr)->DestroyMe();
			(*itr) = nullptr;
            itr = objectList.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}





