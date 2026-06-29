#include "StageManager.h"
#include "../StageObjects/StageObjectBase.h"
#include "../StageObjects/Block/BlockBase.h"
#include "../StageObjects/Entity/EntityBase.h"
#include "../BlockController/BlockController.h"
#include "../EntityController/EntityController.h"
#include "../StageInfo.h"
#include "../StageEditor/StageEditor.h"
#include "../Trap/TrapInterface/ITrap.h"
#include "../Trap/TrapInfo.h"
#include "../../Common/CsvReader/CsvReader.h"
#include "../../Navigation/NavigationManager/NavigationManager.h"
#include "../../Collision/CollisionFunction.h"
#include "../../Collision/CollisionManager/CollisionManager.h"
#include "../../Collision/Collisions/SphereCollision.h"
#include "../../Enemy/EnemyManager/EnemyManager.h"
#include "../../Shot/ShotManager/ShotManager.h"
#include "../../Game/GameController.h"
#include "../../Camera/Camera.h"
#include "../../Common/Sky/Sky.h"
#include "../../Common/Sound/SoundManager.h"
#include "../../Struct.h"
#include "../../Screen.h"
#include "../../../ImGui/imgui.h"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <vector>
#include "../../../Library/DebugNew.h"

#define STAGE_ALL_AREA_PUT (1)	// ステージ全体でのステージオブジェクト配置

namespace
{
	std::vector<std::vector<int>> gridChip =	// 設置ポイントの配置コンテナ
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	};

	enum MEMBER
	{
		POSITION_X = 0,
		POSITION_Y,
		POSITION_Z,
		ROTATION_X,
		ROTATION_Y,
		ROTATION_Z,
		SCALE_X,
		SCALE_Y,
		SCALE_Z,

		MAX,
	};

	constexpr int STAGE_PUT_POINT_MAX					= 3;		// ステージポイントの最大値　今後csvのファイル数に応じた変数に変更
	constexpr float STAGE_OBJECT_PUT_COLLISION_RAY_LEN	= 1000.0f;	// ステージオブジェクトを配置する際の置かれる場所のモデルとのレイの当たり判定でのレイの長さ
}

bool StageManager::isActiveInstance = true;

StageManager::StageManager()
{
	StageManager::isActiveInstance = true;

	stageEditor = new StageEditor();
	
	Transform transform;
	transform.position	= GetStageCenterPosition();
	transform.scale		= VOne * 40.0f;
	new Sky(transform, MV1LoadModel("data/models/sky/sky_2.mv1"));

	circleModel = MV1LoadModel("data/models/circle/impactCircle.mv1");
	assert(circleModel > 0 && "ハンドルの初期化がされていません");

	stageObjectPutPointIndexPosList.clear();
	stageObjectPutPointCenterPositionList.clear();
	interfaceTrapList.clear();
}

void StageManager::Init(int _loadStageNum)
{
	loadStageNum		= _loadStageNum;

	enemyManager		= FindGameObject<EnemyManager>();
	navigationManager	= FindGameObject<NavigationManager>();

	deleteStageObjectCollision = nullptr;

	stageEditor->Init(this);

	BlockController::GetBlockController()->Init(this);
	EntityController::GetEntityController()->Init(this);

	// ステージの生成
	CreateStage(_loadStageNum);
}

StageManager::~StageManager()
{
	// ステージオブジェクトの削除 //
	BlockController::GetBlockController()->StageManagerForDeleteBlock();
	EntityController::GetEntityController()->StageManagerForDeleteEntity();

	// Controllerの削除 //
	BlockController::DeleteBlockController();
	EntityController::DeleteEntityController();

	stageObjectPutPointIndexPosList.clear();
	stageObjectPutPointCenterPositionList.clear();
	navigationAreaBoxNumberList.clear();
	interfaceTrapList.clear();

	if (deleteStageObjectCollision != nullptr)
	{
		delete deleteStageObjectCollision;
		deleteStageObjectCollision = nullptr;	
	}

	if (stageEditor != nullptr)
	{
		delete stageEditor;
		stageEditor = nullptr;
	}

	MV1DeleteModel(circleModel);

	StageManager::isActiveInstance = false;
}

void StageManager::Update()
{
	EntityController::GetEntityController()->Update();
	
#ifdef _DEBUG

	if (canStageEditor)
	{
		stageEditor->Update();
	}
#endif // _DEBUG

	for (auto& trap : interfaceTrapList)
	{
		// アップグレード進行中でなかったら
		if (!trap.second->IsUpgrading())
		{
			// アップグレードの進行値を0にリセットする
			trap.second->ResetUpgrading();
		}
	}
}

