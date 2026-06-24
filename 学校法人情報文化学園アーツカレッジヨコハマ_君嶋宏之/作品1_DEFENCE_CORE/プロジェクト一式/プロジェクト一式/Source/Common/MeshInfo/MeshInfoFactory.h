#pragma once
#include "MeshInfo.h"

class Transform;
class MeshInfoFactory
{
public:

	/// <summary>
	///								コンストラクタ  モデルに対するポリゴン情報を構築後、引数に応じて、メッシュ情報を生成する
	/// </summary>
	/// <param name="_meshHandle">	行列計算を行った、モデルハンドル						</param>
	/// <param name="_meshHandle">	メッシュ情報を生成するかどうか (デフォルト：true)		</param>
	MeshInfoFactory(const int& _meshHandle, bool _createMeshInfo = true);
	~MeshInfoFactory();

	/// <summary>
	///				頂点データのコンテナを返す
	/// </summary>
	/// <returns>	頂点データコンテナ	</returns>
	const std::unordered_map<int, VertexData>& GetVerticesDataContainer() const { return allVerticesDataList; }
	
	/// <summary>
	///				ポリゴンデータのコンテナを返す
	/// </summary>
	/// <returns>	ポリゴンデータコンテナ	</returns>
	const std::unordered_map<int, PolygonData>& GetPolygonDataContainer() const { return allPolygonDataList; }

	/// <summary>
	///				頂点とその接続相手の頂点データのコンテナを返す
	/// </summary>
	/// <returns>	頂点とその接続相手の頂点データコンテナ	</returns>
	const std::unordered_map<int, std::unordered_set<VertexData, VertexDataHash>>& GetVerticesTargetDataContainer() const { return allVerticesTargetDataList; }

	/// <summary>
	///				モデルに対するすべてのポリゴンデータを返す
	/// </summary>
	/// <returns>	モデルに対するすべてのポリゴンデータ	</returns>
	const MV1_REF_POLYGONLIST& GetModelPolygonListData() const { return modelPolygonListData; }

private:

	/// <summary>
	/// メッシュ情報の生成
	/// </summary>
	void CreateMeshInfo();

	/// <summary>
	///									頂点同士の統合
	/// </summary>
	/// <param name="_margeDistance">	統合距離			</param>
	void VerticesMarge(const float& _margeDistance);

	MV1_REF_POLYGONLIST modelPolygonListData;	// モデルに対するすべてのポリゴンデータ

	std::unordered_map<int, VertexData> allVerticesDataList;	// すべての頂点情報を保存	一度すべての頂点データにIDを振り分けないと最適化できないため
	std::unordered_map<int, PolygonData> allPolygonDataList;	// すべてのポリゴンを保存
	std::unordered_map<int, std::unordered_set<VertexData, VertexDataHash>> allVerticesTargetDataList;	// すべての頂点情報の接続相手を保存

	int allVertexCreateNum	= 0;	// すべての頂点情報の生成ナンバー
	int meshModelHandle		= -1;	// メッシュのモデルハンドル
};
