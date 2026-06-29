#include "NavMeshController.h"
#include "PortalPoint.h"
#include "../../NavigationManager/NavigationManager.h"
#include "../../../Stage/StageManager/StageManager.h"
#include "../../../Player/Player.h"
#include "../../../Stage/StageObjects/Block/BlockBase.h"
#include "../../../Transform/Transform.h"
#include "../../../Common/CsvReader/CsvReader.h"
#include "../../../Collision/CollisionFunction.h"
#include "../../../Common/Debug/Debug.h"
#include "../../../Common/MeshInfo/MeshInfoFactory.h"
#include "../../../../ImGui/imgui.h"
#include <queue>
#include <iostream>	// std::coutを使うため
#include <fstream>	// std::ifstreamを使うため
#include "../../../../Library/DebugNew.h"

#define DEBUG_INFO_COUNT_OUT (1)				// debugInfoCountListでの出力を行うかどうか

#define DO_BUG_CONNECT_ME_MODEL_REVISION (0)	// モデルのポリゴンからナビメッシュを構築する際に、ポータルポイントの接続で接続してはいけないところに、接続してしまうバグ修正のソースを走られるかどうかのマクロ / (1 走らせる,0 走らせない) ※現在バグあり

#define IS_MESH_CREATE_THIS_CLASS (0)			// このクラス内で、メッシュ情報を構築するかどうか

namespace
{
	const float WORLD_VERTEX_DISTANCE_EPSILON_NUM = 50.0f;				// 頂点同士の距離が近い場合に一つの頂点とする距離

	const float PLAYER_FOR_NAVMESH_DRAW_DISTANCE = 5000.0f;				// プレイヤーの座標が近かったらナビメッシュの情報を描画する距離

	constexpr float OTHER_MODEL_PORTALPOINT_CONNECT_DISTANCE = 100.0f;		// 他モデルとのポータルポイントの接続を行う距離

	const float WALL_SET_NORM_ANGLE = 60.0f * DegToRad;		// 壁と判定するPortalPointのポリゴン法線平均角度

	/// <summary>
	/// navMeshデータ
	/// </summary>
	enum NAVMESH_DATA
	{
		// ポータルナンバー
		NUMBER = 0,

		// ポータルセンター座標
		P_X,
		P_Y,
		P_Z,

		// ポータルを構築する頂点データ
		F_NUM,	// 頂点ナンバーA　一番　 (小さい数字)
		S_NUM,	// 頂点ナンバーB  二番	 (大きい数字)

		// 法線ベクトル 今後追加
		N_X,
		N_Y,
		N_Z,

		// 生成元のモデルナンバー
		OWNER_MODEL_NUM,

		// 生成元のモデルのポリゴンの最大座標
		OWNER_MODEL_POL_MAX_P_X,
		OWNER_MODEL_POL_MAX_P_Y,
		OWNER_MODEL_POL_MAX_P_Z,

		MAX,
	};
}

namespace std
{
	template<>

	struct hash<VertexData>
	{
		//_ VertexDataのnumberを使ってハッシュ値を定義 _//
		size_t operator()(const VertexData& v) const noexcept
		{
			return std::hash<int>()(v.number);
		}
	};

	//struct hash<OwnerModelData>
	//{
	//	//_ OwnerModelDataのnumberを使ってハッシュ値を定義 _//
	//	size_t operator()(const OwnerModelData& own) const noexcept
	//	{
	//		return std::hash<int>()(own.number);
	//	}
	//};
}

int NavMeshController::polygonCreateNum = 0;
int NavMeshController::vertexCreateNum = 0;
int NavMeshController::portalPointCreateNum = 0;
int NavMeshController::ownerModelSaveNum = 0;

NavMeshController::NavMeshController()
{
	SetTag("NavMeshController");

	modelForPolygonDataList.clear();
	modelForLocalVerticesAndTargetDataList.clear();
	worldPoygonDataList.clear();
	worldVerticesDataList.clear();
	activePortalPointList.clear();
	inactivePortalPointList.clear();
	//modelEndPortalPointList.clear();

	debugInfoCountList.clear();

	stageManager	= nullptr;
	player			= nullptr;

	isFrameDraw				= false;
	isPortalPointRootDraw	= false;
}

NavMeshController::~NavMeshController()
{
	modelForPolygonDataList.clear();

	for (auto itr = worldVerticesDataList.begin();itr != worldVerticesDataList.end();)
	{
		VertexData* v = itr->second;

		if (v != nullptr)
		{
			delete v;
			v = nullptr;
		}
		itr = worldVerticesDataList.erase(itr);
	}

	for (auto itr = modelForLocalVerticesAndTargetDataList.begin();itr != modelForLocalVerticesAndTargetDataList.end();)
	{
		for (auto VItr = itr->second.begin();VItr != itr->second.end();VItr++)
		{
			std::unordered_set<VertexData, VertexDataHash>& trList = VItr->second;

			// worldVerticesDataListで頂点情報を削除しているので、deleteしない
			trList.clear();
		}
		// worldVerticesDataListで頂点情報を削除しているので、deleteしない
		itr->second.clear();
		itr = modelForLocalVerticesAndTargetDataList.erase(itr);
	}

	for (auto itr = worldPoygonDataList.begin();itr != worldPoygonDataList.end();)
	{
		PolygonData* p = itr->second;

		if (p != nullptr)
		{
			delete p;
			p = nullptr;
		}
		itr = worldPoygonDataList.erase(itr);
	}

	for (auto itr = activePortalPointList.begin();itr != activePortalPointList.end();)
	{
		PortalPoint* por = itr->second;

		if (por != nullptr)
		{
			por->DestroyMe();
			por = nullptr;
		}
		itr = activePortalPointList.erase(itr);
	}

	for (auto itr = inactivePortalPointList.begin();itr != inactivePortalPointList.end();)
	{
		PortalPoint* por = itr->second;

		if (por != nullptr)
		{
			por->DestroyMe();
			por = nullptr;
		}
		itr = inactivePortalPointList.erase(itr);
	}

	/*for (auto itr = modelEndPortalPointList.begin();itr != modelEndPortalPointList.end();)
	{
		PortalPoint* por = *itr;

		if (por != nullptr)
		{
			por->DestroyMe();
			por = nullptr;
		}
		itr = modelEndPortalPointList.erase(itr);
	}*/

	debugInfoCountList.clear();

	NavMeshController::polygonCreateNum		= 0;
	NavMeshController::vertexCreateNum		= 0;
	NavMeshController::portalPointCreateNum = 0;
	NavMeshController::ownerModelSaveNum	= 0;
}

