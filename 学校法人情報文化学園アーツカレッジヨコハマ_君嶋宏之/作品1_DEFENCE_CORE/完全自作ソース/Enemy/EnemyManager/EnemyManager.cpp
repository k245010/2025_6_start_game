#include "EnemyManager.h"
#include "../Enemies/Walker.h"
#include "../Enemies/Climber.h"
#include "../Enemies/Rusher.h"
#include "../Enemies/Crusher.h"
#include "../../Player/Player.h"
#include "../../Stage/StageManager/StageManager.h"
#include "../../Navigation/NavigationManager/NavigationManager.h"
#include "../../Collision/CollisionObjectKind.h"
#include "../EnemyInfo.h"
#include "../EnemyDataSerializer/EnemyDataSerializer.h"
#include "../../Common/Debug/Debug.h"
#include "../../../ImGui/imgui.h"
#include "../../../Library/DebugNew.h"
#include <string>

int EnemyManager::enemyNumber		= 0;
bool EnemyManager::isActiveInstance = true;

namespace
{
	constexpr float IMGUI_SLIDER_WIDTH = 150.0f;	// ImGui::SliderやImGui::Dragの表示幅
}

EnemyManager::EnemyManager()
{
	enemies.clear();
	nextSummonEnemyInfoList.clear();
	nextSummonEnemyRootNumberList.clear();
	
	std::string str = "data/models/chara/enemies/";

	hModels[(int)ENEMY_KIND::WALKER]	= MV1LoadModel((str + "Walker/Walker.mv1").c_str());
	hModels[(int)ENEMY_KIND::CLIMBER]	= MV1LoadModel((str + "Climber/Climber.mv1").c_str());
	hModels[(int)ENEMY_KIND::RUSHER]	= MV1LoadModel((str + "Rusher/Rusher.mv1").c_str());
	hModels[(int)ENEMY_KIND::CRUSHER]	= MV1LoadModel((str + "Crusher/Crusher.mv1").c_str());

	for (int i = 0;i < (int)ENEMY_KIND::MAX;i++)
	{
		EnemyInfo::EnemyData enemyData = std::move(EnemyDataSerializer::GetEnemyData((ENEMY_KIND)i));

		// 敵の種類分EnemyDataを取得しpush //
		enemyDataSetList.emplace_back(enemyData);
		enemyDataAttachList.emplace_back(enemyData);
		enemyDataRawList.emplace_back(enemyData);
	}

	for (const auto& hModel : hModels)
		assert(hModel > 0);

	EnemyManager::isActiveInstance = true;
}

EnemyManager::~EnemyManager()
{
	for (auto itr = enemies.begin();itr != enemies.end();)
	{
		EnemyBase* obj = itr->second;

		if (obj != nullptr)
		{
			obj->DestroyMe();
			obj = nullptr;

			itr = enemies.erase(itr);
		}
		else
		{
			itr++;
		}
	}
	enemies.clear();
	nextSummonEnemyInfoList.clear();
	nextSummonEnemyRootNumberList.clear();
	enemyDataSetList.clear();
	enemyDataRawList.clear();
	
	for (auto& hModel : hModels)
	{
		MV1DeleteModel(hModel);
	}

	// 敵データを解放
	EnemyDataSerializer::Release();

	EnemyManager::isActiveInstance = false;
}

void EnemyManager::Init()
{
	//CreateEnemy(ENEMY_KIND::WALKER,VZero);

	player				= FindGameObject<Player>();
	navigationManager	= FindGameObject<NavigationManager>();
	stageManager		= FindGameObject<StageManager>();

	if (player == nullptr || navigationManager == nullptr || stageManager == nullptr)
		assert(false);

	/*for (int i = 0;i < 100;i++)
	{
		CreateEnemy(VECTOR3(1000,0,0), ENEMY_KIND::WALKER);
	}*/

	if (enemies.size() > 0)
	{
		for (auto itr = enemies.begin();itr != enemies.end();itr++)
		{
			EnemyBase& enemy = *itr->second;

			// 敵の初期化
			enemy.Init(this);
		}
	}
}

const VECTOR3* EnemyManager::GetNavPointPosition(const int& _num)
{
	if (navigationManager == nullptr)
		assert(false);
	//return navigationManager->GetWayPointPosition(_num);
	return navigationManager->GetNavPointPosition(_num);
}

