#include "EntityController.h"

#include "../StageObjects/Entity/Turret/AutoTurret.h"
#include "../StageObjects/Entity/Turret/ActiveTurret.h"
#include "../StageObjects/Entity/Turret/ReloadTurret.h"
#include "../StageObjects/Entity/Coin.h"
#include "../StageObjects/Entity/SlimePatch.h"
#include "../StageObjects/Entity/JumpPad.h"
#include "../StageObjects/Entity/Bomb/RemoteBomb.h"

#include "../StageManager/StageManager.h"
#include "../StageInfo.h"
#include "../Trap/TrapInfo.h"

#include "../../Shot/ShotManager/ShotManager.h"
#include "../../Common/UI/UIDrawManager/UIDrawManager.h"

#include "../../Common/Function/Function.h"
#include "../../../ImGui/imgui.h"
#include "../../../Library/DebugNew.h"

EntityController* EntityController::objectMe	= nullptr;
bool EntityController::isActiveInstance			= true;

EntityController::EntityController()
{
	EntityController::isActiveInstance = true;

	const std::set<int> stageObjTypeCon = StageObjectData::GetStageObjectKindContainer(StageObjectData::TYPE::ENTITY);	// タイプに属するステージオブジェクトの種類コンテナ
	int handleId						= 0;		// ハンドルの名前のナンバー

	for (auto itr = stageObjTypeCon.begin();itr != stageObjTypeCon.end();itr++, handleId += 10)
	{
		// コンテナの初期化 //
		hModel[*itr];	
		hPreviewModel[*itr] = -1;

		int len = 1;			// for分を回す回数

		if (TrapInfo::IsTrapStageObjectKind((StageObjectData::STAGE_OBJECT_KIND)*itr))
		{
			// レベル2まで読み込む
			len = 2;	
		}

		for (int i = 0;i < len;i++)
		{
			char set[CHAR_MAX];
			sprintfDx(set, "data/models/stageObject/Entities/entity_%.3d.mv1", handleId + i);
			hModel[*itr].emplace_back(MV1LoadModel(set));
			assert(hModel[*itr].back() >= 0);
			
			// ハンドルが設定されていなかったら
			if (hPreviewModel[*itr] < 0)
				hPreviewModel[*itr] = MV1DuplicateModel(*hModel[*itr].begin());

			// 当たり判定の初期化
			//MV1SetupCollInfo(hModel[*itr].back());
		}
	}

	//circleModel = MV1LoadModel("data/model/circle/circle.mv1");
	
	//_ テクスチャのロード _//

	const std::string TEXTURE_PATH = "data/texture/";

	entityTextureHandleList.insert(std::make_pair("warning",			LoadGraph(std::string(TEXTURE_PATH + "warning.png").c_str())));
	entityTextureHandleList.insert(std::make_pair("f_key",				LoadGraph(std::string(TEXTURE_PATH + "f_key.png").c_str())));
	entityTextureHandleList.insert(std::make_pair("num_white",			LoadGraph(std::string(TEXTURE_PATH + "letter/" + "num_white.png").c_str())));
	entityTextureHandleList.insert(std::make_pair("upgrade_cost",		LoadGraph(std::string(TEXTURE_PATH + "letter/" + "upgrade_cost.png").c_str())));
	entityTextureHandleList.insert(std::make_pair("mouse_right_click",	LoadGraph(std::string(TEXTURE_PATH + "mouse_right_click.png").c_str())));

	for (const auto& texture : entityTextureHandleList)
		assert(texture.second > 0 && "ハンドルが初期化されませんでした。");

	stageManager	= nullptr;
	shotManager		= nullptr;

	// UI描画関数を登録
	turretUIFunctionIDNumber = UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this]() { TurretInfoUIDraw(); });
}

void EntityController::Init(StageManager* _stageManager)
{
	stageManager	= _stageManager;
	shotManager		= FindGameObject<ShotManager>();
	assert(shotManager != nullptr);
}

EntityController::~EntityController()
{
	for (auto& model : hModel)
	{
		for (auto& levelModel : model.second)
			MV1DeleteModel(levelModel);
	}
	hModel.clear();

	for (auto model : hPreviewModel)
	{
		MV1DeleteModel(model.second);
	}
	hPreviewModel.clear();

	//MV1DeleteModel(circleModel);

	for (auto model : entityTextureHandleList)
	{
		MV1DeleteModel(model.second);
	}

	StageManagerForDeleteEntity();

	// 登録したUI描画関数を解放
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(turretUIFunctionIDNumber);

	EntityController::isActiveInstance = false;
}