void StageManager::Draw()
{
#ifdef _DEBUG
	if (canStageEditor)
	{
		stageEditor->Draw();
	}
#endif // _DEBUG

	// 罠の影響範囲を描画するとき
	if (isDrawTrapImpactRadius)
	{
		// プレイヤーから一定距離内の一番近くの罠オブジェクトを取得
		const ITrap* trap = GetNearTrap(GetPlayerTransform().position, TrapInfo::MAX_TRAP_INFO_DRAW_DISTANCE);

		if (trap != nullptr)
		{
			// 影響範囲が設定されていたら
			if (trap->GetImpactRadius() > 0.0f)
			{
				const Transform& trapTrans	= trap->GetTransform();	// トラップのトランスフォーム
				VECTOR3 impactPos			= trapTrans.position;	// 影響範囲の描画座標
				float impactDrawSubYPos		= 20.0f;				// 座標調整用数値
				// モデルの底面に描画するので下げる 下げすぎないように調整
				impactPos.y					-= trapTrans.GetLenY() / 2 - impactDrawSubYPos;

				// 影響範囲の描画
				DrawTrapImpactRadius(impactPos, trap->GetImpactRadius());
			}
		}
	}

	// タレットの情報のUI描画を設定
	EntityController::GetEntityController()->SetDrawTurretInfoUIFlag(isDrawTurretInfoUI);

#if 0
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	SetUseLighting(false);
	for (const auto& itr : stageObjectPutPointIndexPosList)
	{
		for (const auto& data : stageObjectPutPointIndexPosList[itr.first])
		{
			int color = 0xffffff;
			/*if (data.isPut)
				color = 0x0000ff;*/
			//DrawSphere3D(data.position, 50.0f, 16, color, 0xffffff, false);
			DrawCube3D(data.position - VOne * StageInfo::BLOCK_SIZE / 2.0f, data.position + VOne * StageInfo::BLOCK_SIZE / 2.0f, color, 0xffffff, false);
		}
		DrawSphere3D(stageObjectPutPointCenterPositionList[itr.first], 70.0f, 16, 0xff0000, 0xffffff, false);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetUseLighting(true);
#endif
}

void StageManager::DrawTrapImpactRadius(const VECTOR3& _pos, const float& _radius) const
{
	// 罠の影響範囲描画
	//DrawSphere3D(_pos, _radius, 8, 0xff0000, 0xffffff, false);
	// 補助線
	//DrawLine3D(_pos, _pos + VECTOR3(0.0f, _radius, 0.0f), 0xff0000);

	// ライトの影響をオフ
	SetUseLighting(false);

	//_ 罠の影響範囲描画 _//

	VECTOR3 circleSize			= VOne * 220.0f;
	VECTOR3 circleScale			= VOne * ((_radius * 2) / circleSize.x);
	Transform circleTransform	= Transform(_pos, VZero, circleScale, circleSize);	// トランスフォーム	わかりやすくsizeはモデルの直径ではなく、引数の半径 * 2(直径)にする
	// モデルにマトリックスを設定
	circleTransform.SetMatrixModel(circleModel);
	// 円形モデルの描画
	MV1DrawModel(circleModel);

	// ライトの影響をオン
	SetUseLighting(true);
}


bool StageManager::CreateStage(const int& _stageNum)
{
	char set[CHAR_MAX];
	std::string file;

	//_ パスの初期化 _//

	sprintfDx(set, "data/stage/stageData/stage_%.2d/stage.csv", _stageNum);
	file			= set;

	// csvの読み込み
	CsvReader* csv	= new CsvReader(file);

	Transform transform;						// ステージオブジェクトのトランスフォーム
	StageObjectData::STAGE_OBJECT_KIND kind;	// ステージオブジェクトの種類
	int useModelIndex = 0;						// ステージオブジェクトの使用するモデルハンドルのインデクス

	for (int n = 0;n < csv->GetLines();n++)
	{
		for (int c = 0;c < csv->GetColumns(n);c++)
		{
			// csvからの値を変数に代入
			if (c < MEMBER::MAX)		// transformを初期化する行の値だったら
			{
				SetTransformToCsvColumn(transform, csv->GetFloat(n, c), c);
			}
			else if(c == MEMBER::MAX)	// kindを初期化する行の値だったら
			{
				kind = (StageObjectData::STAGE_OBJECT_KIND)csv->GetInt(n, c);
			}
			else						// 使用するモデルインデクスを初期化する行の値だったら
			{
				useModelIndex = csv->GetInt(n, c);
			}
		}

		if (StageObjectData::GetStageObjectType(kind) == StageObjectData::TYPE::BLOCK)
		{
			BlockController::GetBlockController()->CreateBlock(transform, kind, useModelIndex);

			if (kind == StageObjectData::STAGE_OBJECT_KIND::GROUND_BLOCK)
			{
				// 設置ポイントの初期化
				LoadStageObjectPutPointData(_stageNum);
			}
		}
		else
		{
			CreatePutPointStageObject(transform.position, kind);
		}
	}
	delete csv;
	return true;
}

void StageManager::SaveStage(const int& _stageNum, bool _override)
{
	char set[CHAR_MAX];
	std::string filePath;

	std::list<std::string> data;	// ファイルに書き出す情報を行ごとにまとめる string出ないと出力できない

	std::ofstream file;
	bool isFirstOpenFile = true;	// ファイルを初めてオープンしたかどうか

	assert(_stageNum > 0 && "不正なウェーブナンバーです");
	
	sprintfDx(set, "data/stage/stageData/stage_%.2d/stage.csv", _stageNum);
	filePath = set;

	// 上書き保存の時
	if (_override)
	{
		// これでパスの通りにファイルを作成 or 開く // std::ios::truncは完全にデータを一から構築	※file.open()のデフォルトはstd::ios::trunc
		file.open(filePath, std::ios::trunc);
	}
	// 通常保存のとき
	else
	{
		// これでパスの通りにファイルを作成 or 開く // std::ios::appはデータの末尾に追加データを追加していくことを指定
		file.open(filePath, std::ios::app);
	}

	assert(file && "ファイルを開けませんでした");

	for (const auto& stageObject : GetAllStageObject())
	{
		if (stageObject->GetStageObjectKind() == StageObjectData::STAGE_OBJECT_KIND::SPAWNER_BLOCK)
			continue;	// スポナーは動的にWaveControllerが設置をStageManagerに要請するので、ステージ情報として書き出さない

		// 座標
		data.emplace_back(std::to_string(stageObject->GetTransform().position.x));
		data.emplace_back(std::to_string(stageObject->GetTransform().position.y));
		data.emplace_back(std::to_string(stageObject->GetTransform().position.z));
		// 回転
		data.emplace_back(std::to_string(stageObject->GetTransform().rotation.x));
		data.emplace_back(std::to_string(stageObject->GetTransform().rotation.y));
		data.emplace_back(std::to_string(stageObject->GetTransform().rotation.z));
		// スケール
		data.emplace_back(std::to_string(stageObject->GetTransform().scale.x));
		data.emplace_back(std::to_string(stageObject->GetTransform().scale.y));
		data.emplace_back(std::to_string(stageObject->GetTransform().scale.z));

		// ステージオブジェクトの種類
		data.emplace_back(std::to_string((int)stageObject->GetStageObjectKind()));

		// 使用するモデルのインデクス
		data.emplace_back(std::to_string(stageObject->GetModelData().useModelIndex));

		// データをすべて回す
		for (auto dItr = data.begin();dItr != data.end();)
		{
			// データの書き出し
			file << *dItr;

			// 列の最後でなかったら
			if (++dItr != data.end())
			{
				// 切り分け（列）
				file << ',';
			}
		}
		// 改行（行）
		file << "\n";
		// データリセット
		data.clear();
	}
	// ファイルを閉じる
	file.close();
}

bool StageManager::DeleteStage(const int& _stageNum)
{
	char set[CHAR_MAX];
	std::string filePath;

	assert(_stageNum > 0 && "不正なウェーブナンバーです");

	sprintfDx(set, "data/stage/stageData/stage_%.2d/stage.csv", _stageNum);
	filePath = set;

	// ファイルの削除　削除できなかった場合は、falseを返す
	return std::filesystem::remove_all(filePath);
}

VECTOR3 StageManager::LoadPlayerStartPosition()
{
	if (loadStageNum < 0)
		assert(false && "ロードするステージナンバーが初期化されていません");

	char set[CHAR_MAX];
	std::string file;

	//_ パスの初期化 _//

	sprintfDx(set, "data/stage/stageData/stage_%.2d/playerPos.csv", loadStageNum);
	file				= set;

	// csvを読み込む
	CsvReader* csv		= new CsvReader(file);
	VECTOR3 playerPos	= VZero;

	for (int n = 0;n < csv->GetLines();n++)
	{
		for (int c = 0;c < csv->GetColumns(n);c++)
		{
			SetPositionFromCsvColumn(playerPos, csv->GetFloat(n, c), c);
		}
	}
	delete csv;
	return playerPos;
}

bool StageManager::LoadStageObjectPutPointData(const int& _stageNum)
{
#if STAGE_ALL_AREA_PUT

	int indexPosCreateNumber	= 0;	// stageObjectPutPointIndexPosListの生成ナンバー
	int centerPosFirstNum		= 1;	// stageObjectPutPointCenterPositionListの初めのナンバー
	VECTOR3 centerPos			= VZero;// stageObjectPutPointCenterPositionListの中心座標

	stageObjectPutPointCenterPositionList.insert(std::make_pair(centerPosFirstNum, centerPos));

	for (int z = 0;z < gridChip.size();z++)
	{
		for (auto x = 0;x < gridChip[z].size();x++)
		{
			//_ ステージの中心座標から広がるようにインデックスの調整 _//

			float addZ			= ((float)gridChip.size()	 - 1) / 2;					// 設置ポイントのZ軸の要素数
			float addX			= ((float)gridChip[z].size() - 1) / 2;					// 設置ポイントのX軸の要素数
			VECTOR2 indexPos	= VECTOR2((float)x, (float)z) - VECTOR2(addX, addZ);	// ステージの中心座標から広がるようにインデックスを調整

			indexPos			*= StageInfo::PUT_GRID_SIZE;							// 設置ポイントサイズに拡大

			VECTOR3 putPointPos	= VECTOR3(indexPos.x, 0.0f, indexPos.y) + centerPos;	// 設置ポイントの座標
			VECTOR3 rayStartPos = putPointPos + VECTOR3(0.0f, 100000.0f, 0.0f);			// レイの始点
			VECTOR3 rayEndPos	= putPointPos + VECTOR3(0.0f, -100000.0f, 0.0f);		// レイの終点

			// GROUND_BLOCKに対しY軸のレイでの当たり判定をして、当たっていなかったら
			if (!CheckRaycastStageObject(rayStartPos, rayEndPos, std::set<int> {(int)StageObjectData::STAGE_OBJECT_KIND::GROUND_BLOCK}, &putPointPos))
			{
				// 1グリッド分下に座標を設定
				putPointPos.y = -StageInfo::PUT_GRID_SIZE;
			}

			// 地形に埋もれないように、半径分ｙ座標上げる
			putPointPos += VECTOR3(0.0f, StageInfo::BLOCK_SIZE / 2, 0.0f);

			stageObjectPutPointIndexPosList[centerPosFirstNum].emplace_back(PutPointIndexData(indexPosCreateNumber++, putPointPos));
		}
	}
	stageObjectPutPointSelectData.number = 1;

	return true;
#else
	char set[CHAR_MAX];
	std::string file;
	CsvReader* csv = nullptr;

	std::unordered_map<int, std::vector<std::vector<VECTOR2I>>> stageObjectPutPointList;

	for (int i = 1;i <= STAGE_PUT_POINT_MAX;i++)
	{
		//_ ポイントの中心座標の初期化 _//

		sprintfDx(set, "data/stage/objectPutPoints/stage_5_putPoints/putPointCenterPos_%d0%d.csv", _file, i);
		file = set;

		csv = new CsvReader(file);
		VECTOR3 pos;

		for (int n = 0;n < csv->GetLines();n++)
		{
			for (int c = 0;c < csv->GetColumns(n);c++)
			{
				SetVMem(pos, c, csv->GetFloat(n, c));
			}
			stageObjectPutPointCenterPositionList.insert(std::make_pair(i, pos));	// 座標のpush
		}
		delete csv;

		//_ ポイントの配置インデックスの初期化 _//

		sprintfDx(set, "data/stage/objectPutPoints/stage_5_putPoints/putPointIndex_%d0%d.csv", _file,i);
		file = set;
		csv = new CsvReader(file);

		stageObjectPutPointList[i].clear();				// ポイントのナンバーを初期化

		for (int n = 0;n < csv->GetLines();n++)			// 列
		{
			stageObjectPutPointList[i].emplace_back(n);	// 列の確保
			stageObjectPutPointList[i][n].clear();

			for (int c = 0;c < csv->GetColumns(n);c++)	// 行
			{
				// 値が入っていたら
				if (csv->GetInt(n, c) > 0)
				{
					stageObjectPutPointList[i][n].emplace_back(VECTOR2I(c,n));	// 値を代入
				}
			}
		}

		float addZ = ((float)stageObjectPutPointList[i].size() - 1) / 2;
		float addX = ((float)stageObjectPutPointList[i][(int)addZ].size() - 1) / 2;
		VECTOR3 centerPos = stageObjectPutPointCenterPositionList[i];

		stageObjectPutPointIndexPosList[i].clear();		// キーの初期化
		int createNumber = 0;							// 生成ナンバー

		for (int z = 0;z < stageObjectPutPointList[i].size();z++)				
		{
			for (auto itr = stageObjectPutPointList[i][z].begin();itr != stageObjectPutPointList[i][z].end();itr++)
			{
				VECTOR2 indexPos = VECTOR2((float)itr->x, (float)itr->y) - VECTOR2(addX, addZ);
				indexPos *= PUT_GRID_SIZE;

				VECTOR3 pos = VECTOR3(indexPos.x, 0, indexPos.y) + centerPos;

				stageObjectPutPointIndexPosList[i].emplace_back(PutPointIndexData(createNumber++,pos));
			}
		}

		delete csv;
	}
	stageObjectPutPointSelectData.number = 1;
	return true;
#endif
}

VECTOR3 StageManager::GetStageCenterPosition() const
{
	return VZero;
}

bool StageManager::MoveStageObjectPutPointSelect(int _moveNum)
{
	int beforeNumber	= stageObjectPutPointSelectData.number;														// 変更前の設置ポイントナンバー
	int afterNumber		= std::clamp(beforeNumber + _moveNum, 1, (int)stageObjectPutPointIndexPosList.size());		// 変更後の設置ポイントナンバー

	// 選択されている設置ポイントナンバーを変更
	stageObjectPutPointSelectData.number = afterNumber;

	// 変更できていたらtrue変更できていなかったらfalse
	return beforeNumber != afterNumber;
}

// ToDo 生成するステージオブジェクトに識別ナンバーを設定する　DeletePutPointStageObjectで当たり判定を行わずにすむ
bool StageManager::CreatePutPointStageObject(const VECTOR3& _cm, const StageObjectData::STAGE_OBJECT_KIND& _kind, const VECTOR2I& _m)
{
	VECTOR3 hitPos;			// 当たった座標

	// マウス座標をワールドにした座標とカメラ座標でのレイを作って当たり判定を行い、当たっていたら
	if (GetScreenPosToWorldPosRayColl(_m, &hitPos))
	{
		PutPointIndexData* putPointIndexPtr = GetNearPutPointDataPtr(hitPos, true);	// 当たった座標から一番近い、配置ポイントのデータ					

		if (putPointIndexPtr == nullptr)
			return false;	// 当たった座標から一番近い、配置ポイントのデータが見つからなかったので、return

		const Transform& playerTransform = GetPlayerTransform();

		if (!CanPutStageObject(playerTransform, putPointIndexPtr->position))
			return false;	// プレイヤーと重なり、配置ができないとき　return

		const std::list<const StageObjectBase*>& spawnerList = BlockController::GetBlockController()->GetSpawnerList();	// スポナーリスト

		for (const auto& spawner : spawnerList)
		{
			if (!CanPutStageObject(spawner->GetTransform(), putPointIndexPtr->position))
				return false;	// スポナーと重なり、配置ができないとき  return
		}

		return CreatePutPointStageObject(putPointIndexPtr, _kind);
	}
	return false;
}

bool StageManager::CanPutStageObject(const Transform& _targetTransform,const VECTOR3& _putPointIndexPos)
{
	float minDistance		= _targetTransform.GetLenX() / 2 + (StageInfo::BLOCK_SIZE / 2);	// 対象とステージオブジェクトが当たらない、最小距離
	float squareDistance	= VSquareSize(_targetTransform.position - _putPointIndexPos);	// 対象座標と、当たった座標の二乗距離

	return squareDistance > (minDistance * minDistance);
}

bool StageManager::CreatePutPointStageObject(PutPointIndexData* _putPointIndexDataPtr, const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	if (_putPointIndexDataPtr == nullptr)
		return false;	// 置ける範囲外だったら return

	// 配置することができたら
	if (CreateStageObject(_putPointIndexDataPtr->position, _kind) != nullptr)
	{
		// 設置したステージオブジェクトの種類を記録
		_putPointIndexDataPtr->putStageObjectKind = (int)_kind;

		// 配置フラグをtrue
		_putPointIndexDataPtr->isPut = true;

		// ToDo: 再生するサウンドを罠の種類ごとに、変える
		// サウンドの再生  仮でスパイクの設置音を再生　
		SoundManager::PlaySE(Sound_ID::SPIKE_PUT_SE);
		return true;
	}
	return false;
}

bool StageManager::CreatePutPointStageObject(const VECTOR3& _putPos, const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	PutPointIndexData* putPointIndexPtr = GetNearPutPointDataPtr(_putPos, true);

	return CreatePutPointStageObject(putPointIndexPtr, _kind);
}

// ToDo 配置ポイントに生成したブロックのナンバーを見て当たり判定せずに削除する
int StageManager::DeletePutPointStageObject(const VECTOR3& _cm, const VECTOR2I& _m)
{
	VECTOR3 hitPos;

	// マウス座標をワールドにした座標とカメラ座標でのレイを作って当たり判定を行い、当たっていたら
	if (GetScreenPosToWorldPosRayColl(_m, &hitPos))
	{
		PutPointIndexData* putPointIndexPtr = GetNearPutPointDataPtr(hitPos);

		if (putPointIndexPtr == nullptr)
			return -1;	// 範囲外だったら -1を返す

		if (!putPointIndexPtr->isPut)
			return -1;	// 罠が置かれていなかったら -1を返す

		// 不正な配置するポイントナンバーだったら
		if (putPointIndexPtr->number < 0)
			assert(false);

		// トランスフォームの設定 //
		deleteStageObjectCollisionTransform.position	= putPointIndexPtr->position;
		deleteStageObjectCollisionTransform.scale		= VOne;
		deleteStageObjectCollisionTransform.size		= VOne * (StageInfo::BLOCK_SIZE / 2);	// サイズはタレットなどが回転するので小さめにする
	
		// すでにnewしていたら
		if (deleteStageObjectCollision != nullptr)
		{
			// 削除する
			delete deleteStageObjectCollision;
			deleteStageObjectCollision = nullptr;
		}

		// 当たり判定の生成
		deleteStageObjectCollision			= new SphereCollision(&deleteStageObjectCollisionTransform, COLLISION_OBJECT_KIND::ERASER, "AAAA", nullptr, true);

		//_ 当たり判定を行うオブジェクトの設定 _//

		std::set<int> trapCollObjKindCon	= std::move(TrapInfo::GetTrapCollisionObjectKindContainer());	// 罠の当たり判定の種類をコンテナ

		for (const auto& trapColl : trapCollObjKindCon)
		{
			// 罠との当たり判定を設定する
			deleteStageObjectCollision->SetTargetTag((COLLISION_OBJECT_KIND)trapColl);
		}
		// 地面との当たり判定を設定する
		deleteStageObjectCollision->SetTargetTag(COLLISION_OBJECT_KIND::GROUND_BLOCK);

		// putPointのインデックスデータの配置フラグをfalse
		putPointIndexPtr->isPut = false;

		// 削除するオブジェクトの範囲のナビゲーションのNavPointをアクティブ化する
		NavigationClearAreaBox(deleteStageObjectCollisionTransform);

		return putPointIndexPtr->putStageObjectKind;	// 配置したステージオブジェクト種類を返す
	}
	return -1;
}

void StageManager::DrawTrapPutPreview(const VECTOR2I& _mouse, const StageObjectData::STAGE_OBJECT_KIND& _kind, const float& _alpha)
{
	VECTOR3 hitPos;	// 当たった座標

	// マウス座標をワールドにした座標とカメラ座標でのレイを作って当たり判定を行い、当たっていたら
	if (GetScreenPosToWorldPosRayColl(_mouse, &hitPos))
	{
		const PutPointIndexData* putPointIndexPtr = GetNearPutPointDataPtr(hitPos, true);

		// 設置できる範囲外だったら
		if (putPointIndexPtr == nullptr)
			return;	// 描画処理を行わない

		// ライトの影響をOFF
		SetUseLighting(false);

		for (int z = -1;z < 2;z++)
		{
			for (int x = -1;x < 2;x++)
			{
				VECTOR3 startPos	= putPointIndexPtr->position - VOne * StageInfo::BLOCK_SIZE / 2.0f;	// BOX描画の始点 
				VECTOR3 goalPos		= putPointIndexPtr->position + VOne * StageInfo::BLOCK_SIZE / 2.0f;	// BOX描画の終点

				// Yの高さを上げて地面に埋もれるのを防ぐ
				startPos.y	+= 1.0f;
				// Yの高さを下げる 完全に下げてしまうと、地面に埋もれしまうので、5.0fで調整
				goalPos.y	-= StageInfo::BLOCK_SIZE - 5.0f;

				// インデクスに応じて、座標を変更 //
				startPos.x	+= StageInfo::BLOCK_SIZE * x;
				startPos.z	+= StageInfo::BLOCK_SIZE * z;

				goalPos.x	+= StageInfo::BLOCK_SIZE * x;
				goalPos.z	+= StageInfo::BLOCK_SIZE * z;

				// BOX状の枠を描画
				DrawCube3D(startPos, goalPos, 0xffffff, 0xffffff, false);
			}
		}

		// ライトの影響をON
		SetUseLighting(true);

		Transform trans;	
		// 座標を設定
		trans.position = putPointIndexPtr->position;

		// スケールとサイズを設定
		StageObjectData::SetScaleAndSize(trans, _kind);

		// 配置するオブジェクトがブロックだったら
		if (StageObjectData::IsBlockStageObjectKind(_kind))
		{
			// 罠の半透明描画
			BlockController::GetBlockController()->DrawTrapPutPreview(trans, _kind, _alpha);
		}
		// 配置するオブジェクトがエンティティだったら
		else if (StageObjectData::IsEntityStageObjectKind(_kind))
		{
			// 罠の半透明描画
			EntityController::GetEntityController()->DrawTrapPutPreview(trans, _kind, _alpha);
		}

		float impactRadius		= TrapInfo::GetViewImpactRadius(_kind, 1);	// レベル１の罠の影響範囲
		float impactDrawSubYPos	= 20.0f;	// 影響範囲のY座標の調整値

		// 罠の影響範囲が設定されていなかったら
		if (impactRadius < 0.0f)
			return;	// 影響範囲を描画しない

		// モデルの底面に描画するので下げる 下げすぎないように調整
		trans.position.y -= trans.GetLenY() / 2 - impactDrawSubYPos;

		// 罠の影響範囲を描画
		DrawTrapImpactRadius(trans.position, impactRadius);
	}
}

bool StageManager::PutStageObject(const VECTOR3& _pos, const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	return CreateStageObject(_pos, _kind) != nullptr;
}

bool StageManager::PutStageObject(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind)
{
	return CreateStageObject(_trans, _kind) != nullptr;
}

const StageObjectBase* StageManager::PutSpawner(const VECTOR3& _pos)
{
	// スポナーの生成
	return CreateStageObject(_pos, StageObjectData::STAGE_OBJECT_KIND::SPAWNER_BLOCK, false);
}

bool StageManager::DeleteSpawner(StageObjectBase* _spawner)
{
	assert(_spawner->GetStageObjectKind() == StageObjectData::STAGE_OBJECT_KIND::SPAWNER_BLOCK);

	// スポナーの削除
	return DeleteStageObject(_spawner);
}

void StageManager::DeleteAllSpawner()
{
	// 全てのスポナーの削除
	BlockController::GetBlockController()->DeleteBlock(StageObjectData::STAGE_OBJECT_KIND::SPAWNER_BLOCK);
}

StageObjectBase* StageManager::CreateStageObject(const VECTOR3& _pos, const StageObjectData::STAGE_OBJECT_KIND& _kind, bool _isYPosAdjustment)
{
	Transform transStageObjTransform;	// ステージオブジェクトのトランスフォーム
	// トランスフォームのサイズとスケールの設定
	StageObjectData::SetScaleAndSize(transStageObjTransform, _kind);
	// トランスフォームの座標の代入
	transStageObjTransform.position	= _pos;
	
	// ステージオブジェクトの生成
	return CreateStageObject(transStageObjTransform, _kind, _isYPosAdjustment);
}

StageObjectBase* StageManager::CreateStageObject(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, bool _isYPosAdjustment)
{
	Transform stageObjTransform = _trans;	// ステージオブジェクトのトランスフォーム

	// Y座標の調整をするとき
	if (_isYPosAdjustment)
	{
		// 地面にモデルの底面がつくように補正
		stageObjTransform.position.y -= (StageInfo::PUT_GRID_SIZE / 2) - (stageObjTransform.GetLenY() / 2);
	}

	// 配置するオブジェクトがブロックだったら
	if (StageObjectData::IsBlockStageObjectKind(_kind))
	{
		StageObjectBase* putStageObject = BlockController::GetBlockController()->CreateBlock(stageObjTransform, _kind, 0);	// 設置したブロックのトランスフォームポインタ

		if (putStageObject == nullptr)
			return nullptr;	// ブロックが設置されなかったとき　

		// ナビゲーションに変化を及ぼすオブジェクトなら
		if (IsStageObjectToNavigationNodeChange(_kind))
		{
			// ステージのナビゲーションをトランスフォーム範囲で更新する
			NavigationPutAreaBox(putStageObject->GetTransfromPtr(), StageObjectData::GetStageObjectHPConfig(_kind));
		}
		return putStageObject;	// 全ての処理が終わったのでStageObjectBaseポインタを返す
	}
	// 配置するオブジェクトがエンティティだったら
	else if (StageObjectData::IsEntityStageObjectKind(_kind))
	{
		StageObjectBase* putStageObject = EntityController::GetEntityController()->SummonEntity(stageObjTransform, _kind);	// 設置したエンティティのトランスフォームポインタ

		if (putStageObject == nullptr)
			return nullptr;	// エンティティが設置されなかったとき falseを返す

		// ナビゲーションに変化を及ぼすオブジェクトなら
		if (IsStageObjectToNavigationNodeChange(_kind))
		{
			// ステージのナビゲーションをトランスフォーム範囲で更新する
			NavigationClearAreaBox(putStageObject->GetTransform());
		}
		return putStageObject;	// 全ての処理が終わったのでStageObjectBaseポインタを返す
	}
	else
	{
		assert(false);
	}
	return nullptr;
}

bool StageManager::DeleteStageObject(StageObjectBase* _deleteStageObject)
{
	// ステージオブジェクトのTYPEがBlockのとき
	if (StageObjectData::IsBlockStageObjectKind(_deleteStageObject->GetStageObjectKind()))
	{
		// BlockControllerのコンテナ内から削除
		return BlockController::GetBlockController()->DeleteBlock(static_cast<BlockBase*>(_deleteStageObject));
	}
	else if (StageObjectData::IsEntityStageObjectKind(_deleteStageObject->GetStageObjectKind()))
	{
		// EntityControllerのコンテナ内から削除
		return EntityController::GetEntityController()->DeleteEntity(static_cast<EntityBase*>(_deleteStageObject));
	}
	else
	{
		assert(false);
	}
	return false;
}

bool StageManager::GetScreenPosToWorldPosRayColl(const VECTOR2I& _musePos, VECTOR3* _hitPos)
{
	// マウスポインタがある画面上の座標に該当する３Ｄ空間上の Near 面の座標を取得
	VECTOR3 startPos	= ConvScreenPosToWorldPos(VGet((float)_musePos.x, (float)_musePos.y, 0.0f));

	// マウスポインタがある画面上の座標に該当する３Ｄ空間上の Far 面の座標を取得
	VECTOR3 endPos		= ConvScreenPosToWorldPos(VGet((float)_musePos.x, (float)_musePos.y, 1.0f));

	std::set<int> collTargetCon = TrapInfo::GetTrapStageObjectKindContainer();	// 当たり判定を行うステージオブジェクトコンテナ
	collTargetCon.emplace((int)StageObjectData::STAGE_OBJECT_KIND::GROUND_BLOCK);

	// マウス座標をワールドにした座標とカメラ座標でのレイを作って当たり判定を行う
	return CheckRaycastStageObject(startPos, endPos, collTargetCon, _hitPos);
}

StageManager::PutPointIndexData* StageManager::GetNearPutPointDataPtr(const VECTOR3& _pos, bool _putedReturnNull)
{
	float minHitSquareDistance					= FLT_MAX;								// 当たった時の点とステージオブジェクトを設置するポイントのBOX範囲との最小二乗距離
	int number									= stageObjectPutPointSelectData.number;	// 選択されている配置するポイントのナンバー
	PutPointIndexData* nearPutPointIndexData	= nullptr;								// 一番近くの設置するポイントの個々の配置インデックスのデータ

	if (stageObjectPutPointIndexPosList.find(number) == stageObjectPutPointIndexPosList.end())
		assert(false);

	for (auto itr = stageObjectPutPointIndexPosList[number].begin();itr != stageObjectPutPointIndexPosList[number].end();itr++)
	{
		if (_putedReturnNull)
		{
			if (itr->isPut)
				continue;	// ステージオブジェクトを配置していたら continue
		}

		VECTOR3 obbLen					= VOne * (StageInfo::BLOCK_SIZE);									// BOXの直径
		constexpr float POINT_RUDIUS	= 25.0f;															// 点を直径50.0fとしたときの半径
		VECTOR3 obbToPointVec			= OBBToSphereVec(itr->position, obbLen, VZero, _pos, POINT_RUDIUS);	// BOXと点の最短ベクトル

		float hitSquareDistance			= obbToPointVec.SquareSize();	// 点とBOXの距離を２乗した値

		if (hitSquareDistance > (POINT_RUDIUS * POINT_RUDIUS))
			continue;	// 点がBOXに当たっていなかったら continue

		if (minHitSquareDistance > hitSquareDistance)
		{
			minHitSquareDistance	= hitSquareDistance;
			nearPutPointIndexData	= &*itr;
		}
	}
	return nearPutPointIndexData;
}

void StageManager::NavigationPutAreaBox(const Transform* _trans, const int& _hp)
{
	Transform trans		= *_trans;

	VECTOR3	halfLen		= VDivI(VMult(trans.size, trans.scale), 2);	// ブロックの半径
	VECTOR3 startPos	= trans.position - halfLen;					// 始めの座標
	VECTOR3 endPos		= trans.position + halfLen;					// 終わりの座標

	//_ 罠を設置するオブジェクトの範囲のナビゲーションのNavPointを非アクティブ化する _//

	// ナビゲーションマネージャーにブロックを設置した範囲を渡す
	int areaBoxNumber	= navigationManager->StageObjectPutAreaBox(startPos, endPos, _hp);

	navigationAreaBoxNumberList.insert(std::make_pair(_trans, areaBoxNumber));
}

void StageManager::NavigationClearAreaBox(const Transform& _trans)
{
	VECTOR3	halfLen		= VDivI(VMult(_trans.size, _trans.scale), 2);	// ブロックの半径
	VECTOR3 startPos	= _trans.position - halfLen;					// 始めの座標
	VECTOR3 endPos		= _trans.position + halfLen;					// 終わりの座標

	//_ 罠を削除するオブジェクトの範囲のナビゲーションのNavPointをアクティブ化する _//

	// ナビゲーションマネージャーにオブジェクトを削除する範囲を渡す
	navigationManager->StageObjectClearAreaBox(startPos, endPos);
}

void StageManager::NavigationAreaBoxHPChange(const Transform* _trans, const int& _hp)
{
	if (navigationAreaBoxNumberList.find(_trans) == navigationAreaBoxNumberList.end())
		return;

	// ナビゲーションのエリアボクセルのHPを変更
	navigationManager->VoxelNavNodeHPChange(navigationAreaBoxNumberList[_trans], _hp);
}

void StageManager::DeleteNavigationAreaBoxTransformPtr(const Transform* _trans)
{
	auto itr = navigationAreaBoxNumberList.find(_trans);

	// キーが登録されていなかったら
	if (itr == navigationAreaBoxNumberList.end())
		return;

	itr = navigationAreaBoxNumberList.erase(itr);
}

const std::unordered_map<int, int> StageManager::GetRawModelHandles()
{
	std::unordered_map<int, int> re;
	const std::unordered_map<int, std::vector<int>>& rawModels = BlockController::GetBlockController()->GetRawModelHandles();

	//_ 罠のモデルはレベル１のモデルハンドルを返す _//

	for (auto& rawModel : rawModels)
	{
		// 罠のレベル１モデル、通常オブジェクトモデルを代入
		re.insert(std::make_pair(rawModel.first, *rawModel.second.begin()));
	}
	return re;
}

bool StageManager::PushRequestInterfaceTrapList(StageObjectBase* _stageObj)
{
	// 引数のステージオブジェクトが罠に分類されなかったら
	if (!TrapInfo::IsTrapStageObjectKind(_stageObj->GetStageObjectKind()))
		return false;

	ITrap* iTrap = dynamic_cast<ITrap*>(_stageObj);

	if (iTrap == nullptr)
		return false;
	return interfaceTrapList.insert(std::make_pair(_stageObj, iTrap)).second;
}

void StageManager::DeleteInterfaceTrapList(StageObjectBase* _stageObj)
{
	std::unordered_map<const StageObjectBase*, ITrap*>::iterator trapItr = interfaceTrapList.find(_stageObj);

	if (trapItr == interfaceTrapList.end())
		return;	// キーが見つからなかったら return

	// コンテナから削除
	trapItr = interfaceTrapList.erase(trapItr);
}

const ITrap* StageManager::GetNearTrap(const VECTOR3& _pos,const float& _max)
{
	float minDistance		= FLT_MAX;	// 最小距離
	float squareDistance	= FLT_MAX;	// 引数座標との距離
	const ITrap* nearTrap	= nullptr;	// 一番近いITrapポインタ

	for (const auto& trap : interfaceTrapList)
	{
		const VECTOR3& trapPos	= trap.second->GetTransform().position;
		squareDistance			= VSquareSize(trapPos - _pos); // お互いの距離

		if ((_max * _max) < squareDistance)
			continue;	// 引数距離外だったらcontinue

		if (minDistance > squareDistance)
		{
			minDistance = squareDistance;
			nearTrap	= trap.second;
		}
	}
	return nearTrap;
}

void StageManager::SetTransformToCsvColumn(Transform& _trans, const float& _num, const int& _col)
{
	MEMBER m = (MEMBER)_col;

	if (_col / 3 == 0)		// 1 - 3 列目
	{
		SetVMem(_trans.position, _col % 3, _num);
	}
	else if (_col / 3 == 1)	// 4 - 6 列目
	{
		SetVMem(_trans.rotation, _col % 3, _num);
	}
	else					// 7 - 9 列目
	{
		SetVMem(_trans.scale, _col % 3, _num);
	}
}

void StageManager::SetPositionFromCsvColumn(VECTOR3& _pos, const float& _num, const int& _col)
{
	SetVMem(_pos, _col % 3, _num);
}

const std::list<BlockBase*> StageManager::GetAllBlockObject()
{
	return BlockController::GetBlockController()->GetAllBlockObject();
}

const StageObjectBase* StageManager::RaycastGetTrap(const VECTOR3& _startPos, const VECTOR3& _endPos)
{
	// 罠のステージオブジェクトが存在しなかったら
	if (interfaceTrapList.empty())
		return nullptr;

	return RaycastStageObject(_startPos, _endPos, TrapInfo::GetTrapStageObjectKindContainer(), nullptr);;
}

std::list<StageObjectBase*> StageManager::GetStageObjectContainer()
{
	std::list<StageObjectBase*> stageObjList	= BlockController::GetBlockController()->GetAllStageObject();
	std::list<StageObjectBase*> enStageObj		= EntityController::GetEntityController()->GetAllStageObject();

	for (auto itr = enStageObj.begin();itr != enStageObj.end();itr++)
	{
		stageObjList.emplace_back(*itr);
	}
	return stageObjList;
}

const std::list<StageObjectBase*> StageManager::GetAllStageObject()
{
	return GetStageObjectContainer();
}

StageObjectBase* StageManager::RaycastStageObject(const VECTOR3& _startPos, const VECTOR3& _endPos, const std::set<int>& _kindList, VECTOR3* _hitPos)
{
	StageObjectBase* hitStageObject			= nullptr;		// 当たったステージオブジェクトポインタ										
	std::list<StageObjectBase*> stageObjCon = GetAllStageObject();
	
	VECTOR3 hitPos = VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);	// 当たった座標で一番_pos1（レイの始点）に近かった値を設定するために、一度大きな値を代入


	for (const auto& stageObj : stageObjCon)
	{
		if (!_kindList.empty())
		{
			if (_kindList.find((int)stageObj->GetStageObjectKind()) == _kindList.end())
				continue;	// 当たり判定するステージオブジェクトの種類がコンテナ内に、存在しなかったら continue
		}

		int bModel = stageObj->GetHmodel();

		assert(bModel > 0);

		// モデルに座標を反映	ToDo:毎回ここで当たり判定情報を更新するのをやめる
		MV1SetMatrix(bModel, stageObj->GetTransform().GetMatrix());
		MV1RefreshCollInfo(bModel);

		// 当たり判定結果を取得
		MV1_COLL_RESULT_POLY ret = MV1CollCheck_Line(bModel, -1, _startPos, _endPos);

		//debugRayList.emplace_back(std::make_pair(_startPos, _endPos));

		// 当たったかどうか
		if (ret.HitFlag != 0)
		{
			// より近い座標での当たった座標だったら
			if (VSquareSize(hitPos - _startPos) > VSquareSize(ret.HitPosition - _startPos))
			{
				// 当たった座標の代入 //
				hitPos = ret.HitPosition;

				if (_hitPos != nullptr)
				{
					*_hitPos = hitPos;
				}

				// 当たったステージオブジェクトを記録
				hitStageObject = stageObj;
			}
		}
	}
	return hitStageObject;
}

