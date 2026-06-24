#pragma once
#include "Node/NodeData.h"
#include "../../Stage/StageObjectData.h"
#include "../../Stage/StageInfo.h"
//#include "../../Enemy/EnemyInfo.h"
//#include "../../Enemy/EnemyDataSerializer/EnemyDataSerializer.h"
#include <assert.h>
#include <unordered_map>

/// <summary>
/// NavPointの特定エリアに対するコスト倍率を設定する
/// </summary>
namespace NodeCostRateInfo
{
	inline const std::unordered_map<int, float> AREA_COST_RATE =	// 個々の特定エリアのコストを設定するコンテナ	int -> AREA_COST_KIND , float -> 倍率
	{
		{(int)StageObjectData::STAGE_OBJECT_KIND::SWAMP_BLOCK, 2.0f},
	};

	/// <summary>
	///							特定のエリアに対応したノードのベースコストを返す
	/// </summary>
	/// <param name="_kind">	特定のエリア種類 </param>
	/// <returns>				特定のエリアに対応したノードのベースコスト倍率 </returns>
	inline const float GetAreaCostRate(const StageObjectData::STAGE_OBJECT_KIND& _stageObjKind)
	{
		// キーが見つからなかったら　コスト倍率が設定されていなかったら
		if (AREA_COST_RATE.find((int)_stageObjKind) == AREA_COST_RATE.end())
			return 1.0f;
		return AREA_COST_RATE.at((int)_stageObjKind);
	}

	/// <summary>
	///				通常のノードのコスト倍率を返す
	/// </summary>
	/// <returns>	通常のノードのコスト倍率		</returns>
	inline const float GetNormalAreaCostRate() { return 1.0f; }

	inline const std::unordered_map<int, float> NODE_USE_KIND_COST_RATE =
	{
		{ (int)NODE_USE_KIND::NORMAL,	1.0f },
		{ (int)NODE_USE_KIND::WALL,		1.5f },
	};

	/// <summary>
	///								ノードの種類に応じたコスト倍率を返す
	/// </summary>
	/// <param name="_nodeUseKind">	ノードの種類	</param>
	/// <returns>					コスト倍率		</returns>
	inline const float GetNodeUseKindCostRate(const NODE_USE_KIND& _nodeUseKind)
	{
		// キーが見つからなかったら　コスト倍率が設定されていなかったら
		if (NODE_USE_KIND_COST_RATE.find((int)_nodeUseKind) == NODE_USE_KIND_COST_RATE.end())
			return 1.0f;
		return NODE_USE_KIND_COST_RATE.at((int)_nodeUseKind);
	}

	/// <summary>
	///							移動速度に対する、コスト倍率を返す
	/// </summary>
	/// <param name="_speed">	移動速度								</param>
	/// <returns>				移動速度に対する、コスト倍率 0.0f～1.0f	</returns>
	inline const float GetSpeedCostRate(const float& _speed)
	{
		return 1.0f / _speed;	// 移動速度に対する1コストの値
	}

	/// <summary>
	///							ステージオブジェクトに潰されたノードのコスト倍率を返す
	/// </summary>
	/// <param name="_kind">	エージェントの種類														</param>
	/// <param name="_speed">	エージェントの移動速度													</param>
	/// <param name="_kind">	エージェントがステージオブジェクトに対して１秒間に与えるダメージ量		</param>
	/// <param name="_hp">		ステージオブジェクトのHP												</param>
	/// <returns>				ノードのコスト倍率														</returns>
	inline const float GetBlockedCostRate(const float& _speed,const float& _dps,const int& _hp)
	{
		float costValue		= GetSpeedCostRate(_speed);					// 1コストの値
		
		float trapCost		= (StageInfo::BLOCK_SIZE / 2) * costValue;	// 罠のコスト値　潰されたナビポイントとその接続相手の距離がブロックの半径と仮定する
		float breakCost		= _hp / _dps;								// 罠を破壊するコスト 

		float blockedCost	= trapCost + breakCost;						// 潰されたナビポイントのコスト

		return blockedCost / trapCost;									// 通常の罠のコストに対する倍率を返す
	}
}