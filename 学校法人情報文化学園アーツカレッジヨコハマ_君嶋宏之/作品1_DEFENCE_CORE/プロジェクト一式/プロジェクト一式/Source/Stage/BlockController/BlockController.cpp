#include "BlockController.h"
#include "../StageObjects/Block/BlockBase.h"

// 各ブロッククラス
#include "../StageObjects/Block/WallBlock.h"
#include "../StageObjects/Block/GroundBlock.h"
#include "../StageObjects/Block/SpikeBlock.h"
#include "../StageObjects/Block/SwampBlock.h"
#include "../StageObjects/Block/CoreBlock.h"
#include "../StageObjects/Block/SpawnerBlock.h"
#include "../StageObjects/Block/BackBlockBase.h"

// 各エンティティクラス
//#include "TurretBase.h"

#include "../StageManager/StageManager.h"
#include "../StageInfo.h"

#include "../Trap/TrapInfo.h"
#include "../../../Library/DebugNew.h"

BlockController* BlockController::objectMe	= nullptr;
bool BlockController::isActiveInstance		= true;		// インスタンスがnewされているかのフラグ

BlockController::BlockController()
{
	BlockController::isActiveInstance = true;

	stageManager = nullptr;
}

void BlockController::Init(StageManager* _stageManager)
{
	stageManager = _stageManager;
	assert(stageManager != nullptr);

	const std::unordered_map<int, std::vector<int>>& handls = GetRawModelHandles();	// モデルハンドルコンテナを取得

	for (auto& handl : handls)
	{
		hModels[handl.first] = handl.second;
	}

	for (auto& handl : handls)
	{
		hPreviewModel[handl.first] = MV1DuplicateModel(*handl.second.begin());
	}
}

BlockController::~BlockController()
{
	for (auto& model : hModels)
	{
		for (auto& levelModel : model.second)
		{
			MV1DeleteModel(levelModel);
		}
	}
	hModels.clear();
	
	for (auto& preModel : hPreviewModel)
	{
		MV1DeleteModel(preModel.second);
	}
	hPreviewModel.clear();

	StageManagerForDeleteBlock();

	BlockController::isActiveInstance = false;
}

BlockController* BlockController::GetBlockController()
{
	if (objectMe == nullptr)
	{
		objectMe = new BlockController();
	}
	return objectMe;
}

void BlockController::DeleteBlockController()
{
	if (objectMe != nullptr)
	{
		delete objectMe;
		objectMe = nullptr;
	}
}

void BlockController::DrawTrapPutPreview(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, const float& _alpha)
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

StageObjectBase* BlockController::PutBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	return CreateBlock(_trans, _kind, 0, STAGE_OBJECT_CREATE_WAY::DYNAMIC);
}

StageObjectBase* BlockController::CreateBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _useModelIndex)
{
	return CreateBlock(_trans, _kind, _useModelIndex, STAGE_OBJECT_CREATE_WAY::STATIC);
}

