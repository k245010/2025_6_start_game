#pragma once
#include "../Transform/Transform.h"
#include "StageInfo.h"
#include "../Common/Function/Function.h"
#include <set>
#include <unordered_map>

/// <summary>
/// ステージオブジェクトの種類データ
/// </summary>
namespace StageObjectData
{
	/// <summary>
	/// ステージオブジェクトの種類
	/// </summary>
	enum class STAGE_OBJECT_KIND
	{
		NONE		 = -1,

		GROUND_BLOCK,			// 地面（地形）
		WALL_BLOCK,				// 壁
		SPIKE_BLOCK,			// とげ
		SWAMP_BLOCK,			// 泥
		CORE_BLOCK,				// コア

		AUTO_TURRET,			// 自動で動く銃座
		ACTIVE_TURRET,			// 一定時間ごとにメンテナンスする必要のある銃座
		RELOAD_TURRET_SINGLE,	// 弾をリロードする必要のある銃座 (単発)
		RELOAD_TURRET_SPREAD,	// 弾をリロードする必要のある銃座 (拡散)
		JUMP_PAD,				// ジャンプパッド
		SLIME_PATCH,			// ベトベトスライム
		COIN,					// コイン
		REMOTE_BOMB,			// 遠隔起爆爆弾

		MAX,
	};

	inline const std::unordered_map<STAGE_OBJECT_KIND, std::string> STAGE_OBJECT_KIND_STRING_LIST =	// enumのSTAGE_OBJECT_KINDを文字列として持つコンテナ
	{
		{STAGE_OBJECT_KIND::NONE,					"NONE"					},
		{STAGE_OBJECT_KIND::GROUND_BLOCK,			"GROUND_BLOCK"			},
		{STAGE_OBJECT_KIND::WALL_BLOCK,				"WALL_BLOCK"			},
		{STAGE_OBJECT_KIND::SPIKE_BLOCK,			"SPIKE_BLOCK"			},
		{STAGE_OBJECT_KIND::SWAMP_BLOCK,			"SWAMP_BLOCK"			},
		{STAGE_OBJECT_KIND::CORE_BLOCK,				"CORE_BLOCK"			},
		{STAGE_OBJECT_KIND::AUTO_TURRET,			"AUTO_TURRET"			},
		{STAGE_OBJECT_KIND::ACTIVE_TURRET,			"ACTIVE_TURRET"			},
		{STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE,	"RELOAD_TURRET_SINGLE"	},
		{STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD,	"RELOAD_TURRET_SPREAD"	},
		{STAGE_OBJECT_KIND::JUMP_PAD,				"JUMP_PAD"				},
		{STAGE_OBJECT_KIND::SLIME_PATCH,			"SLIME_PATCH"			},
		{STAGE_OBJECT_KIND::COIN,					"COIN"					},
		{STAGE_OBJECT_KIND::REMOTE_BOMB,			"REMOTE_BOMB"			},
	};

	/// <summary>
	///						ステージオブジェクトの種類に対して、対応する文字列を返す
	/// </summary>
	/// <param name="_kind">ステージオブジェクトの種類		</param>
	/// <returns>			対応する文字列					</returns>
	inline std::string GetToString(const STAGE_OBJECT_KIND& _kind)
	{
		if (STAGE_OBJECT_KIND_STRING_LIST.find(_kind) == STAGE_OBJECT_KIND_STRING_LIST.end())
			assert(false && "不正なSTAGE_OBJECT_KINDです");

		return STAGE_OBJECT_KIND_STRING_LIST.at(_kind);	// _kindに対する文字列を返す
	}

	/// <summary>
	///							ステージオブジェクトの種類の文字列に対して、対応する列挙体を返す
	/// </summary>
	/// <param name="_kindStr">	ステージオブジェクトの種類の文字列	</param>
	/// <returns>				STAGE_OBJECT_KIND					</returns>
	inline STAGE_OBJECT_KIND GetFromEnum(std::string_view _kindStr)
	{
		
		for (const auto& stageObjeKindStr : STAGE_OBJECT_KIND_STRING_LIST)
		{
			// 文字列を見つけられたら
			if (stageObjeKindStr.second == _kindStr)
			{
				return stageObjeKindStr.first;	//ステージオブジェクトの種類の文字列に対応するSTAGE_OBJECT_KINDを返す
			}
		}
		assert(false && "不正なのSTAGE_OBJECT_KIND文字列です");
		return STAGE_OBJECT_KIND::NONE;
	}

	/// <summary>
	/// ステージオブジェクトのタイプ
	/// </summary>
	enum class TYPE
	{
		NONE	= -1,

		BLOCK	= 0,	// ブロック系
		ENTITY,			// エンティティ系
	};