void NavMeshController::ModelForPortalPointInit(bool _doNavSetting)
{
	if (_doNavSetting)
	{
		// ポリゴン情報からポータルを生成
		PolygonForNavMeshDataSetting();
	}
	else
	{
		// ハンドルをもとに、ポリゴン情報を取得
		for (int i = 0;i < (int)StageObjectData::GetStageObjectKindTypeMax(StageObjectData::TYPE::BLOCK);i++)
		{
			if (i == (int)StageObjectData::STAGE_OBJECT_KIND::SWAMP_BLOCK)
				continue;	// ナビメッシュを構築しないモデルだったら

			MeshInfoFactory* meshInfo = new MeshInfoFactory(hModel[i], false);	// メッシュ情報は生成せずに、モデルのポリゴン情報だけ構築 

			// モデルの全てのポリゴンデータを代入
			modelForPolygonDataList.insert(std::make_pair(i, meshInfo->GetModelPolygonListData()));

			delete meshInfo;
		}

		int loadFileNum = stageManager->GetLoadStageNum();	// ロードするナビポイントのファイルナンバー

		// 地面のモデルのnavPointをファイルからポータルポイントコンテナに初期化
		LoadNavPoint(loadFileNum);

		// 地面のモデルのnavPointの接続相手をファイルからポータルに代入
		LoadTargetNavPoint(loadFileNum);
	}
}

void NavMeshController::Init(Player* _player, StageManager* _stageManager, bool _doNavSetting)
{
	assert(_player != nullptr && _stageManager != nullptr);

	player			= _player;
	stageManager	= _stageManager;

	const std::unordered_map<int, int> handls = stageManager->GetRawModelHandles();
	hModel.resize(handls.size());

	for (const auto& handl : handls)
	{
		hModel[handl.first] = handl.second;
	}

	/*for (int i = 0;i < (int)StageObjectData::GetStageObjectKindTypeMax(StageObjectData::TYPE::BLOCK);i++)
	{
		char set[CHAR_MAX];
		sprintfDx(set, "data/models/stageObject/blocks/block_%.2d.mv1", i);
		hModel[i] = MV1LoadModel(set);
		assert(hModel[i] >= 0);
	}
	*/
	ModelForPortalPointInit(_doNavSetting);
}

