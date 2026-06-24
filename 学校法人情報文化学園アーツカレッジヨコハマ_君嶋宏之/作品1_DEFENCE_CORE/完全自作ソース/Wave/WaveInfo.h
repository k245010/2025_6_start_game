#pragma once
#include "../Enemy/EnemyKind.h"
#include "../../Library/VectorJsonConverter.h"
#include "../../nlohmann/json.hpp"

namespace WaveEnemyInfo
{
	struct WaveEnemyData
	{
		int enemyKind;				// 敵の種類
		VECTOR3I summonPosition;	// 召喚座標

		/// <summary>
		///						コンストラクタ
		/// </summary>
		/// <param name="_kind">敵の種類			</param>
		/// <param name="_pos">	召喚座標			</param>
		WaveEnemyData(const int& _kind, const VECTOR3I& _pos) : enemyKind(_kind), summonPosition(_pos) {}
		WaveEnemyData() : enemyKind(0), summonPosition(VZero) {}
	};

	/// <summary>
	///								WaveDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照					</param>
	/// <param name="_waveData">	WaveData					</param>
	inline void to_json(nlohmann::json& _json, const WaveEnemyData& _waveData)
	{
		_json = nlohmann::json
		{
			{"EnemyKind",		_waveData.enemyKind			},
			{"SummonPosition",	_waveData.summonPosition	},
		};
	}

	/// <summary>
	///							jsonからWaveDataへ変換する
	/// </summary>
	/// <param name="_json">	json						</param>
	/// <param name="_waveData">WaveData参照				</param>
	inline void from_json(const nlohmann::json& _json, WaveEnemyData& _waveData)
	{
		_json.at("EnemyKind").get_to(_waveData.enemyKind);
		_json.at("SummonPosition").get_to(_waveData.summonPosition);
	}
}

namespace WaveInfo
{
	struct WaveData
	{
		std::list<WaveEnemyInfo::WaveEnemyData> waveDataList;	// ウェーブデータコンテナ

		WaveData() : waveDataList(std::list<WaveEnemyInfo::WaveEnemyData>{})											{}
 		WaveData(const std::list<WaveEnemyInfo::WaveEnemyData>& _waveDataContainer) : waveDataList(_waveDataContainer)	{}
	};

	inline void to_json(nlohmann::json& _json, const WaveData& _waveData)
	{
		_json = nlohmann::json
		{
			{"WaveDataList",		_waveData.waveDataList	},
		};
	}
	
	inline void from_json(const nlohmann::json& _json, WaveData& _waveData)
	{
		_json.at("WaveDataList").get_to(_waveData.waveDataList);
	}
}