BlockBase* BlockController::CreateBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _useModelIndex, const STAGE_OBJECT_CREATE_WAY& _createWay)
{
	BlockBase* blockObject = nullptr;
	
	// ハンドルコンテナのキーが見つからなかったら
	if (hModels.find((int)_kind) == hModels.end())
		assert(false);

	assert(hModels[(int)_kind].size() > _useModelIndex);

	const StageObjectBase::ModelData& modelData = StageObjectBase::ModelData(hModels[(int)_kind][_useModelIndex], _useModelIndex);	// 使用するモデルハンドルデータ

	switch (_kind)
	{
	case StageObjectData::STAGE_OBJECT_KIND::GROUND_BLOCK:

		if (_createWay == STAGE_OBJECT_CREATE_WAY::DYNAMIC)
			return nullptr;	// 地面ブロックはプレイヤーが生成できないようにする
		blockObject = new GroundBlock(_trans, modelData, StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK:

		blockObject = new WallBlock(_trans, modelData, StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::SWAMP_BLOCK:

		if (_createWay == STAGE_OBJECT_CREATE_WAY::DYNAMIC)
			return nullptr;	// 泥ブロックは今後削除
		blockObject = new SwampBlock(_trans, modelData, StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK:

		blockObject = new SpikeBlock(_trans, modelData, StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::CORE_BLOCK:

		if (_createWay == STAGE_OBJECT_CREATE_WAY::DYNAMIC)
			return nullptr;	// 地面ブロックはユーザーは生成できないようにする
		blockObject = new CoreBlock(_trans, modelData, StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::SPAWNER_BLOCK:

		blockObject = new SpawnerBlock(_trans, modelData, StageObjectData::GetStageObjectHPConfig(_kind));
		break;
	case StageObjectData::STAGE_OBJECT_KIND::BACK_MOUNTAIN:
	case StageObjectData::STAGE_OBJECT_KIND::BACK_ROCK:

		blockObject = new BackBlockBase(_trans, modelData, _kind, StageObjectData::GetStageObjectHPConfig(_kind), true);
		break;
	case StageObjectData::STAGE_OBJECT_KIND::BACK_TREE:

		blockObject = new BackBlockBase(_trans, modelData, _kind, StageObjectData::GetStageObjectHPConfig(_kind), false);
		break;
	default:
		assert(false);
		break;
	}
	std::pair<std::unordered_map<BlockBase* ,STAGE_OBJECT_CREATE_WAY>::iterator, bool> pair = blocks.insert(std::make_pair(blockObject, _createWay));
	
	if (!pair.second)
	{
		// すでに同じキーが登録されています
		assert(false);
	}
	return blockObject;
}

void BlockController::StageManagerForDeleteBlock()
{
	for (auto itr = blocks.begin();itr != blocks.end();)
	{
		BlockBase* obj = itr->first;

		if (obj != nullptr)
		{
			obj->DestroyMe();
			obj = nullptr;
		}
		itr = blocks.erase(itr);
	}
}

void BlockController::StageManagerForDeleteBlock(const STAGE_OBJECT_CREATE_WAY& createWay)
{
	for (auto itr = blocks.begin();itr != blocks.end();)
	{
		if (itr->second == createWay)
		{		
			BlockBase* obj = itr->first;

			if (obj != nullptr)
			{
				obj->DestroyMe();
				obj = nullptr;
			}
			itr = blocks.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

const std::list<BlockBase*> BlockController::GetAllBlockObject()
{
	std::list<BlockBase*> re;

	for (auto itr = blocks.begin();itr != blocks.end();itr++)
	{
		BlockBase* block = itr->first;

		re.emplace_back(block);
	}
	return re;
}

std::list<StageObjectBase*> BlockController::GetAllStageObject()
{
	std::list<StageObjectBase*> re;

	for (auto itr = blocks.begin();itr != blocks.end();itr++)
	{
		StageObjectBase* stageObj = itr->first;

		re.emplace_back(stageObj);
	}
	return re;
}

bool BlockController::DeleteBlock(BlockBase* _obj)
{
	std::unordered_map<BlockBase*, STAGE_OBJECT_CREATE_WAY>::iterator itr = blocks.find(_obj);

	if (itr == blocks.end())
	{
		// 引数のキーのmap要素を見つけられませんでした。
		assert(false);
		return false;
	}

	BlockBase& block = *itr->first;

	if (itr->first != nullptr)
	{
		block.DestroyMe();
	}
	itr = blocks.erase(itr);

	//stageManager->ChangedStageObject();

	// putPointに置かれたオブジェクトだったら
	if (block.GetPutPlaceKind() == StageObjectBase::PUT_PLACE_KIND::PUT_POINT)
	{
		stageManager->SendDeletePutPointStageObject(block.GetTransform());
	}
	return true;
}

bool BlockController::DeleteBlock(const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	assert(StageObjectData::IsBlockStageObjectKind(_kind));

	for (auto blockItr = blocks.begin();blockItr != blocks.end();)
	{
		BlockBase* blockObj = blockItr->first;

		if (blockObj != nullptr)
		{
			// 削除したいステージオブジェクトの種類と一致したら
			if (blockObj->GetStageObjectKind() == _kind)
			{
				// オブジェクトの削除
				blockObj->DestroyMe();
				blockObj = nullptr;

				blockItr = blocks.erase(blockItr);
				continue;	// コンテナから要素を削除したので continue
			}
		}
		blockItr++;
	}
	return false;
}

VECTOR3 BlockController::GetPutGridPosition(const Transform& _trans)
{
	return _trans.position / StageInfo::PUT_GRID_SIZE;
}

const VECTOR3* BlockController::GetCorePosition() const
{
	for (auto itr = blocks.begin();itr != blocks.end();itr++)
	{
		if (itr->first->GetStageObjectKind() == StageObjectData::STAGE_OBJECT_KIND::CORE_BLOCK)
		{
			return &itr->first->GetTransform().position;
		}
	}
	return nullptr;
}

bool BlockController::IsCoreBroken()
{
	for (auto itr = blocks.begin();itr != blocks.end();itr++)
	{
		if (itr->first->GetStageObjectKind() == StageObjectData::STAGE_OBJECT_KIND::CORE_BLOCK)
		{
			return static_cast<CoreBlock*>(itr->first)->IsCoreBroken();
		}
	}
	return false;
}

bool BlockController::CheckRaycastStageObject(const VECTOR3& _pos1, const VECTOR3& _pos2, const std::set<int>& _kindList, VECTOR3* _hitP)
{
	return stageManager->CheckRaycastStageObject(_pos1, _pos2, _kindList, _hitP);
}

void BlockController::NavigationAreaBoxHPChange(const Transform* _trans, const int& _hp)
{
	stageManager->NavigationAreaBoxHPChange(_trans, _hp);
}

void BlockController::DeleteNavigationAreaBoxTransformPtr(const Transform* _trans)
{
	if (!StageManager::IsActiveInstance())
		return;	// ステージマネージャーが削除されていたら return

	stageManager->DeleteNavigationAreaBoxTransformPtr(_trans);
}

const std::unordered_map<int, std::vector<int>> BlockController::GetRawModelHandles()
{
	std::unordered_map<int, std::vector<int>> rawModelHandleCon;	// 元のモデルハンドルのコンテナ

	int stageNum		= stageManager->GetLoadStageNum();
	std::set<int> con	= std::move(StageObjectData::GetStageObjectKindContainer(StageObjectData::TYPE::BLOCK));
	int count			= 0;		// イテレータのカウント
	int handleId		= 0;		// ハンドルの名前のナンバー

	std::vector<int> rawHModel;		// ハンドルコンテナ
	rawHModel.resize(con.size());

	for (auto itr = con.begin();itr != con.end();itr++, count++)
	{
		// Groundモデルのとき
		if (count == 0)
		{
			/*if (stageNum == 2)
				handleId = 10;
			if (stageNum == 3)
				handleId = 20;*/

			// ステージ番号に対して以下の形式で設定
			// 1 -> 0
			// 2 -> 1
			// 3 -> 2 ...
			handleId = (stageNum * 100) - 100;
		}
		else
		{
			handleId = count * 10;
		}

		int len = 1;			// for分を回す回数

		// 罠のステージオブジェクトの種類だったら
		if (TrapInfo::IsTrapStageObjectKind((StageObjectData::STAGE_OBJECT_KIND)count))
		{
			len = 2;	// レベル2まで読み込む
		}
		// 背景のステージオブジェクトの種類だったら
		else if(StageObjectData::IsBackBlockStageObjectKind((StageObjectData::STAGE_OBJECT_KIND)count))
		{
			len = 9;	// 10未満の数値全てを読み込む
		}

		for (int i = 0;i < len;i++)
		{
			rawHModel[i] = -1;	// コンテナの初期化
			char set[CHAR_MAX];
			sprintfDx(set, "data/models/stageObject/blocks/block_%.3d.mv1", handleId + i);
			rawHModel[i] = MV1LoadModel(set);

			if (len <= 1)
			{
				assert(rawHModel[i] >= 0);
			}
			else if(rawHModel[i] < 0)
			{
				break;	// モデルの読み込みに失敗した場合は、モデルハンドルがないと判断して、break
			}

			// 当たり判定を行うための初期化
			//MV1SetupCollInfo(rawHModel[i]);
			
			// コンテナに代入
			rawModelHandleCon[count].emplace_back(rawHModel[i]);
		}
	}
	return rawModelHandleCon;
}