const NODE_USE_KIND* EnemyManager::GetNavPointUseKind(const int& _num)
{
	if (navigationManager == nullptr)
		assert(false);

	return navigationManager->GetNavPointUseKind(_num);
}

const NODE_BLOCK_STATE* EnemyManager::GetNavPointBlockState(const int& _num)
{
	if (navigationManager == nullptr)
		assert(false);

	return navigationManager->GetNavPointBlockState(_num);
}

std::vector<int> EnemyManager::GetTargetMoveNavPointContainer(const VECTOR3& _mePos, const VECTOR3& _goalPos, const int& _agentKind, const float& _moveSpeed, const int& _nodeKind, const int& _mode, const float& _dps)
{
	if (navigationManager == nullptr)
		assert(false);
	//return navigationManager->GetTargetMoveWayPointContainer(_mePos, _goalPos);
	return navigationManager->GetTargetMoveNavPointContainer(_mePos, _goalPos, _agentKind, _moveSpeed, _nodeKind,_mode, _dps);
}

const Transform& EnemyManager::GetPlayerTransform()
{
	return player->GetTransform();
}

const VECTOR3* EnemyManager::GetPlayerPositionPtr() const
{
	return player->GetPositionPtr();
}

//const VECTOR3* EnemyManager::GetNearBlockedNavPointPtr() const
//{
//	return navigationManager-;
//}

const VECTOR3* EnemyManager::GetCorePositionPtr() const
{
	return stageManager->GetCorePosition();
}

//bool EnemyManager::StageObjectTypeRayCollision(const VECTOR3& _pos1, const VECTOR3& _pos2, const StageObjectData::TYPE& _type, VECTOR3* _hitP)
//{
//	return stageManager->StageObjectTypeRayCollision(_pos1, _pos2, _type, _hitP);
//}

bool EnemyManager::CheckRaycastStageObject(const VECTOR3& _pos1, const VECTOR3& _pos2, const std::set<int>& _kindList, VECTOR3* _hitP)
{
	return stageManager->CheckRaycastStageObject(_pos1, _pos2, _kindList, _hitP);
}

void EnemyManager::DropCoin(const VECTOR3& _pos)
{
	stageManager->PutStageObject(_pos, StageObjectData::STAGE_OBJECT_KIND::COIN);
}

void EnemyManager::Update()
{
	
#if 0
	//__ 敵が死亡していたら(非アクティブ)だったらコンテナから削除する __//

	for (auto itr = enemies.begin();itr != enemies.end();)
	{
		EnemyBase* obj = itr->second;

		if(!obj->IsActive())
		{
			obj->DestroyMe();
			obj = nullptr;

			itr = enemies.erase(itr);
		}
		else
		{
			obj->SetNavAlways(navAlways);
			itr++;
		}
	}
#endif

	bool copyIsDrawEnemyRoot = isDrawEnemyRoot;

#if _DEBUG
	
	if (CheckHitKey(KEY_INPUT_7))
	{
		// すべての敵を削除するために、DestroyMe関数を呼ぶ

		for (auto& enemy : enemies)
		{
			EnemyBase* obj = enemy.second;

			if (obj != nullptr)
				obj->DestroyMe();
		}
	}
	
#endif

	// デバッグモードがオンの時
	if (Debug::GetDebugMode())
	{
		// 敵データの変更を行う
		ChangeEnemyDataUpdate();
	}

	// デバッグモードがオンの時
	if (Debug::GetDebugMode())
	{
		ImGui::Begin("NavRootDraw");

		ImGui::Checkbox("RootDraw", &isDrawEnemyRoot);

		/*if (ImGui::Button("RootDraw"))
		{
			isDrawEnemyRoot = !isDrawEnemyRoot;
		}*/
		ImGui::End();
	}

	// ルートを描画するフラグが変更されていたら
	if (copyIsDrawEnemyRoot != isDrawEnemyRoot)
	{
		for (const auto& ene : enemies)
		{
			ene.second->SetIsDrawRoot(isDrawEnemyRoot);
		}
	}
}

