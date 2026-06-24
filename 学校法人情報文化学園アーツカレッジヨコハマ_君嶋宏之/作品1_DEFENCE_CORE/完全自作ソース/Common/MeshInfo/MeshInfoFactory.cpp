#include "MeshInfoFactory.h"
#include "../Function/Function.h"
#include "../../Collision/CollisionFunction.h"
#include <assert.h>

namespace
{
	constexpr float VERTEX_MARGE_DIS = 0.5f;	// この距離よりも近かったら、統合する
}

MeshInfoFactory::MeshInfoFactory(const int& _meshHandle,bool _createMeshInfo)
{
	assert(_meshHandle >= 0 && "モデルハンドルが不正です");

	// 引数モデルと同じモデル基本データを使用してモデルを作成、代入する
	meshModelHandle = MV1DuplicateModel(_meshHandle);

	// モデル情報を得るための、セットアップ
	MV1SetupReferenceMesh(meshModelHandle, -1, true);

	if (!_createMeshInfo)
		return;	// メッシュ情報を生成しないので、rerturn

	// メッシュ情報の生成
	CreateMeshInfo();
}

MeshInfoFactory::~MeshInfoFactory()
{
	MV1DeleteModel(meshModelHandle);

	allVerticesDataList.clear();
	allPolygonDataList.clear();
	allVerticesTargetDataList.clear();
}

void MeshInfoFactory::CreateMeshInfo()
{
	// すべてのポリゴンデータを代入
	modelPolygonListData = MV1GetReferenceMesh(meshModelHandle, -1, true);

	int maxPolygonNum = modelPolygonListData.PolygonNum;// ポリゴンの最大値
	VECTOR3 polygonCenterPos;							// ローカル重点座標
	VECTOR3 localVerPos[3];								// ローカル頂点座標
	VECTOR3 localVerSum;								// ローカル頂点座標の平均を求めるために座標をすべて足すための変数

	// モデルのポリゴン数分回す
	for (int polygonNum = 0; polygonNum < maxPolygonNum; polygonNum++)
	{
		// 頂点の数だけ回す
		for (int v = 0; v < 3; v++)
		{
			// 指定されたポリゴンの頂点座標を代入
			localVerPos[v]	= modelPolygonListData.Vertexs[modelPolygonListData.Polygons[polygonNum].VIndex[v]].Position;
			localVerSum		+= localVerPos[v];

			// allVerticesDataListのイテレータを初期化
			auto allVerticesItr = allVerticesDataList.begin();

			// allVerticesDataListのコンテナをすべて回す
			for (allVerticesItr;allVerticesItr != allVerticesDataList.end();allVerticesItr++)
			{
				VECTOR3 vPos = allVerticesItr->second.position;

				// 座標が同じだったら　同じならallVerticesDataListにpushしない 
				if (VAllSame(vPos, localVerPos[v]))
				{
					// この頂点を保持するポリゴンナンバーを追加
					allVerticesItr->second.PushPolygonNumber(polygonNum);

					// ポリゴン側に頂点データをpush
					allPolygonDataList[polygonNum].PushVertex(allVerticesItr->second);
					break;	// 座標が同じ頂点が見つかったので、breaks
				}
			}

			// 座標が同じ頂点が見つからなかったら　新しい頂点だったら
			if (allVerticesItr == allVerticesDataList.end())
			{
				// 頂点データを生成 この頂点を保持するポリゴンナンバーも追加している
				VertexData vAllP = VertexData(allVertexCreateNum, polygonNum, localVerPos[v]);

				// 頂点コンテナにポリゴンナンバーと頂点データをpush
				allVerticesDataList.insert(std::make_pair(allVertexCreateNum, vAllP));

				// 頂点の接続相手を保存するコンテナの初期化
				allVerticesTargetDataList[vAllP.number];
				allVerticesTargetDataList[vAllP.number].clear();

				// ポリゴン側に頂点データをpush
				allPolygonDataList[polygonNum].PushVertex(vAllP);

				allVertexCreateNum++;
			}
		}
		// 重点座標を計算 ポリゴンの頂点決定時に計算
		polygonCenterPos	= VDivF(localVerSum, 3.0f);
		localVerSum			= VZero;

		// 重点座標を代入
		allPolygonDataList[polygonNum].centerPosition = polygonCenterPos;

		// 法線の初期化 //
		VECTOR3 v1 = localVerPos[0] - localVerPos[1];
		VECTOR3 v2 = localVerPos[0] - localVerPos[2];
		allPolygonDataList[polygonNum].norm = VNorm(VCross(v1, v2));
	}

	//_ 同じポリゴンで構成された頂点同士をさがし、互いに接続相手として追加する _//
	
	// すべての頂点データ分回す
	for (auto vAllItr = allVerticesDataList.begin();vAllItr != allVerticesDataList.end();vAllItr++)
	{
		const VertexData& vAllData						= vAllItr->second;
		const std::unordered_set<int>& polygonNumCon	= vAllData.GetPolygonNumberContainer();	// この頂点で構築されている、ポリゴンの識別ナンバーコンテナ

		for (auto polygonNumItr = polygonNumCon.begin();polygonNumItr != polygonNumCon.end();polygonNumItr++)
		{
			for (auto vAllTarItr = allVerticesDataList.begin();vAllTarItr != allVerticesDataList.end();vAllTarItr++)
			{
				if (vAllItr == vAllTarItr)
					continue;	// 自身だったら

				const VertexData& vAllTarData					= vAllTarItr->second;
				const std::unordered_set<int>& tarPolygonNumCon	= vAllTarData.GetPolygonNumberContainer();	// 相手の頂点で構築されている、ポリゴンの識別ナンバーコンテナ

				// 同じポリゴンで構成されていたら
				if (tarPolygonNumCon.find(*polygonNumItr) != tarPolygonNumCon.end())
				{
					//_ 互いに接続相手としてallVerticesTargetDataListにpush追加 _//
					allVerticesTargetDataList[vAllItr->second.number].emplace(vAllTarItr->second);
					allVerticesTargetDataList[vAllTarItr->second.number].emplace(vAllItr->second);
				}
			}
		}
	}

	// 一定距離内の頂点座標を統合
	VerticesMarge(VERTEX_MARGE_DIS);
}