bool NavMeshController::LoadNavPoint(const int& _file)
{
	char set[CHAR_MAX];
	sprintfDx(set, "data/navPoint/navPoint_stage_%.2d.csv", _file);
	std::string file;
	file = set;

	CsvReader* csv = new CsvReader(file);

	const std::list<BlockBase*> blocks = stageManager->GetAllBlockObject();		// ステージのブロックのコンテナ

	int number			= -1;													// ポータルナンバー
	VECTOR3 pos			= VZero;												// 座標
	int mem				= 0;													// NavPointでSetVMemで座標を初期化するための引数
	int diff			= abs((int)NAVMESH_DATA::P_Z) - 2;						// NavPointでSetVMem関数の引数の最大値とenumでのZの値の差　enumの条件分岐での初期化を最適化するため
	int ownModelMem		= 0;													// 生成元のモデルデータのSetVMemで座標を初期化するための引数
	int ownModelDiff	= abs((int)NAVMESH_DATA::OWNER_MODEL_POL_MAX_P_Z) - 2;	// 生成元のモデルデータのSetVMem関数の引数の最大値とenumでのZの値の差　enumの条件分岐での初期化を最適化するため
	int ownerModelNum	= -1;													// 生成元のモデルデータのナンバー

	std::unordered_map<int, OwnerModelData> ownerModelList;						// オーナーモデルのデータを記録　すでにこのコンテナにデータを記録しているなら、コンテナの値を初期化でつかうことで、処理負荷を軽減
	PortalData portalData;														// ポータルデータ
	OwnerModelData ownerModel;													// 初期化用生成元のモデル構造体


	for (int n = 0;n < csv->GetLines();n++)
	{
		bool doInitOwnerModelData = false;		// 生成元のモデルデータをownerModelListに初期化してあるかのフラグ

		for (int c = 0;c < csv->GetColumns(n);c++)
		{
			// csvからの値を変数に代入
			switch (c)
			{
			case NAVMESH_DATA::NUMBER:

				number = csv->GetInt(n, c);
				break;
			case NAVMESH_DATA::P_X:
			case NAVMESH_DATA::P_Y:
			case NAVMESH_DATA::P_Z:

				// diff(差の値)を考慮し引数の0,1,2の範囲にする
				mem = abs(c) - diff;
				SetVMem(pos, mem, csv->GetFloat(n, c));
				break;
			case NAVMESH_DATA::F_NUM:

				portalData.numberA = csv->GetInt(n, c);
				break;
			case NAVMESH_DATA::S_NUM:

				portalData.numberB = csv->GetInt(n, c);
				break;
			case NAVMESH_DATA::N_X:

				portalData.averageNorm.x = csv->GetFloat(n, c);
				break;
			case NAVMESH_DATA::N_Y:

				portalData.averageNorm.y = csv->GetFloat(n, c);
				break;
			case NAVMESH_DATA::N_Z:

				portalData.averageNorm.z = csv->GetFloat(n, c);
				break;
			case NAVMESH_DATA::OWNER_MODEL_NUM:

				ownerModelNum = csv->GetInt(n, c);

				if (ownerModelList.find(ownerModelNum) != ownerModelList.end())
				{
					doInitOwnerModelData = true;
					continue;	// すでにナンバーが登録されていたので、continue
				}
				ownerModelList[ownerModelNum].number = ownerModelNum;
				break;
			case NAVMESH_DATA::OWNER_MODEL_POL_MAX_P_X:
			case NAVMESH_DATA::OWNER_MODEL_POL_MAX_P_Y:
			case NAVMESH_DATA::OWNER_MODEL_POL_MAX_P_Z:

				ownModelMem = abs(c) - ownModelDiff;	// diff(差の値)を考慮し引数の0,1,2の範囲にする
				SetVMem(ownerModelList[ownerModelNum].position, ownModelMem, csv->GetFloat(n, c));
				break;
			default:
				assert(false);
				break;
			}
		}

		// 生成元のモデルのポインタが設定されていなかったら
		if (ownerModelList[ownerModelNum].pointer == nullptr)
		{
			//_ 生成元モデルのインスタンスのポインター設定 _//

			for (auto blockItr = blocks.begin();blockItr != blocks.end();blockItr++)
			{
				BlockBase* blockPtr = *blockItr;
				int blockKind		= (int)blockPtr->GetStageObjectKind();

				// キーが存在しなかったら、assert
				HasUnorderedMapContainerKey(modelForPolygonDataList, blockKind, true);

				for (auto oItr = ownerModelList.begin();oItr != ownerModelList.end();oItr++)
				{
					MV1_REF_POLYGONLIST& modelPolygonListData	= modelForPolygonDataList[blockKind];
					VECTOR3 maxPos								= modelPolygonListData.MaxPosition * blockPtr->GetTransform().GetMatrix();
					OwnerModelData& ownerModelData				= oItr->second;

					//_ 同じモデルを指しているかの判定 _//

					// ポリゴンの最大座標が同じだったら
					if (VAllSame(maxPos, ownerModelData.position))
					{
						// モデルのインスタンスのポインタを記録する
						ownerModelData.pointer = blockPtr;
					}
				}
			}
		}

		// ポータルポイントをコンテナに追加
		activePortalPointList.insert(std::make_pair(number, new PortalPoint(number, pos, portalData, ownerModelList[ownerModelNum])));
	}
	NavMeshController::portalPointCreateNum = number;
	delete csv;
	return false;
}

bool NavMeshController::LoadTargetNavPoint(const int& _file)
{
	char set[CHAR_MAX];
	sprintfDx(set, "data/navPoint/navPointTarget_stage_%.2d.csv", _file);
	std::string file;
	file = set;

	CsvReader* csv = new CsvReader(file);

	int number = -1;
	std::list<int> saveTr; // 接続相手を保存
	saveTr.clear();

	for (int n = 0;n < csv->GetLines();n++)
	{
		for (int c = 0;c < csv->GetColumns(n);c++)
		{
			// csvからの値を変数に代入
			if (c == 0)	// ポータルポイントナンバー
				number = csv->GetInt(n, c);
			else		// ポータルポイント接続相手ナンバー
				saveTr.emplace_back(csv->GetInt(n, c));
		}

		// ポータルポイントが追加されていなかった時
		if (activePortalPointList.find(number) == activePortalPointList.end())
			assert(false);

		// ポータルポイントに接続相手を追加
		for (auto itr = saveTr.begin();itr != saveTr.end();itr++)
			activePortalPointList[number]->PushInitTargetPortalPoint(activePortalPointList[*itr]);

		saveTr.clear();
	}
	delete csv;
	return false;
}