void EnemyManager::ChangeEnemyDataUpdate()
{
	ImGui::Begin("EnemyData");

	// ImGui::Slider幅を設定する
	ImGui::PushItemWidth(IMGUI_SLIDER_WIDTH);

	ImGui::SliderInt("EnemyKind", &setEnemyKind, 0, (int)ENEMY_KIND::MAX - 1);

	ImGui::SameLine();

	std::string enemyKindStr = ":";

	switch ((ENEMY_KIND)setEnemyKind)
	{
	case ENEMY_KIND::WALKER:	enemyKindStr += "WALKER";	break;
	case ENEMY_KIND::CLIMBER:	enemyKindStr += "CLIMBER";	break;
	case ENEMY_KIND::RUSHER:	enemyKindStr += "RUSHER";	break;
	case ENEMY_KIND::CRUSHER:	enemyKindStr += "CRUSHER";	break;
	default:	assert(false);	break;
	}

	ImGui::Text(enemyKindStr.c_str());

	if (ImGui::Button("Attach"))
	{
		enemyDataAttachList = enemyDataSetList;

		for (auto& enemy : enemies)
		{
			// 敵の敵データを再設定
			enemy.second->SetEnemyData(enemyDataAttachList.at((int)enemy.second->GetEnemyKind()));
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Reset"))
	{
		enemyDataSetList = enemyDataRawList;
	}

	if (ImGui::Button("Save"))
	{
		// 敵の敵データを保存
		EnemyDataSerializer::Save((ENEMY_KIND)setEnemyKind, enemyDataSetList.at(setEnemyKind));
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		// 敵の敵データを取得　
		enemyDataRawList[setEnemyKind] = EnemyDataSerializer::GetEnemyData((ENEMY_KIND)setEnemyKind);
	}

	// ラインを引く
	ImGui::Separator();

	EnemyInfo::EnemyData& enemyData = enemyDataSetList[setEnemyKind];

	if (ImGui::CollapsingHeader("HPData"))
	{
		int hpMax		= (int)enemyData.hpMax;
		ImGui::DragInt("HPMax", &hpMax, 1.0f, 1, 1000);
		enemyData.hpMax = (float)hpMax;
	}

	if (ImGui::CollapsingHeader("MoveData"))
	{
		ImGui::DragFloat("WalkSpeed", &enemyData.moveData.walkSpeed,1.0f, 0.1f, 500.0f);

		// 壁を登れる敵のとき
		if (enemyData.navigationData.nodeUseKind == (int)NODE_USE_KIND::WALL || enemyData.navigationData.nodeUseKind == (int)NODE_USE_KIND::ALL)
			ImGui::DragFloat("ClimbSpeed", &enemyData.moveData.climbSpeed, 1.0f, 0.1f, 500.0f);
	}

	if (ImGui::CollapsingHeader("ModelSizeData"))
	{
		ImGui::DragFloat3("ModelSize", &enemyData.modelSizeData.size.x, 0.25f, 0.1f, 10000.0f);
		ImGui::DragFloat3("ModelScale", &enemyData.modelSizeData.scale.x, 0.25f, 0.01f, 1000.0f);
	}

	if (ImGui::CollapsingHeader("AttackData"))
	{
		ImGui::DragFloat("AttackInterval", &enemyData.attackData.attackInterval, 1.0f, 0.0f, 10.0f);
		ImGui::DragFloat("AttackReach", &enemyData.attackData.attackReach, 1.0f, 0.0f, 500.0f);
	}

	if (ImGui::CollapsingHeader("CollisionData"))
	{
		ImGui::SliderInt("CollisionDataMember", &setSelectCollisionDataMember, 0, 1);

		//ImGui::SameLine();

		if (setSelectCollisionDataMember == 0)
		{
			ImGui::Text(">> BodyCollisionTarget");

			// 体の当たり判定データの改変
			ChangeEnemyCollisionDataList(enemyData.collisionData.bodyCollTargetKindList);
		}
		else if(setSelectCollisionDataMember == 1)
		{
			ImGui::Text(">> AttackCollisionTarget");

			// 攻撃の当たり判定データの改変
			ChangeEnemyCollisionDataList(enemyData.collisionData.attackCollTargetKindList);
		}
		else
		{
			assert(false);
		}
	}

	if (ImGui::CollapsingHeader("NavigationData"))
	{
		//_ 経路探索して、たどり着く相手の種類の設定 _//

		int navTargetKind = (int)enemyData.navigationData.navigationTargetKind;	// int型の 経路探索して、たどり着く相手の種類
		ImGui::SliderInt("NavigationTargetKind", &navTargetKind, 0, (int)EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND::MAX - 1);
		enemyData.navigationData.navigationTargetKind = (EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND)navTargetKind;
		
		ImGui::SameLine();
		
		std::string selectNavTargetKindStr = ":";	//  経路探索して、たどり着く相手の種類の文字列
		switch ((EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND)navTargetKind)
		{
		case EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND::CORE:		selectNavTargetKindStr += "CORE";	break;
		case EnemyNavigationInfo::NavigationData::NAVIGATION_TARGET_KIND::PLAYER:	selectNavTargetKindStr += "PLAYER"; break;
		default:	assert(false);	break;
		}
		ImGui::Text(selectNavTargetKindStr.c_str());

		//_ 経路探索に使用するNavPointの種類の設定 _//

		ImGui::SliderInt("NavigationNodeUseKind", &enemyData.navigationData.nodeUseKind, (int)NODE_USE_KIND::NORMAL, (int)NODE_USE_KIND::ALL);
		
		ImGui::SameLine();

		std::string selectNodeUseKindStr = ":";	//  経路探索に使用するNavPointの種類
		switch ((NODE_USE_KIND)enemyData.navigationData.nodeUseKind)
		{
		case NODE_USE_KIND::NORMAL:		selectNodeUseKindStr += "NORMAL(OnGroundWayPoint)";	break;
		case NODE_USE_KIND::WALL:		selectNodeUseKindStr += "WALL";						break;
		case NODE_USE_KIND::ALL:		selectNodeUseKindStr += "ALL";						break;
		default:	assert(false);	break;
		}
		ImGui::Text(selectNodeUseKindStr.c_str());

		//_ 経路探索するモードの設定 _//

		ImGui::SliderInt("NavigationMode", &enemyData.navigationData.navigationMode, (int)NAVIGATION_MODE::ACTIVE, (int)NAVIGATION_MODE::MAX - 1);

		ImGui::SameLine();

		std::string selectNavModeStr = ":";	//  経路探索するモード
		switch ((NAVIGATION_MODE)enemyData.navigationData.navigationMode)
		{
		case NAVIGATION_MODE::ACTIVE:		selectNavModeStr += "ACTIVE";		break;
		case NAVIGATION_MODE::IN_BLOCKED:	selectNavModeStr += "IN_BLOCKED";	break;
		default:	assert(false);	break;
		}
		ImGui::Text(selectNavModeStr.c_str());
	}

	if (ImGui::CollapsingHeader("DropCoinData"))
	{
		ImGui::SliderInt("DropCoinNum", &enemyData.dropCoinData.dropNum, 0, 1000);
		ImGui::SliderInt("DropProbability", &enemyData.dropCoinData.probability, 1, 100);
	}

	// ImGui::Slider幅設定を解除
	ImGui::PopItemWidth();

	ImGui::End();
}

void EnemyManager::ChangeEnemyCollisionDataList(std::unordered_map<COLLISION_OBJECT_KIND, float>& _collisionDataList)
{
	ImGui::BeginChild("CollisionDataList",ImVec2(IMGUI_SLIDER_WIDTH * 1.5f,100.0f),false,1);
	
	for (const auto& collTarget : _collisionDataList)
	{
		// データの出力 //
		ImGui::TextUnformatted(CollisionObjectKindInfo::GetToString(collTarget.first).c_str());
		ImGui::SameLine();
		ImGui::TextUnformatted(std::to_string(collTarget.second).c_str());
	}
	// データリストの一番下へスクロール　常に最新のデータを見るため
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
	
	ImGui::EndChild();

	//_ 全ての当たり判定オブジェクトの種類から選択 _//

	if (ImGui::BeginCombo("CollisionObjectKindList", selectCollisionObjectKindStr.c_str()))
	{
		for (const auto& collObjKindStr : CollisionObjectKindInfo::COLLISION_OBJECT_KIND_STRING_LIST)
		{
			if (ImGui::Selectable(collObjKindStr.second.c_str()))
			{
				selectCollisionObjectKindStr	= collObjKindStr.second;
				setCollisionDataTargetKind		= collObjKindStr.first;
				isSelectCollisionData			= true;
			}
		}
		ImGui::EndCombo();
	}

	if (isSelectCollisionData)
	{
		//ImGui::Text(selectCollisionObjectKindStr.c_str());
		//ImGui::SameLine();
		ImGui::DragFloat("ToDamageNum", &setCollisionDataTargetDamge, 1.0f,0.0f, 1000.0f);

		bool isChangeCollData = false;	// 当たり判定データを変更したかどうか

		if (ImGui::Button("Add"))
		{
			if (_collisionDataList.find(setCollisionDataTargetKind) == _collisionDataList.end())
			{
				// 敵データに選択した敵の当たり判定データの種類と与えるダメージ量を設定
				_collisionDataList.emplace(setCollisionDataTargetKind, setCollisionDataTargetDamge);
			}
			else
			{
				// 敵データに選択した敵の当たり判定データの与えるダメージ量を設定
				_collisionDataList[setCollisionDataTargetKind] = setCollisionDataTargetDamge;
			}
			isChangeCollData = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{
			auto collDataItr = _collisionDataList.find(setCollisionDataTargetKind);

			// keyが存在したら
			if (collDataItr != _collisionDataList.end())
			{
				// 当たり判定データの削除
				collDataItr = _collisionDataList.erase(collDataItr);
				isChangeCollData = true;
			}
		}

		// 当たり判定データが変更されていたら
		if (isChangeCollData)
		{
			// 選択した情報をリセット //
			isSelectCollisionData		= false;
			setCollisionDataTargetKind	= COLLISION_OBJECT_KIND::NONE;
		}
	}
}

void EnemyManager::Draw()
{
	if (!isDrawEnemyRoot)
		return;	// ルートを描画しない場合 return

	if (!isDrawNextSummonEnemyRoot)
		return;	// ルートを描画しない場合 return

	for (const auto& root : nextSummonEnemyRootNumberList)
	{
		if (root.empty())
			continue;	// コンテナが空なので、以下のfor分で配列外参照しないように、contiune

		for (size_t i = 0;i < root.size() - 1;i++)
		{
			const VECTOR3* posNow	= GetNavPointPosition(root[i]);		// 現在の座標
			const VECTOR3* posNext	= GetNavPointPosition(root[i + 1]);	// 次の座標

			if (posNow == nullptr || posNext == nullptr)
				continue;	// 座標の代入ができなかったのでcontinue

			VECTOR3 pos1 = *posNow;
			VECTOR3 pos2 = *posNext;
			// 描画が見えやすいように描画のy座標を高くする //
			pos1.y += 10.0f;
			pos2.y += 10.0f;

			// カプセルでルートの描画
			DrawCapsule3D(pos1, pos2, 10, 2, 0xff0000, 0xffffff, true);
		}
	}
}

void EnemyManager::Push(EnemyBase* _enemy)
{
	enemies.insert(std::make_pair(EnemyManager::enemyNumber++, _enemy));
}

void EnemyManager::Pop(EnemyBase* _enemy)
{
	for (auto itr = enemies.begin();itr != enemies.end();)
	{
		EnemyBase* enemy = itr->second;

		// 引数の削除したい敵クラスポインタと一致したら
		if (enemy == _enemy)
		{
			itr = enemies.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void EnemyManager::CreateEnemy(const VECTOR3& _pos, const ENEMY_KIND& _kind)
{
	EnemyBase* obj = nullptr;

	switch (_kind)
	{
	case ENEMY_KIND::WALKER:

		obj = new Walker(_pos, hModels[(int)_kind],this);
		break;
	case ENEMY_KIND::CLIMBER:

		obj = new Climber(_pos, hModels[(int)_kind],this);
		break;
	case ENEMY_KIND::RUSHER:

		obj = new Rusher(_pos, hModels[(int)_kind],this);
		break;
	case ENEMY_KIND::CRUSHER:

		obj = new Crusher(_pos, hModels[(int)_kind], this);
		break;
	default:
		assert(false);
		break;
	}

	obj->Init(this);
	obj->SetDrawOrder(-1);
	obj->SetEnemyData(enemyDataAttachList.at((int)_kind));
}

void EnemyManager::SetNextSummonEnemyRootNumberList()
{
	// コンテナ情報をリセット
	nextSummonEnemyRootNumberList.clear();

	for (const auto& eneInfo : nextSummonEnemyInfoList)
	{
		const VECTOR3* corePos = GetCorePositionPtr();

		if (corePos == nullptr)
			continue;	// コアの座標を取得できなかったので、break

		ENEMY_KIND kind = (ENEMY_KIND)eneInfo.first;

		const EnemyInfo::EnemyData& enemyData = EnemyDataSerializer::GetEnemyData(kind);

		// 経路探索をして結果のeneInfoごとのルートのナビポイントナンバーを代入
		nextSummonEnemyRootNumberList.insert
		(
			GetTargetMoveNavPointContainer
			(
				VECTOR3(eneInfo.second),								// 現在の座標
				*corePos, 												// 目的地座標
				(int)kind, 												// エージェントの種類
				enemyData.moveData.walkSpeed,							// 移動速度
				enemyData.navigationData.nodeUseKind, 					// 使うノードの種類		地面/壁/全て
				enemyData.navigationData.navigationMode,				// ナビゲーションモード
				EnemyInfo::GetBlockedObjectDamagePerSecond(enemyData)	// DPS
			)															
		);
	}
}

void EnemyManager::EnemyNavigationReload()
{
	for (auto itr = enemies.begin();itr != enemies.end();itr++)
	{
		EnemyBase& obj = *itr->second;
		obj.ResetSearchInfo();
		//obj.SetState(EnemyBase::ENEMY_STATE::SEARCH);
	}

	// 次のウェーブで召喚される敵のルートのナビポイントナンバーコンテナを更新
	SetNextSummonEnemyRootNumberList();
}

int EnemyManager::GetNearEnemyNumber(const VECTOR3& _pos)
{
	float min			= FLT_MAX;
	float dis			= FLT_MAX;
	int nearEnemyNum	= -1;

	for (auto itr = enemies.begin();itr != enemies.end();itr++)
	{
		EnemyBase& ene = *itr->second;

		dis = VSquareSize(ene.GetPosition() - _pos);

		if (min > dis)
		{
			min				= dis;
			nearEnemyNum	= itr->first;
		}
	}

	if (nearEnemyNum < 0)
		assert(false);
	return nearEnemyNum;
}

const Transform& EnemyManager::GetNearEnemyTransform(const VECTOR3& _pos)
{
	return enemies[GetNearEnemyNumber(_pos)]->GetTransform();
}

const VECTOR3& EnemyManager::GetNearEnemyPosition(const VECTOR3& _pos)
{
	return enemies[GetNearEnemyNumber(_pos)]->GetPosition();
}

const VECTOR3& EnemyManager::GetEnemyPosition(const int& _num)
{
	std::unordered_map<int, EnemyBase*>::iterator itr = enemies.find(_num);

	if (itr == enemies.end())
	{
		assert(false);
	}
	return enemies[_num]->GetPosition();
}

const VECTOR3& EnemyManager::GetEnemyVelocity(const int& _num)
{
	std::unordered_map<int, EnemyBase*>::iterator itr = enemies.find(_num);

	if (itr == enemies.end())
	{
		assert(false);
	}
	return enemies[_num]->GetVelocity();
}

bool EnemyManager::RequestSummonEnemy(const VECTOR3& _pos, const ENEMY_KIND& _kind)
{	
	// 敵の生成
	CreateEnemy(_pos,_kind);
	return true;
}

void EnemyManager::SetNextSummonEnemyInfo(std::unordered_multimap<int, VECTOR3I> _enemyInfo)
{
	// コンテナをリセット //
	nextSummonEnemyInfoList.clear();
	nextSummonEnemyRootNumberList.clear();

	// 次のウェーブで召喚する敵の情報を代入
	nextSummonEnemyInfoList = std::move(_enemyInfo);

	// 次のウェーブで召喚される敵のルートのWayPointナンバーコンテナを設定
	SetNextSummonEnemyRootNumberList();
}