void MeshInfoFactory::VerticesMarge(const float& _margeDistance)
{
	//_ 一定距離内の頂点座標を統合 _//

	std::unordered_set<VertexData, VertexDataHash> saveDeleteList;		// 削除する頂点を保存
	std::unordered_set<VertexData, VertexDataHash> saveNoDeleteList;	// 削除しない頂点を保存

	// すべての頂点と接続データ分回す
	for (auto allVerticesTarDataItr = allVerticesTargetDataList.begin();allVerticesTarDataItr != allVerticesTargetDataList.end();allVerticesTarDataItr++)
	{
		if (allVerticesDataList.find(allVerticesTarDataItr->first) == allVerticesDataList.end())
			continue;	// キーが登録されていなかったら

		VertexData vertexMe = allVerticesDataList[allVerticesTarDataItr->first];	// 自身の頂点情報

		if (saveDeleteList.find(vertexMe) != saveDeleteList.end())
			continue;	// 削除コンテナに保存されていたら

		// 頂点接続相手
		for (auto vTarItr = allVerticesTarDataItr->second.begin();vTarItr != allVerticesTarDataItr->second.end();vTarItr++)
		{
			if (saveDeleteList.find(*vTarItr) != saveDeleteList.end())
				continue;	// 削除コンテナに保存されていたら

			VertexData vertexTar = *vTarItr;	// 接続相手の頂点情報

			//__ 頂点同士の距離を求める __//

			VECTOR3 edgeVec = vertexMe.position - vertexTar.position;
			float SquareDistance = VSquareSize(edgeVec);

			// 距離が統合距離以下だったら
			if (SquareDistance <= _margeDistance * _margeDistance)
			{
				//__ 接続相手に削除候補があるなら、そちらに合わせる（そちらで削除したい）ので削除しない __//

				auto searchVAllTarItr = allVerticesTargetDataList.find(vTarItr->number);

				if (searchVAllTarItr == allVerticesTargetDataList.end())
					assert(false);

				//__ 削除する頂点がもっている接続相手の頂点を自身へ追加 __//

				for (auto holdItr = searchVAllTarItr->second.begin();holdItr != searchVAllTarItr->second.end();holdItr++)
				{
					//_ 統合する側(他にすでに追加されている接続相手が削除対象の接続相手にいてもpushするがunordered_setなので、許容)と削除する頂点コンテナに追加されていたらpusn(統合)しない _//

					if (vTarItr == holdItr)
						continue;	// 自身だたっら continue

					if (!saveDeleteList.empty())
					{
						if (saveDeleteList.find(*holdItr) != saveDeleteList.end())
							continue;	// saveDeleteListに追加されていたら continue
					}

					// 削除する頂点がもっている接続相手の頂点をpush
					searchVAllTarItr->second.emplace(*holdItr);
				}

				// 削除しない頂点コンテナに追加されていなかったら
				if (saveNoDeleteList.find(vertexTar) == saveNoDeleteList.end())
				{
					// 削除する頂点を保存
					saveDeleteList.emplace(vertexTar);
				}
			}
			// 削除しない頂点を保存
			saveNoDeleteList.emplace(allVerticesTarDataItr->first);
		}
	}

	// 頂点の接続相手の接続相手から接続相手自身を削除
	for (auto deleteItr = saveDeleteList.begin(); deleteItr != saveDeleteList.end();deleteItr++)
	{
		VertexData deleteVertex = *deleteItr;

		for (auto vAllTarItr = allVerticesTargetDataList.begin();vAllTarItr != allVerticesTargetDataList.end();vAllTarItr++)
		{
			for (auto vTarItr = vAllTarItr->second.begin();vTarItr != vAllTarItr->second.end();)
			{
				// 自身が接続相手に含まれていたら
				if (*deleteItr == *vTarItr)
				{
					// 接続相手の接続相手から接続相手自身を削除
					vTarItr = vAllTarItr->second.erase(vTarItr);
					break;	// 削除したので break
				}
				vTarItr++;
			}
		}

		auto vertexTarDataItr = allVerticesTargetDataList.find(deleteVertex.number);

		if (vertexTarDataItr != allVerticesTargetDataList.end())
			assert(false);

		// deleteVertexをコンテナから削除
		allVerticesTargetDataList.erase(vertexTarDataItr);
	}
}

