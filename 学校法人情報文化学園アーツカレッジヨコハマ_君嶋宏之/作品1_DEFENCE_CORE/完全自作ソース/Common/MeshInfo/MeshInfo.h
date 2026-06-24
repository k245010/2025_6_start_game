#pragma once
#include "../../../Library/myDxLib.h"
#include <unordered_set>

/// <summary>
/// ポリゴンの頂点情報
/// </summary>
struct VertexData
{
	std::unordered_set<int> polygonNumber;	// この頂点をもっているポリゴンの識別idナンバー
	int number; 					// 頂点の識別idナンバー
	VECTOR3 position;				// 座標

	/// <summary>
	/// ハッシュで比較できるように、ナンバーでの==演算子を定義
	/// </summary>
	/// <param name="v"> 相手 </param>
	/// <returns></returns>
	bool operator ==(const VertexData& v) const noexcept
	{
		return number == v.number;
	}

	VertexData() : number(-1), position(VZero)
	{
		polygonNumber.clear();
	}

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 頂点の識別id </param>
	VertexData(const int& _num) : number(_num), position(VZero)
	{
		polygonNumber.clear();
	}

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 頂点の識別id </param>
	/// <param name="_polNum"> この頂点をもっているポリゴンの識別id </param>
	/// <param name="_pos"> 頂点の座標 </param>
	VertexData(const int& _num, const int& _polNum, const VECTOR3& _pos) : number(_num), position(_pos)
	{
		polygonNumber.emplace(_polNum);
	}

	/// <summary>
	/// この頂点をもっているポリゴンの識別idをpushする　ポータルの計算や頂点同士の繋がり計算を行うため
	/// </summary>
	/// <param name="_polNum"> ポリゴンの識別id </param>
	/// <returns> 成功：失敗 </returns>
	bool PushPolygonNumber(const int& _polNum)
	{
		// コンテナの中に引数の値がなかったら
		if (std::find(polygonNumber.begin(), polygonNumber.end(), _polNum) == polygonNumber.end())
		{
			polygonNumber.emplace(_polNum);
			return true;
		}
		return false;
	}

	/// <summary>
	/// この頂点をもっているポリゴンの識別idを削除する
	/// </summary>
	/// <param name="_polNum"></param>
	/// <returns></returns>
	bool DeletePolygonNumber(const int& _polNum)
	{
		auto itr = std::find(polygonNumber.begin(), polygonNumber.end(), _polNum);

		// コンテナの中に引数の値がなかったら
		if (itr == polygonNumber.end())
		{
			return false;
		}
		itr = polygonNumber.erase(itr);
		return true;
	}

	/// <summary>
	/// この頂点で構築されているポリゴンの識別idコンテナを返す
	/// </summary>
	/// <returns> この頂点で構築されているポリゴンの識別idコンテナ </returns>
	const std::unordered_set<int>& GetPolygonNumberContainer() const { return polygonNumber; }

	/// <summary>
	/// この頂点で構築されているポリゴンの識別idコンテナの最初の要素数を返す
	/// </summary>
	/// <returns> この頂点で構築されているポリゴンの識別idコンテナの最初の要素数 </returns>
	int GetPolygonNumberContainerFirst() const { return *polygonNumber.begin(); }

	/// <summary>
	/// 共通の頂点を持ったポリゴンが存在するかどうか返す　存在すればpolygonNumberが2つ以上あるはず　
	/// </summary>
	/// <returns> 存在する：存在しない </returns>
	bool IsSharedPolygon() const { return (polygonNumber.size() > 1); }
};

/// <summary>
/// VertexDataのハッシュ定義
/// </summary>
struct VertexDataHash
{
	size_t operator()(const VertexData& v) const noexcept
	{
		return std::hash<int>()(v.number);
	}
};

/// <summary>
/// ワールド変換された（トランスフォームが適応された）ポリゴン情報 経路探索に必要な最低限のポリゴンの情報を保持
/// </summary>
struct PolygonData
{
	int number;							// ポリゴンの識別id
	VECTOR3 centerPosition;				// 重点
	VECTOR3 norm;						// 法線
	std::vector<VertexData> vertices;	// ポリゴンの頂点データコンテナ

	PolygonData() : number(-1), centerPosition(VZero) {}
	PolygonData(int _num, VECTOR3 _center = VZero) : number(_num), centerPosition(_center) {}
	PolygonData(int _num, VECTOR3 _center = VZero, VertexData _vers[3] = nullptr) : number(_num), centerPosition(_center)
	{
		if (_vers == nullptr)
			return;

		for (int i = 0;i < 3;i++)
			vertices.emplace_back(_vers[i]);
	}

	/// <summary>
	/// ポリゴンの頂点データコンテナを返す
	/// </summary>
	/// <returns> ポリゴンの頂点コンテナ </returns>
	const std::vector<VertexData>& GetVertexContainer() const { return vertices; }

	/// <summary>
	/// 頂点データをコンテナにpushする
	/// </summary>
	/// <param name="_ver"> 頂点座標 </param>
	void PushVertex(const VertexData& _ver) { vertices.emplace_back(_ver); }

	/// <summary>
	/// 頂点データコンテナのサイズを返す
	/// </summary>
	/// <returns> 頂点コンテナのサイズ </returns>
	size_t GetVertexSize() const { return vertices.size(); }
};