bool StageManager::CheckRaycastStageObject(const VECTOR3& _startPos, const VECTOR3& _endPos, const std::set<int>& _kindList, VECTOR3* _hitPos)
{
	return RaycastStageObject(_startPos, _endPos, _kindList, _hitPos) != nullptr;
}

const StageObjectBase* StageManager::RaycastGetCanMaintainTrap(const VECTOR3& _startPos, const VECTOR3& _endPos)
{
	const StageObjectBase* stageObj = RaycastGetTrap(_startPos, _endPos);

	// コンテナに存在しなかったら
	if (!HasUnorderedMapContainerKey(interfaceTrapList, stageObj))
		return nullptr;

	ITrap* iTrap = interfaceTrapList[stageObj];
	assert(iTrap != nullptr);

	// メンテナンスできないとき
	if (!iTrap->CanMaintain())
		return nullptr;

	return stageObj;
}

float StageManager::TrapMaintaining(const StageObjectBase* _stageObj, const float& _add)
{
	// コンテナに存在しなかったら
	if (!HasUnorderedMapContainerKey(interfaceTrapList, _stageObj))
		return -1.0f;

	ITrap* iTrap = interfaceTrapList[_stageObj];
	assert(iTrap != nullptr);

	// メンテナンスを進める
	return iTrap->Maintaining(_add);
}