void NavMeshController::SaveNavPoint(const int& _file)
{
	char set[CHAR_MAX];
	sprintfDx(set, "data/navPoint/navPoint_stage_%.2d.csv", _file);
	std::string filePath;
	filePath = set;

	std::ofstream file(filePath);	// これでパスの通りにcsvファイルが作成される
	std::list<std::string> data;	// ファイルに書き出す情報を行ごとにまとめる string出ないと出力できない

	//__ ポータルポイントの保存 __//

	for (auto itr = activePortalPointList.begin();itr != activePortalPointList.end();itr++)
	{
		// ナンバー
		data.emplace_back(std::to_string(itr->first));

		// 座標
		data.emplace_back(std::to_string(itr->second->GetPosition().x));
		data.emplace_back(std::to_string(itr->second->GetPosition().y));
		data.emplace_back(std::to_string(itr->second->GetPosition().z));

		// ポータルデータ
		data.emplace_back(std::to_string(itr->second->GetPortalData().numberA));
		data.emplace_back(std::to_string(itr->second->GetPortalData().numberB));

		// 法線
		data.emplace_back(std::to_string(itr->second->GetPortalData().averageNorm.x));
		data.emplace_back(std::to_string(itr->second->GetPortalData().averageNorm.y));
		data.emplace_back(std::to_string(itr->second->GetPortalData().averageNorm.z));

		// 生成元のモデルナンバー
		data.emplace_back(std::to_string(itr->second->GetOwnerModelData().number));

		// 生成元のモデルのポリゴンの最大座標
		data.emplace_back(std::to_string(itr->second->GetOwnerModelData().position.x));
		data.emplace_back(std::to_string(itr->second->GetOwnerModelData().position.y));
		data.emplace_back(std::to_string(itr->second->GetOwnerModelData().position.z));

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

	//__ ポータルポイントの接続相手の保存 __//

	sprintfDx(set, "data/navPoint/navPointTarget_stage_%.2d.csv", _file);
	filePath = set;
	file.open(filePath);

	for (auto itr = activePortalPointList.begin();itr != activePortalPointList.end();itr++)
	{
		// ナンバー
		data.emplace_back(std::to_string(itr->first));

		// 接続相手ナンバー分回す
		for (auto tItr = itr->second->GetInitTargetPortalPointContainer().begin();tItr != itr->second->GetInitTargetPortalPointContainer().end();tItr++)
		{
			PortalPoint* v = *tItr;
			data.emplace_back(std::to_string(v->GetGlobalNumber()));
		}

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

void NavMeshController::PolygonForNavMeshDataSetting()
{
	const std::list<BlockBase*> blocks = stageManager->GetAllBlockObject();		// すべてのブロックを得る

#if DEBUG_INFO_COUNT_OUT
	debugInfoCountList["stageObjCount"]				= 0;	// ナビメッシュ化したステージオブジェクトのカウント
	debugInfoCountList["polygonCount"]				= 0;	// ナビメッシュのポリゴンカウント 
	debugInfoCountList["vertexCount"]				= 0;	// ナビメッシュの頂点カウント 
	debugInfoCountList["portalPointCount"]			= 0;	// ナビメッシュのポータルポイントカウント 
	debugInfoCountList["activePortalPointCount"]	= 0;	// ナビメッシュのアクティブポータルポイントカウント 
#endif
	// ブロックの数分回す
	for (auto blockItr = blocks.begin();blockItr != blocks.end();blockItr++, NavMeshController::ownerModelSaveNum++)
	{
#if DEBUG_INFO_COUNT_OUT
		debugInfoCountList["stageObjCount"]++;
		Debug::DebugOutPutPrintf("stageObjCount -- %d / %d", debugInfoCountList["stageObjCount"], blocks.size());
#endif

		std::unordered_map<int, std::unordered_map<int, PortalPoint*>> modelForLocalPortalPointList;// モデルのポリゴンから構築された、ポータルポイントをモデルの種類ごとに保存

		BlockBase* blockPtr = *blockItr;

		if (!StageObjectData::IsStageObjectToNavMeshCreate(blockPtr->GetStageObjectKind()))
			continue;	// ナビメッシュを構築するステージオブジェクトでなかったら

		Transform blockTrans	= blockPtr->GetTransform();				// block個々のトランスフォーム
		int blockKind			= (int)blockPtr->GetStageObjectKind();	// blockの種類

		auto localVerticesItr			= modelForLocalVerticesAndTargetDataList.find(blockKind);	// モデルの頂点を登録したキーがあるかどうか判定するイテレータ
		
		MeshInfoFactory* meshInfo		= new MeshInfoFactory(blockPtr->GetHmodel());				// メッシュ情報の生成
		const MV1_REF_POLYGONLIST& modelPolygonListData = meshInfo->GetModelPolygonListData();		// モデルに対するすべてのポリゴンデータ	
			
		// キーの初期化
		modelForPolygonDataList[blockKind];
		// ポリゴンデータの代入
		modelForPolygonDataList[blockKind] = modelPolygonListData;

		std::unordered_map<int, VertexData> allVerticesDataList = meshInfo->GetVerticesDataContainer();	// 全ての頂点情報コンテナ
		std::unordered_map<int, PolygonData> allPolygonDataList = meshInfo->GetPolygonDataContainer();	// 全てのポリゴン情報コンテナ

		// キーが登録されてなかったら
		if (localVerticesItr == modelForLocalVerticesAndTargetDataList.end())
		{
			// キーの初期化
			modelForLocalVerticesAndTargetDataList[blockKind];
			// 値の初期化
			modelForLocalVerticesAndTargetDataList[blockKind].clear();
			// 頂点データとその接続相手の頂点データのコンテナの代入
			modelForLocalVerticesAndTargetDataList[blockKind] = meshInfo->GetVerticesTargetDataContainer();
		}

		// メッシュ情報の開放
		delete meshInfo;

		//__ ポータルポイントの生成 __//

		std::unordered_set<std::pair<int, int>, SymmetricPairHash> portalPairList;		// ポリゴン同士で共有しているポータル頂点の順不同のペア(a,b == b,a)を保存　ペアの組み合わせの存在を高速検索したいため

		// モデル個々からポータルポイントを構築し初期化していなかったら
		if (modelForLocalPortalPointList.find(blockKind) == modelForLocalPortalPointList.end())
		{
#if DEBUG_INFO_COUNT_OUT
			debugInfoCountList["portalPointCount"] = 0;
#endif
			// モデルに対するポータルポイントコンテナの設定をする
			ModelPortalPointListSetting
			(
				modelForLocalVerticesAndTargetDataList[blockKind], 
				modelForLocalPortalPointList[blockKind], 
				allVerticesDataList, 
				allPolygonDataList, 
				portalPairList, 
				modelPolygonListData.MaxPosition,
				blockTrans.GetMatrix(), 
				blockPtr
			);
		}

		// ワールド頂点データの初期化のための、ローカル頂点データのpush
		CreateWorldVerticesDataList(allVerticesDataList, blockTrans);

		// ワールドポリゴンデータの初期化
		CreateWorldPoygonDataList(allPolygonDataList, blockTrans);

		std::unordered_map<int, PortalPoint*> initPortalPointList;	// ローカルからグローバルへのポータルポイントの初期化のための変数  接続相手の初期化用のローカルナンバー(localNumber)から接続相手（ポインタ）を初期化

		// ポータルポイントのinitPortalPointListへのpush
		ApplyInitPortalPointList(modelForLocalPortalPointList, blockKind, blockTrans, allVerticesDataList, allPolygonDataList, initPortalPointList);
		// initPortalPointListのポータルポイントの接続相手の設定
		ApplyInitPortalPointTarget(initPortalPointList);

#if DEBUG_INFO_COUNT_OUT
		debugInfoCountList["activePortalPointCount"] = 0;
#endif

		// initPortalPointListをactivePortalPointListにpush
		CreatePortalPoint(initPortalPointList);
	}
	// ポータルポイントをファイルに出力　毎回ポータルポイントの初期化をしなくて済むように、ファイルに出力し読み込む
	SaveNavPoint(stageManager->GetLoadStageNum());
}

std::unordered_set<int> NavMeshController::GetPortalPointShaerdPolygonNumber(std::unordered_map<int, VertexData>& _localAllVerticesDataList, const int& _numA, const int& _numB)
{
	std::unordered_set<int> localPolygonSaveNum;	// 頂点ABで構成されたポリゴンを保存

	const std::unordered_set<int>& conA = _localAllVerticesDataList[_numA].GetPolygonNumberContainer();
	const std::unordered_set<int>& conB = _localAllVerticesDataList[_numB].GetPolygonNumberContainer();

	for (auto polygonAItr = conA.begin();polygonAItr != conA.end() && localPolygonSaveNum.size() < 2;polygonAItr++)
	{
		for (auto polygonBItr = conB.begin();polygonBItr != conB.end();polygonBItr++)
		{
			if (*polygonAItr != *polygonBItr)
				continue;	// 頂点ABを持つポリゴンだったら  基本二つある

			std::pair<std::unordered_set<int>::iterator, bool> isPush = localPolygonSaveNum.insert(*polygonBItr);	// 確認したポリゴンナンバーとして保存

			if (localPolygonSaveNum.size() > 2)
				break;		// 頂点ABで構築される全てのポリゴンを見つけられたら break
		}
	}
	return localPolygonSaveNum;
}

std::vector<VECTOR3> NavMeshController::GetPortalPointShaerdPolygonNorm(std::unordered_map<int, VertexData>& _localAllVerticesDataList, std::unordered_map<int, PolygonData>& _localAllPolygonDataList, const PortalPoint& _portalPoint)
{
	std::unordered_set<int> localPolygonSaveNum = GetPortalPointShaerdPolygonNumber(_localAllVerticesDataList, _portalPoint.GetPortalData().numberA, _portalPoint.GetPortalData().numberB);	// 確認したポリゴンを保存
	std::vector<VECTOR3> reNormCon;					// 返す法線のコンテナ

	for (auto polItr = localPolygonSaveNum.begin();polItr != localPolygonSaveNum.end();polItr++)
	{
		// コンテナに法線をpush
		reNormCon.emplace_back(_localAllPolygonDataList[*polItr].norm);
	}

	if (reNormCon.size() > 2 || reNormCon.empty())
	{
		// ２超えのポリゴンで構成されたポータルが見つかった時　計算があっているかのデバッグ
		assert(false && "two polygon over is Portal find");
	}
	return reNormCon;
}

void NavMeshController::ModelPortalPointListSetting
(
	const std::unordered_map<int, std::unordered_set<VertexData, VertexDataHash>>& _localVerticesAndTargetDataList,
	std::unordered_map<int, PortalPoint*>& _localPortalPointList, 
	std::unordered_map<int, VertexData>& _localAllVerticesDataList, 
	std::unordered_map<int, PolygonData>& _localAllPolygonDataList,
	std::unordered_set<std::pair<int, int>, SymmetricPairHash>& _portalPairList,
	const VECTOR3& _modelMaxPosition,
	const MATRIX& _modelMatrix,
	const void* _modelOwnerPtr
)
{
	int modelPortalPointCreateNum = 0; // モデルごとのポータルポイントの生成ナンバー

	for (auto vAllTarItr = _localVerticesAndTargetDataList.begin();vAllTarItr != _localVerticesAndTargetDataList.end();vAllTarItr++)
	{
#if DEBUG_INFO_COUNT_OUT
		debugInfoCountList["portalPointCount"]++;
		Debug::DebugOutPutProgressNumber("portalPointCount", debugInfoCountList["portalPointCount"], _localVerticesAndTargetDataList.size());
#endif

		VertexData& vData = _localAllVerticesDataList[vAllTarItr->first];

		if (!vData.IsSharedPolygon())
			continue;	// 共通ポリゴンが無かった時

		for (auto vTarItr = vAllTarItr->second.begin();vTarItr != vAllTarItr->second.end();vTarItr++)
		{
			const VertexData& vTarData = *vTarItr;

			int fNum = vData.number;		// ナンバーの値が小さい方の頂点ナンバー	A
			int sNum = vTarData.number;		// ナンバーの値が大きい方の頂点ナンバー	B

			// 小さい方の頂点ナンバーが大きい方の頂点ナンバーよりも大きかったら、スワップ  
			// ペア a,b == b,aとするハッシュを上手く動かすため
			if (fNum > sNum)
				std::swap(fNum, sNum);

			std::pair<std::unordered_set<std::pair<int, int>, SymmetricPairHash>::iterator, bool> isPush = _portalPairList.insert(std::make_pair(fNum, sNum));	// portalListに追加できたかどうか

			// 追加することができたら 新しい組み合わせペアだったら
			if (isPush.second)
			{
				// ２点間座標の平均
				VECTOR3 portalCenterPos = (vData.position + vTarData.position) / 2.0f;	// ポータルの中心座標

				// ポータルポイントコンテナに生成ナンバーとクラスをpush
				_localPortalPointList.insert(std::make_pair(
					modelPortalPointCreateNum,
					new PortalPoint(
						modelPortalPointCreateNum,
						portalCenterPos,
						PortalData(fNum, sNum),
						OwnerModelData(NavMeshController::ownerModelSaveNum, _modelMaxPosition * _modelMatrix, _modelOwnerPtr)
					)
				));

				// ポータルポイントの接続相手の初期化
				ModelPortalPointTargetSetting(_localPortalPointList, modelPortalPointCreateNum, _localAllPolygonDataList, _localAllVerticesDataList, fNum, sNum);

				modelPortalPointCreateNum++;
			}
		}
	}
}

void NavMeshController::ModelPortalPointTargetSetting
(
	std::unordered_map<int, PortalPoint*>& _localPortalPointList, 
	const int& _modelPortalPointCreateNum,
	std::unordered_map<int, PolygonData>& _localAllPolygonDataList,
	std::unordered_map<int, VertexData>& _localAllVerticesDataList, 
	const int& _firstNum, 
	const int& _secondNum
)
{
	PortalPoint& mePortalPoint = *_localPortalPointList[_modelPortalPointCreateNum];

	// ポータルポイントコンテナを全て回す　
	for (auto portalPointItr = _localPortalPointList.begin();portalPointItr != _localPortalPointList.end();portalPointItr++)
	{
		PortalPoint& tarPortalPoint = *portalPointItr->second;

		// どちらかの頂点ナンバーを保持していたら	ポータルポイントの接続相手の候補なのかを判定
		if (tarPortalPoint.GetPortalData().numberA == _firstNum || tarPortalPoint.GetPortalData().numberA == _secondNum || tarPortalPoint.GetPortalData().numberB == _firstNum || tarPortalPoint.GetPortalData().numberB == _secondNum)
		{
			if (portalPointItr->first == _modelPortalPointCreateNum)
				continue;	// 自身の時　自身は追加しないので continue

			// お互いのポータルポイントが接続可能な時
			if (
				CanConnectPortalPoint
				(
					std::move(GetPortalPointShaerdPolygonNorm(_localAllVerticesDataList, _localAllPolygonDataList, mePortalPoint)),
					std::move(GetPortalPointShaerdPolygonNorm(_localAllVerticesDataList, _localAllPolygonDataList, tarPortalPoint))
				)
				)
			{
				//_ 頂点ABで構成されたポリゴン（以降ベースポリゴン１、２とすると）のエッジ４つで成り立つポリゴンがベースポリゴン１または２の法線と方向が一致しなかったら、接続しない(一致したら接続) _//	
				// モデルのポリゴンを貫通した接続を回避するため

				// 接続相手に同じ頂点を持っているポータルのナンバー(今生成された自身)を追加
				tarPortalPoint.PushInitTargetPortalPointNumber(mePortalPoint.GetLocalNumber());

				// 今生成された自身に同じ頂点を持っているポータルのナンバー(相手)を追加
				mePortalPoint.PushInitTargetPortalPointNumber(portalPointItr->second->GetLocalNumber());
			}
		}
	}
}

bool NavMeshController::CanConnectPortalPoint(const std::vector<VECTOR3>& _meNormContainer, const std::vector<VECTOR3>& _targetNormContainer)
{
	for (auto tarNormItr = _targetNormContainer.begin();tarNormItr != _targetNormContainer.end();tarNormItr++)
	{
		for (auto meNormItr = _meNormContainer.begin();meNormItr != _meNormContainer.end();meNormItr++)
		{
			VECTOR3 tarNorm = *tarNormItr;	// 接続相手の法線方向だったら
			VECTOR3 meNorm = *meNormItr;	// 自身の法線方向だったら

			constexpr float SAME_DOT = 0.999f;		// floatの誤差を考慮した同じベクトル方向の内積の判定数値

			// 同じ法線方向だったら
			if (VDot(tarNorm, meNorm) >= SAME_DOT)
				return true;	// 同じ法線方向を見つけたので、returnでtrueを返す
		}
	}
	return false;
}

void NavMeshController::CreateWorldVerticesDataList(std::unordered_map<int, VertexData>& _localVerticesDataList, const Transform& _modelTrans)
{
	//_ ワールド頂点データの初期化 _//

	for (auto vItr = _localVerticesDataList.begin();vItr != _localVerticesDataList.end();vItr++)
	{
		VertexData& vP = vItr->second;
		vP.position *= _modelTrans.GetMatrix();

		// 頂点データのnewとコンテナにpush
		worldVerticesDataList.insert(std::make_pair(NavMeshController::vertexCreateNum, new VertexData(vP)));

		NavMeshController::vertexCreateNum++;
	}
}

void NavMeshController::CreateWorldPoygonDataList(std::unordered_map<int, PolygonData>& _localAllPolygonDataList, const Transform& _modelTrans)
{
	//_ ワールドポリゴンデータの初期化 _//

	for (auto pAllItr = _localAllPolygonDataList.begin();pAllItr != _localAllPolygonDataList.end();pAllItr++)
	{
		PolygonData& pol = pAllItr->second;

		// 重点座標
		pol.centerPosition *= _modelTrans.GetMatrix();

		for (int i = 0;i < 3;i++)
		{
			// 頂点
			pol.vertices[i].position *= _modelTrans.GetMatrix();
		}
		// 法線
		pol.norm *= _modelTrans.GetRotMatrix();

		// ポリゴンデータのnewとコンテナにpush
		worldPoygonDataList.insert(std::make_pair(NavMeshController::polygonCreateNum, new PolygonData(pol)));

		NavMeshController::polygonCreateNum++;
	}
}

void NavMeshController::ApplyInitPortalPointList(std::unordered_map<int, std::unordered_map<int, PortalPoint*>>& _modelForLocalPortalPointList, const int& _modelKind, const Transform& _modelTrans, std::unordered_map<int, VertexData>& _localAllVerticesDataList, std::unordered_map<int, PolygonData>& _localAllPolygonDataList, std::unordered_map<int, PortalPoint*>& _initPortalPointList)
{
	for (auto porLocalItr = _modelForLocalPortalPointList[_modelKind].begin();porLocalItr != _modelForLocalPortalPointList[_modelKind].end();porLocalItr++)
	{
		PortalPoint& portalPoint = *porLocalItr->second;

		//__ 法線の値と0,1,0の方向ベクトルで内積をとり、45度以上135度以下の差があった場合は使用用途(NODE_USE_KIND)を壁にする __//

		std::vector<VECTOR3> normCon = GetPortalPointShaerdPolygonNorm(_localAllVerticesDataList, _localAllPolygonDataList, portalPoint);	// 法線のコンテナ
		VECTOR3 sumNorm = VZero;			// 法線の合計値
		float minNormY = 1.0f;				// 法線のY軸の最小値

		for (auto normItr = normCon.begin();normItr != normCon.end();normItr++)
		{
			// 法線を加算
			sumNorm += *normItr;

			// イテレータの法線Y軸の値より、minNormYの法線Y軸の値の方が大きかったら
			if (minNormY > normItr->y)
			{
				minNormY = normItr->y;
			}
		}

		// 平均法線を算出
		VECTOR3 averageNorm = VDivI(sumNorm, (int)normCon.size());
		// 平均法線を設定
		portalPoint.SetPortalDataAverageNorm(averageNorm);

		// 法線の値と0,1,0の方向ベクトルで内積をとる
		float dot = VDot(VECTOR3(0, 1, 0), VECTOR3(0, minNormY, 0));

		// 60度以上の差があった場合	// マイナスに近づくほど差がある
		if (dot <= cosf(WALL_SET_NORM_ANGLE))
		{
			// 135度を超えた差があった場合　立方体の角は含まない程度の法線ベクトル　
			// 底面は追加しない(ナビゲーションしないので)
			if (dot < cosf(135.0f * DegToRad))
				continue;

			// 壁のNavPointと設定する
			portalPoint.SetUseKind(NODE_USE_KIND::WALL);
		}
		else
		{
			// ノーマルのNavPointと設定する
			portalPoint.SetUseKind(NODE_USE_KIND::NORMAL);
		}

		// 座標
		portalPoint.SetPosition(portalPoint.GetPosition() * _modelTrans.GetMatrix());

		// ポータルポイントのnewとコンテナにpush
		_initPortalPointList.insert(std::make_pair(portalPoint.GetLocalNumber(), &portalPoint));
	}
}

void NavMeshController::ApplyInitPortalPointTarget(std::unordered_map<int, PortalPoint*>& _initPortalPointList)
{
	for (auto initPorItr = _initPortalPointList.begin();initPorItr != _initPortalPointList.end();initPorItr++)
	{
		PortalPoint& portalPoint = *initPorItr->second;
		const std::unordered_set<int>& initTarPorPointLocalNumCon = portalPoint.GetInitTargetPortalPointLocalNumberContainer();	// 初期化用の接続相手のコンテナ

		for (auto porInitTarItr = initTarPorPointLocalNumCon.begin();porInitTarItr != initTarPorPointLocalNumCon.end();porInitTarItr++)
		{
			if (!HasUnorderedMapContainerKey(_initPortalPointList, *porInitTarItr))
				continue;	// 接続相手がinitPortalPointListに追加されていなかったら

			// ローカルナンバーから接続相手を初期化 //
			portalPoint.PushInitTargetPortalPoint(_initPortalPointList[*porInitTarItr]);
			_initPortalPointList[*porInitTarItr]->PushInitTargetPortalPoint(&portalPoint);
		}
	}
}

void NavMeshController::CreatePortalPoint(std::unordered_map<int, PortalPoint*>& _initPortalPointList)
{
	for (auto porLocalItr = _initPortalPointList.begin();porLocalItr != _initPortalPointList.end();porLocalItr++)
	{
#if DEBUG_INFO_COUNT_OUT
		debugInfoCountList["activePortalPointCount"]++;
		Debug::DebugOutPutProgressNumber("activePortalPointCount", debugInfoCountList["activePortalPointCount"], _initPortalPointList.size());
#endif

		PortalPoint& localPortalPoint = *porLocalItr->second;

		//_ 他モデルのポータルポイントとの接続設定 _//

		for (auto porItr = activePortalPointList.begin();porItr != activePortalPointList.end();porItr++)
		{
			// お互いの距離がOTHER_MODEL_PORTALPOINT_CONNECT_DISTANCEよりも近かったら
			if (VSquareSize(porLocalItr->second->GetPosition() - porItr->second->GetPosition()) < OTHER_MODEL_PORTALPOINT_CONNECT_DISTANCE * OTHER_MODEL_PORTALPOINT_CONNECT_DISTANCE)
			{
				//_ 接続相手としてお互いにコンテナにpush _//

				localPortalPoint.PushInitTargetPortalPoint(porItr->second);

				porItr->second->PushInitTargetPortalPoint(porLocalItr->second);
			}
		}

		// グローバルナンバーの設定
		localPortalPoint.SetGlobalNumber(NavMeshController::portalPointCreateNum);
		// ポータルポイントをコンテナに追加
		activePortalPointList.insert(std::make_pair(NavMeshController::portalPointCreateNum, &localPortalPoint));
		NavMeshController::portalPointCreateNum++;
	}
}

void NavMeshController::Update()
{
#if _DEBUG
	if (Debug::GetDebugMode())
	{
		// 描画フラグの管理
		ImGui::Begin("ModelFrameDraw");

		ImGui::Checkbox("ModelFrameDraw", &isFrameDraw);

		/*if (ImGui::Button("ModelFrameDraw"))
		{
			isFrameDraw = !isFrameDraw;
		}*/
		ImGui::End();
	}
#endif
}

void NavMeshController::Draw()
{
	VECTOR3 addPos = VGet(0.0f, 10.0f, 0.0f);	// 描画用加算座標

	if (isFrameDraw)
	{
		//_ フレーム描画 _//

		for (auto pItr = worldPoygonDataList.begin();pItr != worldPoygonDataList.end();pItr++)
		{
			PolygonData& polData = *pItr->second;

			if (VSquareSize(polData.centerPosition - player->GetDebugPosition()) > PLAYER_FOR_NAVMESH_DRAW_DISTANCE * PLAYER_FOR_NAVMESH_DRAW_DISTANCE)
				continue;	// プレイヤーとの距離が遠かったら continue

			// 重点の描画
			//DrawSphere3D(polData.centerPosition, 20.0f, 16, 0xff00ff, 0xffffff, false);

			const std::vector<VertexData> vertices = polData.GetVertexContainer();

			// フレームの描画
			DrawLine3D(vertices[0].position + addPos, vertices[1].position + addPos, 0xffff00);
			DrawLine3D(vertices[1].position + addPos, vertices[2].position + addPos, 0xffff00);
			DrawLine3D(vertices[2].position + addPos, vertices[0].position + addPos, 0xffff00);
		}

		//_ 頂点の描画 _//

		for (auto vItr = worldVerticesDataList.begin();vItr != worldVerticesDataList.end();vItr++)
		{
			VECTOR3 vPos = vItr->second->position;

			if (VSquareSize(vPos - player->GetDebugPosition()) > PLAYER_FOR_NAVMESH_DRAW_DISTANCE * PLAYER_FOR_NAVMESH_DRAW_DISTANCE)
				continue;	// プレイヤーとの距離が遠かったら continue

			// 頂点の球描画
			DrawSphere3D(vPos, 15.0f, 2, 0x00ff00, 0xffffff, false);
		}

		//_ 法線の描画 _//

		for (auto porItr = activePortalPointList.begin();porItr != activePortalPointList.end();porItr++)
		{
			PortalPoint& por = *porItr->second;
			VECTOR3 porPos = por.GetPosition();

			float dot = VDot(VECTOR3(0, 1, 0), por.GetPortalData().averageNorm);
			int color = 0x00ffff;

			dot = fabsf(dot);

			// 差が壁と判定する角度以上の場合
			if (dot <= cosf(WALL_SET_NORM_ANGLE))
			{
				color = 0x0066ff;
			}

			VECTOR3 normDir = por.GetPortalData().averageNorm;	// 法線方向
			VECTOR3 normVec = normDir * 100.0f;					// 法線の長さ

			// 法線の円錐描画
			DrawCone3D(porPos + normVec + (por.GetPortalData().averageNorm * 20.0f), porPos + normVec, 10.0f, 16, color, 0xffffff, true);
			// 法線の線描画
			DrawLine3D(porPos, porPos + normVec, color);

			//if (VSquareSize(porPos - player->GetDebugPosition()) < PLAYER_FOR_NAVMESH_DRAW_DISTANCE * PLAYER_FOR_NAVMESH_DRAW_DISTANCE)
			//	DrawCone3D(porPos + (por.GetPortalData().averageNorm * 80.0f), porPos, 2.0f, 2, 0xff00ff, 0xffffff, true);
		}


		//_ モデルの端っこのポータルポイント _//

		/*for (auto endPorItr = modelEndPortalPointList.begin();endPorItr != modelEndPortalPointList.end();endPorItr++)
		{
			PortalPoint& por = *(*endPorItr);
			VECTOR3 porPos = por.GetPosition();

			DrawLine3D(porPos, porPos + VECTOR3(0.0f, 100.0f, 0.0f), 0x00ff00);
		}*/
	}
}