EntityController* EntityController::GetEntityController()
{
	if (objectMe == nullptr)
	{
		// Init()を呼んでください
		//assert(false);
		objectMe = new EntityController();
	}
	return objectMe;
}

void EntityController::DeleteEntityController()
{
	if (objectMe != nullptr)
	{
		delete objectMe;
		objectMe = nullptr;
	}
}

void EntityController::Update()
{
}

void EntityController::TurretInfoUIDraw()
{
	if (!isDrawTurretInfoUI)
		return;	// 描画しないとき、return

	TurretBase* nearTurret		= nullptr;							// 一番プレイヤーに近いタレットポインタ
	TurretBase* upgradeTurret	= nullptr;							// アップグレードするタレット
	float minSquareDistance		= FLT_MAX;							// 最近値
	VECTOR3 playerPos			= GetPlayerTransform().position;	// プレイヤー座標

	bool findTurret				= false;							// タレットのインスタンスが見つかったかどうか

	for (const auto& entity : entities)
	{
		// タレットのインスタンスでなかったら
		if (entity->GetStageObjectKind() != StageObjectData::STAGE_OBJECT_KIND::ACTIVE_TURRET &&
			entity->GetStageObjectKind() != StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE &&
			entity->GetStageObjectKind() != StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD &&
			entity->GetStageObjectKind() != StageObjectData::STAGE_OBJECT_KIND::AUTO_TURRET)
			continue;	// 処理を飛ばす

		float squareDistance = VSquareSize(playerPos - *entity->GetPositionPtr());

		TurretBase* turret = static_cast<TurretBase*>(entity);

		// 弾を発射できない状態だったら
		if(!turret->CanShot())
		{
			// タレットの警告描画
			turret->CanNotShotDraw();
		}

		// アップグレードしていたら
		if (turret->IsUpgrading())
		{
			// タレットを記録
			upgradeTurret = turret;
		}

		if (minSquareDistance > squareDistance)
		{
			// タレットを記録
			nearTurret = turret;
			findTurret = true;

			minSquareDistance = squareDistance;
		}
	}

	if (!findTurret)
		return;	// タレットを見つけることができなかったので、return

	if (nearTurret != nullptr)
	{
		// タレットの残り弾数の描画
		nearTurret->BulletInfoDraw();
		// タレットのアップグレードの描画
		nearTurret->TrapUpgradInfoDraw();
	}
}

void EntityController::DrawTrapPutPreview(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, const float& _alpha)
{
	// コンテナにキーが存在するか確認
	HasUnorderedMapContainerKey(hPreviewModel, (int)_kind, true);

	// モデルの負透明度の設定
	MV1SetOpacityRate(hPreviewModel[(int)_kind], _alpha);
	// ハンドルに行列を設定
	_trans.SetMatrixModel(hPreviewModel[(int)_kind]);
	// モデルの描画
	MV1DrawModel(hPreviewModel[(int)_kind]);
}

const Transform* EntityController::SummonEntity(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	return CreateEntity(_trans, _kind);
}

const Transform& EntityController::GetNearEnemyTransform(const VECTOR3& _pos)
{
	return stageManager->GetNearEnemyTransform(_pos);
}

const VECTOR3& EntityController::GetNearEnemyPosition(const VECTOR3& _pos)
{
	return stageManager->GetNearEnemyPosition(_pos);
}

const VECTOR3& EntityController::GetEnemyPosition(const int _eneNumber)
{
	return stageManager->GetEnemyPosition(_eneNumber);
}

bool EntityController::IsEnemySummon() const
{
	return stageManager->IsEnemySummon();
}

//void EntityController::SetShotInterval(void* _pointer, const float _interval)
//{
//	stageManager->SetShotInterval(_pointer, _interval);
//}

//void EntityController::DeleteShotOwner(const void* _pointer)
//{
//	stageManager->DeleteShotOwner(_pointer);
//}

//bool EntityController::Shot(const VECTOR3& _pos, const VECTOR3& _vel, const BULLET_KIND& _bulletKind, void* _pointer)
//{
//	return stageManager->Shot(_pos, _vel, _bulletKind, _pointer);
//}