	/// <summary>
	///							ステージオブジェクトのタイプを返す
	/// </summary>
	/// <param name="_kind">	ステージオブジェクトの種類 </param>
	/// <returns>				ステージオブジェクトのタイプ </returns>
	inline TYPE GetStageObjectType(const STAGE_OBJECT_KIND& _kind)
	{
		switch (_kind)
		{
		case STAGE_OBJECT_KIND::GROUND_BLOCK:
		case STAGE_OBJECT_KIND::WALL_BLOCK:
		case STAGE_OBJECT_KIND::SPIKE_BLOCK:
		case STAGE_OBJECT_KIND::SWAMP_BLOCK:
		case STAGE_OBJECT_KIND::CORE_BLOCK:

			return TYPE::BLOCK;
			break;
		case STAGE_OBJECT_KIND::COIN:
		case STAGE_OBJECT_KIND::AUTO_TURRET:
		case STAGE_OBJECT_KIND::ACTIVE_TURRET:
		case STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE:
		case STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD:
		case STAGE_OBJECT_KIND::SLIME_PATCH:
		case STAGE_OBJECT_KIND::JUMP_PAD:
		case STAGE_OBJECT_KIND::REMOTE_BOMB:

			return TYPE::ENTITY;
			break;
		default:

			assert(false && "STAGE_OBJECT_KINDの列挙体を見つけることができませんでした");
			break;
		}
		return TYPE::NONE;
	}

	/// <summary>
	///						引数で渡されたSTAGE_OBJECT_KINDがブロック系かどうかを返す
	/// </summary>
	/// <param name="_kind">STAGE_OBJECT_KIND								</param>
	/// <returns>			true:ブロック系である / false:ブロック系でない	</returns>
	inline bool IsBlockStageObjectKind(const STAGE_OBJECT_KIND& _kind)
	{
		return GetStageObjectType(_kind) == TYPE::BLOCK;
	}

	/// <summary>
	///						引数で渡されたSTAGE_OBJECT_KINDがエンティティ系かどうかを返す
	/// </summary>
	/// <param name="_kind">STAGE_OBJECT_KIND										</param>
	/// <returns>			true:エンティティ系である / false:エンティティ系でない	</returns>
	inline bool IsEntityStageObjectKind(const STAGE_OBJECT_KIND& _kind)
	{
		return GetStageObjectType(_kind) == TYPE::ENTITY;
	}

	/// <summary>
	///						TYPEごとのSTAGE_OBJECT_KINDの最大値を返す
	/// </summary>
	/// <param name="_type">調べたいステージオブジェクトのタイプ	</param>
	/// <returns>			MAX値									</returns>
	inline int GetStageObjectKindTypeMax(const TYPE& _type)
	{
		int re = 0;
		for (int i = 0;i < (int)STAGE_OBJECT_KIND::MAX;i++)
		{
			// タイプがブロックでなかったら
			if (_type == TYPE::BLOCK)
			{
				// ブロックだったら
				if (IsBlockStageObjectKind((STAGE_OBJECT_KIND)i))
				{
					re++;
				}
			}
			// タイプがブロックでなかったら
			else
			{
				// エンティティだったら
				if (IsEntityStageObjectKind((STAGE_OBJECT_KIND)i))
				{
					re++;
				}
			}
		}
		return re;
	}

	/// <summary>
	///							タイプに属するステージオブジェクトの種類コンテナを返す
	/// </summary>
	/// <param name="_type">	ステージオブジェクトのタイプ				</param>
	/// <returns>				ステージオブジェクトのタイプの種類コンテナ	</returns>
	inline std::set<int> GetStageObjectKindContainer(const TYPE& _type)
	{
		std::set<int> re;		// returnするステージオブジェクトの種類コンテナ

		for (int i = 0;i < (int)STAGE_OBJECT_KIND::MAX;i++)
		{
			// タイプがブロックでなかったら
			if (_type == TYPE::BLOCK)
			{
				// ブロックだったら
				if (IsBlockStageObjectKind((STAGE_OBJECT_KIND)i))
				{
					re.emplace(i);
				}
			}
			// タイプがブロックでなかったら
			else
			{
				// エンティティだったら
				if (IsEntityStageObjectKind((STAGE_OBJECT_KIND)i))
				{
					re.emplace(i);
				}
			}
		}
		return re;
	}