const StageObjectBase* StageManager::RaycastGetCanUpgradTrap(const VECTOR3& _pos1, const VECTOR3& _pos2)
{
	const StageObjectBase* stageObj = RaycastGetTrap(_pos1, _pos2);

	// コンテナに存在しなかったら
	if (!HasUnorderedMapContainerKey(interfaceTrapList, stageObj))
		return nullptr;

	ITrap* iTrap = interfaceTrapList[stageObj];
	assert(iTrap != nullptr);

	// アップグレードできないとき
	if (!iTrap->CanUpgrade())
		return nullptr;

	return stageObj;
}

float StageManager::TrapUpgrade(const StageObjectBase* _stageObj, const float& _add)
{
	// コンテナに存在しなかったら
	if (!HasUnorderedMapContainerKey(interfaceTrapList,_stageObj))
		return -1.0f;

	ITrap* iTrap = interfaceTrapList[_stageObj];
	assert(iTrap != nullptr);

	// アップグレードを進める
	return iTrap->Upgrading(_add);
}

int StageManager::GetTrapLevel(const StageObjectBase* _stageObj)
{
	// コンテナに存在しなかったら
	if (!HasUnorderedMapContainerKey(interfaceTrapList, _stageObj))
		return -1;

	ITrap* iTrap = interfaceTrapList[_stageObj];
	assert(iTrap != nullptr);

	return iTrap->GetNowLevel();
}

