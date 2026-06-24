#pragma once
#include "../StageObjectData.h"
#include "../../Collision/CollisionObjectKind.h"

/// <summary>
/// 罠に関する情報
/// </summary>
namespace TrapInfo
{
	constexpr float MAX_TRAP_INFO_DRAW_DISTANCE = 500.0f;	// 罠の情報の最大描画距離
	constexpr float MAX_TRAP_INTERACT_DISTANCE	= 500.0f;	// 罠に対して干渉できる距離

	/// <summary>
	///				罠として分類されるステージオブジェクトをコンテナで返す
	/// </summary>
	/// <returns>	罠として分類されるステージオブジェクトコンテナ </returns>
	inline std::set<int> GetTrapStageObjectKindContainer()
	{
		return std::set<int>
		{
			(int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK,
			(int)StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK,
			(int)StageObjectData::STAGE_OBJECT_KIND::JUMP_PAD,
			(int)StageObjectData::STAGE_OBJECT_KIND::REMOTE_BOMB,
			(int)StageObjectData::STAGE_OBJECT_KIND::AUTO_TURRET,
			(int)StageObjectData::STAGE_OBJECT_KIND::ACTIVE_TURRET,
			(int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE,
			(int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD,
		};
	}

	/// <summary>
	///				罠の当たり判定の種類をコンテナで返す
	/// </summary>
	/// <returns>	罠の当たり判定の種類コンテナ </returns>
	inline std::set<int> GetTrapCollisionObjectKindContainer()
	{
		return std::set<int>
		{
			(int)COLLISION_OBJECT_KIND::WALL_BLOCK,
			(int)COLLISION_OBJECT_KIND::SPIKE_BLOCK,
			(int)COLLISION_OBJECT_KIND::JUMP_PAD,
			(int)COLLISION_OBJECT_KIND::BOMB,
			(int)COLLISION_OBJECT_KIND::TURRET,
		};
	}

	/// <summary>
	///							引数のステージオブジェクトの種類が罠として分類されるか返す
	/// </summary>
	/// <param name="_kind">	STAGE_OBJECT_KIND										</param>
	/// <returns>				true:罠として分類される / false:罠として分類されない	</returns>
	inline bool IsTrapStageObjectKind(const StageObjectData::STAGE_OBJECT_KIND& _kind)
	{
		const auto& traps = GetTrapStageObjectKindContainer();

		if (traps.find((int)_kind) == traps.end())
			return false;
		return true;
	}

	// ToDo:以下のデータの外部化
#if 1
	/// <summary>
	/// 攻撃範囲など影響範囲の半径を描画するためのデータコンテナ
	/// </summary>
	inline const std::unordered_map<int, std::vector<float>> VIEW_IMPACT_RADIUS_CONFIG =
	{
		// key:ステージオブジェクトの種類				/	value:影響範囲 level 1 ～
		{ (int)StageObjectData::STAGE_OBJECT_KIND::AUTO_TURRET,			{ StageInfo::BLOCK_SIZE * 5.0f, StageInfo::BLOCK_SIZE * 10.0f } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::ACTIVE_TURRET,		{ StageInfo::BLOCK_SIZE * 5.0f, StageInfo::BLOCK_SIZE * 10.0f } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE,{ StageInfo::BLOCK_SIZE * 5.0f, StageInfo::BLOCK_SIZE * 10.0f } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD,{ StageInfo::BLOCK_SIZE * 2.0f, StageInfo::BLOCK_SIZE * 4.0f  } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK,			{ -1.0f } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK,			{ -1.0f } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::JUMP_PAD,			{ -1.0f } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::REMOTE_BOMB,			{ -1.0f } },
	};

	/// <summary>
	///							攻撃範囲など影響範囲の描画するための半径を返す
	/// </summary>
	/// <param name="_kind">	ステージオブジェクトの種類									</param>
	/// <param name="_level">	罠のレベル													</param>
	/// <returns>				値が設定されている:0以上 / 値が設定されていない：0未満		</returns>
	inline float GetViewImpactRadius(const StageObjectData::STAGE_OBJECT_KIND& _kind,const int& _level)
	{
		HasUnorderedMapContainerKey(VIEW_IMPACT_RADIUS_CONFIG, (int)_kind);

		if (_level <= 0)
			assert(false && "レベル０以下を指定しています");

		// コンテナサイズに対してレベルが超えていたら
		if (VIEW_IMPACT_RADIUS_CONFIG.at((int)_kind).size() < _level)
			return -1.0f;
		// インデクスとしてアクセスするために _level - 1 する
		return VIEW_IMPACT_RADIUS_CONFIG.at((int)_kind).at(_level - 1);
	}

	/// <summary>
	/// 罠の設置コストコンテナ
	/// </summary>
	inline const std::unordered_map<int, int> PUT_COST_CONFIG =
	{
		{ (int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK,			15	},
		{ (int)StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK,			10	},
		{ (int)StageObjectData::STAGE_OBJECT_KIND::JUMP_PAD,			15	},
		{ (int)StageObjectData::STAGE_OBJECT_KIND::REMOTE_BOMB,			5	},
		{ (int)StageObjectData::STAGE_OBJECT_KIND::AUTO_TURRET,			50	},
		{ (int)StageObjectData::STAGE_OBJECT_KIND::ACTIVE_TURRET,		50	},
		{ (int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE,50	},
		{ (int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD,50	},
	};

	/// <summary>
	///							罠の設置コストを返す
	/// </summary>
	/// <param name="_kind"> 	ステージオブジェクトの種類	</param>
	inline int GetPutCost(const StageObjectData::STAGE_OBJECT_KIND& _kind)
	{
		HasUnorderedMapContainerKey(PUT_COST_CONFIG, (int)_kind, true);

		return PUT_COST_CONFIG.at((int)_kind);
	}

	/// <summary>
	/// 罠のアップグレードコストコンテナ
	/// </summary>
	inline const std::unordered_map<int, std::vector<int>> UPGRADE_COST_CONFIG =
	{
		// key:ステージオブジェクトの種類				/			value:アップグレード level 2 ～
		{ (int)StageObjectData::STAGE_OBJECT_KIND::AUTO_TURRET,			{ 150 } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::ACTIVE_TURRET,		{ 150 } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE,{ 150 } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD,{ 150 } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK,			{ 30  } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK,			{ 15  } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::JUMP_PAD,			{ 15  } },
		{ (int)StageObjectData::STAGE_OBJECT_KIND::REMOTE_BOMB,			{ 15  } },
	};

	/// <summary>
	///							罠のアップグレードコストを返す
	/// </summary>
	/// <param name="_kind">	ステージオブジェクトの種類													</param>
	/// <param name="_level">	アップグレードしたいレベル	2～												</param>
	/// <returns>				アップグレードできる：アップグレードコスト / アップグレードできない:-1;		</returns>
	inline int GetUpgradeCost(const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _level)
	{
		HasUnorderedMapContainerKey(UPGRADE_COST_CONFIG, (int)_kind, true);

		if (_level <= 1)
			assert(false && "不正なアップグレードレベルです");

		// valueのコンテナのサイズよりも、多いレベルのインデクスでアクセスしようとしていたら
		if (UPGRADE_COST_CONFIG.at((int)_kind).size() < _level - 1)
			return -1;
		// インデクスとしてアクセスするために _level - 2 する
		return UPGRADE_COST_CONFIG.at((int)_kind).at(_level - 2);
	}
#else
	struct TrapData
	{
		int putCost;						// 設置コスト
		int upgradeCost;					// アップグレードコスト
		int levelMax;						// 最大レベル
		std::vector<float> impactRadius;	// レベルに応じた攻撃範囲
	};
#endif
}