	/// <summary>
	///							ステージオブジェクトのトランスフォームのスケールとサイズ値を設定する
	/// </summary>
	/// <param name="_trans">	トランスフォーム			</param>
	/// <param name="_kind">	ステージオブジェクトの種類	</param>
	inline void SetScaleAndSize(Transform& _trans,const STAGE_OBJECT_KIND& _kind)
	{
		switch (_kind)
		{
		case STAGE_OBJECT_KIND::WALL_BLOCK:
		case STAGE_OBJECT_KIND::SLIME_PATCH:

			_trans.scale	= VOne * StageInfo::BLOCK_SIZE;
			_trans.size		= VOne * 1.0f;
			break;
		case STAGE_OBJECT_KIND::AUTO_TURRET:
		case STAGE_OBJECT_KIND::ACTIVE_TURRET:
		case STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE:
		case STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD:
		case STAGE_OBJECT_KIND::JUMP_PAD:

			_trans.scale	= VOne * StageInfo::BLOCK_SIZE / 200.0f;
			_trans.size		= VOne * 200.0f;
			break;
		case STAGE_OBJECT_KIND::CORE_BLOCK:

			_trans.scale	= VOne * StageInfo::BLOCK_SIZE / 5.0f;
			_trans.size		= VOne * 10.0f;
			break;
		case STAGE_OBJECT_KIND::COIN:

			_trans.scale	= VOne * ((StageInfo::BLOCK_SIZE / 108.0f) * 0.6f);	// コインはブロックの大きさに対して、60%の大きさに設定
			_trans.size		= VOne * 108.0f;
			break;
		case STAGE_OBJECT_KIND::REMOTE_BOMB:

			_trans.scale	= VOne * ((StageInfo::BLOCK_SIZE / 10.0f) * 0.4f);	// 遠隔起爆爆弾はブロックの大きさに対して、40%の大きさに設定
			_trans.size		= VOne * 10.0f;
			break;
		default:

			_trans.scale	= VOne * StageInfo::BLOCK_SIZE / 100.0f;
			_trans.size		= VOne * 100.0f;
			break;
		}
	}

	/// <summary>
	///						ステージオブジェクトがナビメッシュを生成するかどうか
	/// </summary>
	/// <param name="_kind">ステージオブジェクトの種類			</param>
	/// <returns>			true:生成する / false:生成しない	</returns>
	inline bool IsStageObjectToNavMeshCreate(const STAGE_OBJECT_KIND& _kind)
	{
		switch (_kind)
		{
		case STAGE_OBJECT_KIND::GROUND_BLOCK:

			return true;
			break;
		}
		return false;
	}

	/// <summary>
	///						ステージオブジェクトがナビゲーションのノードを変化させる（影響をもっている）どうか
	/// </summary>
	/// <param name="_kind">ステージオブジェクトの種類				</param>
	/// <returns>			true:変化させる / false:変化させない	</returns>
	inline bool IsStageObjectToNavigationNodeChange(const STAGE_OBJECT_KIND& _kind)
	{
		switch (_kind)
		{
		case STAGE_OBJECT_KIND::SLIME_PATCH:
		case STAGE_OBJECT_KIND::SPIKE_BLOCK:
		case STAGE_OBJECT_KIND::CORE_BLOCK:
		case STAGE_OBJECT_KIND::AUTO_TURRET:
		case STAGE_OBJECT_KIND::ACTIVE_TURRET:
		case STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE:
		case STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD:
		case STAGE_OBJECT_KIND::COIN:
		case STAGE_OBJECT_KIND::JUMP_PAD:
		case STAGE_OBJECT_KIND::REMOTE_BOMB:

			return false;
			break;
		}
		return true;
	}

	// ステージオブジェクトのHPコンテナ
	inline const std::unordered_map<int, int> HP_CONFIG =
	{
		// 現在HPが必要ないものは0で初期化
		{ (int)STAGE_OBJECT_KIND::GROUND_BLOCK,			0	},
		{ (int)STAGE_OBJECT_KIND::WALL_BLOCK,			10	},
		{ (int)STAGE_OBJECT_KIND::SPIKE_BLOCK,			10	},
		{ (int)STAGE_OBJECT_KIND::SWAMP_BLOCK,			0	},
		{ (int)STAGE_OBJECT_KIND::CORE_BLOCK,			15	},
		{ (int)STAGE_OBJECT_KIND::COIN,					0	},
		{ (int)STAGE_OBJECT_KIND::AUTO_TURRET,			10	},
		{ (int)STAGE_OBJECT_KIND::ACTIVE_TURRET,		10	},
		{ (int)STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE,	10	},
		{ (int)STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD,	10	},
		{ (int)STAGE_OBJECT_KIND::SLIME_PATCH,			20	},
		{ (int)STAGE_OBJECT_KIND::JUMP_PAD,				20	},
		{ (int)STAGE_OBJECT_KIND::REMOTE_BOMB,			20	},
	};

	/// <summary>
	///							ステージオブジェクト種類ごとのHPを返す
	/// </summary>
	/// <param name="_kind">	ステージオブジェクトの種類	</param>
	/// <returns>				ステージオブジェクトのHP	</returns>
	inline const int& GetStageObjectHPConfig(const STAGE_OBJECT_KIND& _kind)
	{
		HasUnorderedMapContainerKey(HP_CONFIG, (int)_kind, true);

		return HP_CONFIG.at((int)_kind);
	}
}