std::list<StageObjectBase*> EntityController::GetAllStageObject()
{
	std::list<StageObjectBase*> re;

	for (auto itr = entities.begin();itr != entities.end();itr++)
	{
		StageObjectBase* b = *itr;

		re.emplace_back(b);
	}
	return re;
}

//bool EntityController::StageObjectTypeRayCollision(const VECTOR3& _pos1, const VECTOR3& _pos2, const StageObjectData::TYPE& _type, VECTOR3* _hitP)
//{
//	return stageManager->StageObjectTypeRayCollision(_pos1, _pos2, _type,_hitP);
//}

bool EntityController::CheckRaycastStageObject(const VECTOR3& _pos1, const VECTOR3& _pos2, const std::set<int>& _kindList, VECTOR3* _hitP)
{
	return stageManager->CheckRaycastStageObject(_pos1, _pos2, _kindList, _hitP);
}

void EntityController::NavigationAreaBoxHPChange(const Transform* _trans, const int& _hp)
{
	/*if (!StageManager::IsActiveInstance())
		return;*/

	stageManager->NavigationAreaBoxHPChange(_trans, _hp);
}

void EntityController::DeleteNavigationAreaBoxTransformPtr(const Transform* _trans)
{
	if (!StageManager::IsActiveInstance())
		return;

	stageManager->DeleteNavigationAreaBoxTransformPtr(_trans);
}

const std::vector<int>& EntityController::GetModelHandle(const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	// コンテナにキーが存在するか確認
	HasUnorderedMapContainerKey(hModel, (int)_kind,true);

	return hModel[(int)_kind];
}

const Transform* EntityController::CreateEntity(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	EntityBase* obj = nullptr;

	// コンテナにキーが存在するか確認
	HasUnorderedMapContainerKey(hModel, (int)_kind, true);

	switch (_kind)
	{
	case StageObjectData::STAGE_OBJECT_KIND::COIN:

		obj = new Coin(_trans, *hModel[(int)_kind].begin(), StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::AUTO_TURRET:

		obj = new AutoTurret(_trans, *hModel[(int)_kind].begin(),StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::ACTIVE_TURRET:

		obj = new ActiveTurret(_trans, *hModel[(int)_kind].begin(), StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE:
	case StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD:

		obj = new ReloadTurret(_trans, *hModel[(int)_kind].begin(), _kind,StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::SLIME_PATCH:

		obj = new SlimePatch(_trans, *hModel[(int)_kind].begin(), StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::JUMP_PAD:

		obj = new JumpPad(_trans, *hModel[(int)_kind].begin(), StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::REMOTE_BOMB:

		obj = new RemoteBomb(_trans, *hModel[(int)_kind].begin(), StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	default:
		assert(false);
		break;
	}

	std::pair<std::unordered_set<EntityBase*>::iterator, bool> pair = entities.insert(obj);

	if (!pair.second)
	{
		// すでに同じキーが登録されています
		assert(false);
	}

	// この関数内で罠に分類された場合はステージマネージャーで、objのITrap(罠のインタフェース)を管理される
	stageManager->PushInterfaceTrapList(obj);

	//stageManager->ChangedStageObject();

	return &obj->GetTransform();
}

const Transform& EntityController::GetPlayerTransform() const
{
	return stageManager->GetPlayerTransform();
}

void EntityController::StageManagerForDeleteEntity()
{
	for (auto itr = entities.begin();itr != entities.end();)
	{
		EntityBase& obj = **itr;

		if (*itr != nullptr)
			obj.DestroyMe();

		itr = entities.erase(itr);

		stageManager->DeleteInterfaceTrapList(&obj);
	}
	entities.clear();
}

bool EntityController::DeleteEntity(EntityBase* _obj)
{
	std::unordered_set<EntityBase*>::iterator itr = entities.find(_obj);

	if (itr == entities.end())
	{
		// 引数のキーのmap要素を見つけられませんでした。
		assert(false);
		return false;
	}
	EntityBase& obj = **itr;

	if (*itr != nullptr)
		obj.DestroyMe();

	itr = entities.erase(itr);

	stageManager->DeleteInterfaceTrapList(_obj);

	// putPointに置かれたオブジェクトだったら
	if (obj.GetPutPlaceKind() == StageObjectBase::PUT_PLACE_KIND::PUT_POINT)
		stageManager->SendDeletePutPointStageObject(obj.GetTransform());
	return true;
}

VECTOR3 EntityController::GetPutGridPosition(const Transform& _trans)
{
	return _trans.position / StageInfo::PUT_GRID_SIZE;
}