void StageManager::SendDeletePutPointStageObject(const Transform& _trans)
{
	PutPointIndexData* putData = GetNearPutPointDataPtr(_trans.position);	// 引数のトランスフォームに対し、近くの設置された、PutPointIndexDataポインタ

	if (putData == nullptr)
		return;	// 引数のトランスフォームに対し、近くの設置された、PutPointIndexDataポインタを取得できなかったら return

	// 罠が削除される範囲の情報を送りナビゲーションの設置をする
	NavigationClearAreaBox(_trans);

	// 罠が削除されたので置かれているフラグをfalse
	putData->isPut = false;
}

std::vector<VECTOR3> StageManager::GetStageObjectPutPointList() const
{
	std::vector<VECTOR3> re;

	for (const auto& itr : stageObjectPutPointCenterPositionList)
	{
		re.emplace_back(itr.second);
	}
	return re;
}

const VECTOR3* StageManager::GetCorePosition() const
{
	return BlockController::GetBlockController()->GetCorePosition();
}

bool StageManager::IsCoreBroken()
{
	return BlockController::GetBlockController()->IsCoreBroken();
}

const Transform& StageManager::GetNearEnemyTransform(const VECTOR3& _pos) const
{
	return enemyManager->GetNearEnemyTransform(_pos);
}

const VECTOR3& StageManager::GetNearEnemyPosition(const VECTOR3& _pos)
{
	return enemyManager->GetNearEnemyPosition(_pos);
}

const VECTOR3& StageManager::GetEnemyPosition(const int& _eneNumber)
{
	return enemyManager->GetEnemyPosition(_eneNumber);
}

bool StageManager::IsEnemySummon() const
{
	return enemyManager->IsEnemySummon();
}