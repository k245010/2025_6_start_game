#pragma once
#include "EnemyKind.h"
#include "../Stage/StageObjectData.h"
#include "../Navigation/NavigationManager/NavigationManager.h"
#include "../Collision/CollisionObjectKind.h"
#include "../Common/Function/Function.h"
//#include "../../Library/myDxLib.h"
#include <assert.h>
#include <unordered_map>
#include <set>

/// <summary>
/// 敵の移動データ
/// </summary>
namespace EnemyMoveInfo
{
	/// <summary>
	/// 移動スピードデータ
	/// </summary>
	struct MoveData
	{
		float walkSpeed;	// 地面を歩くスピード
		float climbSpeed;	// 壁を登るスピード

		/// <summary>
		/// 
		/// </summary>
		/// <param name="_walkSpeed"></param>
		/// <param name="_climbSpeed"></param>
		MoveData(const float& _walkSpeed, const float& _climbSpeed)
			: walkSpeed(_walkSpeed), climbSpeed(_climbSpeed) {
		}

		MoveData() : walkSpeed(0.0f), climbSpeed(0.0f) {}
	};

	/// <summary>
	///								MoveDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照					</param>
	/// <param name="_moveData">	MoveData					</param>
	inline void to_json(nlohmann::json& _json, const MoveData& _moveData)
	{
		_json = nlohmann::json
		{
			{"WalkSpeed",	_moveData.walkSpeed},
			{"ClimbSpeed",	_moveData.climbSpeed}
		};
	}

	/// <summary>
	///							jsonからMoveDataへ変換する
	/// </summary>
	/// <param name="_json">	json						</param>
	/// <param name="_moveData">MoveData参照				</param>
	inline void from_json(const nlohmann::json& _json, MoveData& _moveData)
	{
		_json.at("WalkSpeed").get_to(_moveData.walkSpeed);
		_json.at("ClimbSpeed").get_to(_moveData.climbSpeed);
	}
}

namespace EnemyModelSizeInfo
{
	/// <summary>
	/// モデルのサイズデータ
	/// </summary>
	struct ModelSizeData
	{
		VECTOR3 size;	// モデルのサイズ
		VECTOR3 scale;	// 描画するときのモデルのスケール

		/// <summary>
		/// コンストラクタ
		/// </summary>
		ModelSizeData() : size(VOne), scale(VOne) {};
	};

	/// <summary>
	///								ModelSizeDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照					</param>
	/// <param name="_modelSizeData">	ModelSizeData					</param>
	inline void to_json(nlohmann::json& _json, const ModelSizeData& _modelSizeData)
	{
		_json = nlohmann::json
		{
			{"ModelSize",	_modelSizeData.size		},
			{"ModelScale",	_modelSizeData.scale	},
		};
	}

	/// <summary>
	///									jsonからModelSizeDataへ変換する
	/// </summary>
	/// <param name="_json">			json						</param>
	/// <param name="_modelSizeData">	ModelSizeData参照			</param>
	inline void from_json(const nlohmann::json& _json, ModelSizeData& _modelSizeData)
	{
		_json.at("ModelSize").get_to(_modelSizeData.size);
		_json.at("ModelScale").get_to(_modelSizeData.scale);
	}
}

/// <summary>
/// 敵の攻撃データ
/// </summary>
namespace EnemyAttackInfo
{
	/// <summary>
	/// 攻撃データ
	/// </summary>
	struct AttackData
	{
		float attackInterval;			// 攻撃間隔
		float attackReach;				// 攻撃リーチ

		/// <summary>
		/// 
		/// </summary>
		/// <param name="_attackInterval"></param>
		/// <param name="_attackReach"></param>
		AttackData(const float& _attackInterval, const float& _attackReach)
			: attackInterval(_attackInterval), attackReach(_attackReach) {
		}

		AttackData() : attackInterval(0.0f), attackReach(0.0f) {}
	};

	/// <summary>
	///								AttackDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照					</param>
	/// <param name="_attackData">	AttackData					</param>
	inline void to_json(nlohmann::json& _json, const AttackData& _attackData)
	{
		_json = nlohmann::json
		{
			{"AttackInterval",	_attackData.attackInterval	},
			{"AttackReach",		_attackData.attackReach		},
		};
	}

	/// <summary>
	///								jsonからAttackDataへ変換する
	/// </summary>
	/// <param name="_json">		json						</param>
	/// <param name="_attackData">	AttackData参照				</param>
	inline void from_json(const nlohmann::json& _json, AttackData& _attackData)
	{
		_json.at("AttackInterval").get_to(_attackData.attackInterval);
		_json.at("AttackReach").get_to(_attackData.attackReach);
	}
}

/// <summary>
/// 敵の当たり判定データ
/// </summary>
namespace EnemyCollisionInfo
{
	/// <summary>
	/// 当たり判定データ
	/// </summary>
	struct CollisionData
	{
		//					key:当たり判定の種類 / value:相手に与えるダメージ量
		std::unordered_map<COLLISION_OBJECT_KIND, float> bodyCollTargetKindList;	// 体の当たり判定に対する当たる相手の当たり判定オブジェクトの種類コンテナ
		std::unordered_map<COLLISION_OBJECT_KIND, float> attackCollTargetKindList;	// 攻撃の当たり判定に対する当たる相手の当たり判定オブジェクトの種類コンテナ

		/// <summary>
		/// 
		/// </summary>
		/// <param name="_bodyCollTargetKindList"></param>
		/// <param name="_attackCollTargetKindList"></param>
		CollisionData(const std::unordered_map<COLLISION_OBJECT_KIND, float>& _bodyCollTargetKindList, const std::unordered_map<COLLISION_OBJECT_KIND, float>& _attackCollTargetKindList)
			: bodyCollTargetKindList(_bodyCollTargetKindList), attackCollTargetKindList(_attackCollTargetKindList) {
		}

		CollisionData() : bodyCollTargetKindList(std::unordered_map<COLLISION_OBJECT_KIND, float>{}), attackCollTargetKindList(std::unordered_map<COLLISION_OBJECT_KIND, float>{}) {}
	};

	/// <summary>
	///								CollisionDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照						</param>
	/// <param name="_collData">	CollisionData					</param>
	inline void to_json(nlohmann::json& _json, const CollisionData& _collData)
	{
		_json = nlohmann::json
		{
			{"BodyCollTargetKindList",			_collData.bodyCollTargetKindList	},
			{"AttackCollTargetKindList",		_collData.attackCollTargetKindList	},
		};
	}

	/// <summary>
	///							jsonからCollisionDataへ変換する
	/// </summary>
	/// <param name="_json">	json							</param>
	/// <param name="_collData">CollisionData参照				</param>
	inline void from_json(const nlohmann::json& _json, CollisionData& _collData)
	{
		_json.at("BodyCollTargetKindList").get_to(_collData.bodyCollTargetKindList);
		_json.at("AttackCollTargetKindList").get_to(_collData.attackCollTargetKindList);
	}
}

/// <summary>
/// 敵のナビゲーションデータ
/// </summary>
namespace EnemyNavigationInfo
{
	/// <summary>
	/// ナビゲーションデータ
	/// </summary>
	struct NavigationData
	{
		/// <summary>
		/// 経路探索して、たどり着く相手の種類
		/// </summary>
		enum class NAVIGATION_TARGET_KIND
		{
			CORE = 0,
			PLAYER,
			//BLOCKED_NAVPOINT,	// 潰されたナビポイント

			MAX,
		};

		NAVIGATION_TARGET_KIND navigationTargetKind;	// 経路探索したどり着く相手の種類
		int nodeUseKind;								// ナビゲーションで使用するノードの種類
		int navigationMode;								// ナビゲーションのモード　

		/// <summary>
		/// 
		/// </summary>
		/// <param name="_navTargetKind"></param>
		/// <param name="_nodeUseKind"></param>
		/// <param name="_navigationMode"></param>
		NavigationData(const NAVIGATION_TARGET_KIND& _navTargetKind, const int& _nodeUseKind, const int& _navigationMode) :
			navigationTargetKind(_navTargetKind), nodeUseKind(_nodeUseKind), navigationMode(_navigationMode) {
		}

		NavigationData() : navigationTargetKind(NAVIGATION_TARGET_KIND::CORE), nodeUseKind(0), navigationMode(0) {};
	};

	/// <summary>
	///								NavigationDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照						</param>
	/// <param name="_navData">		NavigationData					</param>
	inline void to_json(nlohmann::json& _json, const NavigationData& _navData)
	{
		_json = nlohmann::json
		{
			{"NavigationTargetKind",	_navData.navigationTargetKind	},
			{"NodeUseKind",				_navData.nodeUseKind			},
			{"NavigationMode",			_navData.navigationMode			},
		};
	}

	/// <summary>
	///							jsonからNavigationDataへ変換する
	/// </summary>
	/// <param name="_json">	json							</param>
	/// <param name="_navData">	NavigationData参照				</param>
	inline void from_json(const nlohmann::json& _json, NavigationData& _navData)
	{
		_json.at("NavigationTargetKind").get_to(_navData.navigationTargetKind);
		_json.at("NodeUseKind").get_to(_navData.nodeUseKind);
		_json.at("NavigationMode").get_to(_navData.navigationMode);
	}
}

/// <summary>
/// 敵が落とすコインデータ
/// </summary>
namespace EnemyDropCoinInfo
{
	/// <summary>
	/// ドロップするコインデータ
	/// </summary>
	struct DropCoinData
	{
		int dropNum;			// ドロップするコイン数
		int probability;		// コインをドロップする確率

		/// <summary>
		/// 
		/// </summary>
		/// <param name="_dropNum"></param>
		/// <param name="_probability"></param>
		DropCoinData(const int& _dropNum, const int& _probability) :
			dropNum(_dropNum), probability(_probability) {
		}

		DropCoinData() : dropNum(0), probability(0) {}
	};

	/// <summary>
	///								DropCoinDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照						</param>
	/// <param name="_dropCoinData">DropCoinData					</param>
	inline void to_json(nlohmann::json& _json, const DropCoinData& _dropCoinData)
	{
		_json = nlohmann::json
		{
			{"DropNum",		_dropCoinData.dropNum		},
			{"Probability",	_dropCoinData.probability	},
		};
	}

	/// <summary>
	///								jsonからDropCoinDataへ変換する
	/// </summary>
	/// <param name="_json">		json							</param>
	/// <param name="_dropCoinData">DropCoinData参照				</param>
	inline void from_json(const nlohmann::json& _json, DropCoinData& _dropCoinData)
	{
		_json.at("DropNum").get_to(_dropCoinData.dropNum);
		_json.at("Probability").get_to(_dropCoinData.probability);
	}
}

/// <summary>
/// 敵のデータ情報
/// </summary>
namespace EnemyInfo
{
	struct EnemyData
	{
		float hpMax;				// 最大HP
		EnemyMoveInfo::MoveData moveData;
		EnemyModelSizeInfo::ModelSizeData modelSizeData;
		EnemyAttackInfo::AttackData attackData;
		EnemyCollisionInfo::CollisionData collisionData;
		EnemyNavigationInfo::NavigationData navigationData;
		EnemyDropCoinInfo::DropCoinData dropCoinData;

		/// <summary>
		///									コンストラクタ
		/// </summary>
		/// <param name="_hpMax">			最大HP						</param>
		/// <param name="_moveData">		移動データ					</param>
		/// <param name="_moveData">		モデルサイズデータ			</param>
		/// <param name="_attackData">		攻撃データ					</param>
		/// <param name="_collisionData">	当たり判定データ			</param>
		/// <param name="_navigationData">	ナビゲーションデータ		</param>
		/// <param name="_dropCoinData">	ドロップするコインデータ	</param>
		EnemyData
		(
			const float& _hpMax,
			const EnemyMoveInfo::MoveData _moveData,
			const EnemyModelSizeInfo::ModelSizeData _modelSizeData,
			const EnemyAttackInfo::AttackData _attackData,
			const EnemyCollisionInfo::CollisionData _collisionData,
			const EnemyNavigationInfo::NavigationData _navigationData,
			const EnemyDropCoinInfo::DropCoinData _dropCoinData
		)
			: hpMax(_hpMax),
			moveData(_moveData),
			attackData(_attackData),
			collisionData(_collisionData),
			navigationData(_navigationData),
			dropCoinData(_dropCoinData) {}

		EnemyData() :
			hpMax(0.0f),
			moveData(EnemyMoveInfo::MoveData()),
			modelSizeData(EnemyModelSizeInfo::ModelSizeData()),
			attackData(EnemyAttackInfo::AttackData()),
			collisionData(EnemyCollisionInfo::CollisionData()),
			navigationData(EnemyNavigationInfo::NavigationData()),
			dropCoinData(EnemyDropCoinInfo::DropCoinData()) {}

			/// <summary>
			/// コンテナを解放する
			/// </summary>
		void ReleaseContainer()
		{
			// コンテナのクリア //
			collisionData.attackCollTargetKindList.clear();
			collisionData.bodyCollTargetKindList.clear();
		}
	};

	/// <summary>
	///								遮断物に対するDPSを返す	遮断物にダメージを与えることができない敵の場合は、-1.0を返す
	/// </summary>
	/// <param name="_enemyData">	敵のデータ																</param>
	/// <returns>					遮断物に対してダメージを与えれる:DPS / ダメージを与えれない:-1.0		</returns>
	inline float GetBlockedObjectDamagePerSecond(const EnemyData& _enemyData)
	{
		auto attackCollItr = _enemyData.collisionData.attackCollTargetKindList.find(COLLISION_OBJECT_KIND::WALL_BLOCK);

		// 遮断物である壁を壊すことができないとき　
		if (attackCollItr == _enemyData.collisionData.attackCollTargetKindList.end())
			return -1.0f;	// 攻撃できないので-1.0を返す

		return attackCollItr->second / _enemyData.attackData.attackInterval;	// DPSを計算し返す
	}

	/// <summary>
	///								EnemyDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照					</param>
	/// <param name="_enemyData">	EnemyData					</param>
	inline void to_json(nlohmann::json& _json, const EnemyData& _enemyData)
	{
		_json = nlohmann::json
		{
			{"HPMax",			_enemyData.hpMax			},
			{"MoveData",		_enemyData.moveData			},
			{"ModelSizeData",	_enemyData.modelSizeData	},
			{"AttackData",		_enemyData.attackData		},
			{"CollisionData",	_enemyData.collisionData	},
			{"NavigationData",	_enemyData.navigationData	},
			{"DropCoinData",	_enemyData.dropCoinData		},
		};
	}

	/// <summary>
	///								jsonからEnemyDataへ変換する
	/// </summary>
	/// <param name="_json">		json						</param>
	/// <param name="_enemyData">	EnemyData参照				</param>
	inline void from_json(const nlohmann::json& _json, EnemyData& _enemyData)
	{
		_json.at("HPMax").get_to(_enemyData.hpMax);
		_json.at("MoveData").get_to(_enemyData.moveData);
		_json.at("ModelSizeData").get_to(_enemyData.modelSizeData);
		_json.at("AttackData").get_to(_enemyData.attackData);
		_json.at("CollisionData").get_to(_enemyData.collisionData);
		_json.at("NavigationData").get_to(_enemyData.navigationData);
		_json.at("DropCoinData").get_to(_enemyData.dropCoinData);
	}
}

/// <summary>
/// 敵の移動速度データ
/// </summary>
//namespace EnemyMoveSpeedData
//{
//	/// <summary>
//	/// 敵の移動速度の設定
//	/// </summary>
//	inline const std::unordered_map<int, float> MOVE_SPEED_CONFIG =
//	{
//		//	key:敵の種類(int)		value:移動速度
//		{ (int)ENEMY_KIND::WALKER,	200.0f },
//		{ (int)ENEMY_KIND::CLIMBER,	150.0f },
//		{ (int)ENEMY_KIND::RUSHER,	300.0f },
//	};
//
//	/// <summary>
//	/// 敵の登る移動速度の設定
//	/// </summary>
//	inline const std::unordered_map<int, float> CLIMB_MOVE_SPEED_CONFIG =
//	{
//		//	key:敵の種類(int)		value:移動速度
//		{ (int)ENEMY_KIND::CLIMBER,	75.0f },
//	};
//
//	/// <summary>
//	///							敵の移動速度を返す
//	/// </summary>
//	/// <param name="_kind">	敵の種類				</param>
//	/// <returns>				移動速度				</returns>
//	inline const float& GetMoveSpeed(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(MOVE_SPEED_CONFIG, (int)_kind);
//
//		return MOVE_SPEED_CONFIG.at((int)_kind);
//	}
//
//	inline const float& GetClimbMoveSpeed(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(CLIMB_MOVE_SPEED_CONFIG, (int)_kind);
//
//		return CLIMB_MOVE_SPEED_CONFIG.at((int)_kind);
//	}
//}
//
///// <summary>
///// 敵の攻撃データ
///// </summary>
//namespace EnemyAttackDamageData
//{
//	/// <summary>
//	/// 敵の攻撃力に関するデータ
//	/// </summary>
//	struct AttackDamageData
//	{
//		ENEMY_KIND enemyKind;		// 敵の種類
//		float coreDamage;			// コアへのダメージ
//		float trapDamage;			// 罠へのダメージ
//		float trapAttackInterval;	// 罠への攻撃間隔
//
//		AttackDamageData(const ENEMY_KIND& _kind, const float& _coreDamage, const float& _trapDamage, const float& _trapAttackInterval)
//			: enemyKind(_kind), coreDamage(_coreDamage), trapDamage(_trapDamage), trapAttackInterval(_trapAttackInterval) {
//		}
//	};
//
//	/// <summary>
//	/// 攻撃力データ設定
//	/// key:敵の種類(int)	/	value:ダメージデータ
//	/// </summary>
//	inline const std::unordered_map<int, AttackDamageData> ATTACK_DAMAGE_CONFIG =
//	{
//		//	key:敵の種類(int)	value:攻撃ダメージデータ			敵の種類	コアダメージ	罠ダメージ	攻撃間隔
//		{	(int)ENEMY_KIND::WALKER,	AttackDamageData(ENEMY_KIND::WALKER,	1.0f,			1.0f,		2.0f)	},
//		{	(int)ENEMY_KIND::CLIMBER,	AttackDamageData(ENEMY_KIND::CLIMBER,	1.0f,			1.0f,		2.0f)	},
//		{	(int)ENEMY_KIND::RUSHER,	AttackDamageData(ENEMY_KIND::RUSHER,	0.5f,			0.5f,		2.0f)	},
//	};
//
//	/// <summary>
//	///							コアに対するダメージ量	
//	/// </summary>
//	/// <param name="_kind">	敵の種類　							</param>
//	/// <returns>				ダメージ量							</returns>
//	inline const float& GetCoreAttackDamage(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(ATTACK_DAMAGE_CONFIG, (int)_kind);
//
//		return ATTACK_DAMAGE_CONFIG.at((int)_kind).coreDamage;
//	}
//
//	/// <summary>
//	///							トラップに対するダメージ量
//	/// </summary>
//	/// <param name="_kind">	敵の種類　							</param>
//	/// <returns>				ダメージ量							</returns>
//	inline const float& GetTrapAttackDamage(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(ATTACK_DAMAGE_CONFIG, (int)_kind);
//
//		return ATTACK_DAMAGE_CONFIG.at((int)_kind).trapDamage;
//	}
//
//	/// <summary>
//	///							敵の攻撃間隔
//	/// </summary>
//	/// <param name="_kind">	敵の種類				</param>
//	/// <returns>				攻撃間隔				</returns>
//	inline const float& GetTrapAttackInterval(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(ATTACK_DAMAGE_CONFIG, (int)_kind);
//
//		return ATTACK_DAMAGE_CONFIG.at((int)_kind).trapAttackInterval;
//	}
//
//	/// <summary>
//	///							敵の罠に対する一秒間のダメージ量
//	/// </summary>
//	/// <param name="_kind">	敵の種類						</param>
//	/// <returns>				一秒間のダメージ量				</returns>
//	inline const float GetTrapDamagePerSecond(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(ATTACK_DAMAGE_CONFIG, (int)_kind);
//
//		return ATTACK_DAMAGE_CONFIG.at((int)_kind).trapDamage / ATTACK_DAMAGE_CONFIG.at((int)_kind).trapAttackInterval;
//	}
//}
//
//namespace EnemyNavigationData
//{
//	/// <summary>
//	/// 経路探索したどり着く相手の種類
//	/// </summary>
//	enum class ENEMY_NAV_TARGET_OBJ_KIND
//	{
//		CORE = 0,
//		PLAYER,
//
//		MAX,
//	};
//
//	/// <summary>
//	/// ナビゲーションデータ
//	/// </summary>
//	struct NavigationData
//	{
//		ENEMY_KIND enemyKind;	// 敵の種類
//		int nodeUseKind;		// ナビゲーションで使用するノードの種類
//		int navigationMode;		// ナビゲーションのモード　
//
//		NavigationData(const ENEMY_KIND& _kind, const int& _nodeUseKind, const int& _navigationMode) :
//			enemyKind(_kind), nodeUseKind(_nodeUseKind), navigationMode(_navigationMode) {
//		}
//	};
//
//	inline const std::unordered_map<int, NavigationData> NAVIGATION_CONFIG =
//	{
//		// key:敵の種類				value:ナビゲーションデータ 敵の種類		使用するノードの種類		ナビゲーションのモード
//		{(int)ENEMY_KIND::WALKER,	NavigationData(ENEMY_KIND::WALKER,	(int)NODE_USE_KIND::NORMAL, (int)NAVIGATION_MODE::IN_BLOCKED)},
//		{(int)ENEMY_KIND::CLIMBER,	NavigationData(ENEMY_KIND::CLIMBER, (int)NODE_USE_KIND::ALL,	(int)NAVIGATION_MODE::ACTIVE)},
//		{(int)ENEMY_KIND::RUSHER,	NavigationData(ENEMY_KIND::RUSHER,	(int)NODE_USE_KIND::NORMAL, (int)NAVIGATION_MODE::IN_BLOCKED)},
//	};
//
//	/// <summary>
//	///							ナビゲーションで使用するノードの種類を返す
//	/// </summary>
//	/// <param name="_kind">	敵の種類									</param>
//	/// <returns>				使用するノードの種類						</returns>
//	inline const int GetNavigationNodeUseKind(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(NAVIGATION_CONFIG, (int)_kind);
//
//		return NAVIGATION_CONFIG.at((int)_kind).nodeUseKind;
//	}
//
//	/// <summary>
//	///							ナビゲーションのモードを返す
//	/// </summary>
//	/// <param name="_kind">	敵の種類									</param>
//	/// <returns>				ナビゲーションのモード						</returns>
//	inline const int GetNavigationMode(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(NAVIGATION_CONFIG, (int)_kind);
//
//		return NAVIGATION_CONFIG.at((int)_kind).navigationMode;
//	}
//}
//
//namespace EnemyDropCoinData
//{
//	/// <summary>
//	/// ドロップするコインデータ
//	/// </summary>
//	struct DropCoinData
//	{
//		ENEMY_KIND enemyKind;	// 敵の種類
//		int dropNum;			// ドロップするコイン数
//		int probability;		// コインをドロップする確率
//
//		DropCoinData(const ENEMY_KIND& _kind, const int& _dropNum, const int& _probability) :
//			enemyKind(_kind), dropNum(_dropNum), probability(_probability) {
//		}
//	};
//
//	/// <summary>
//	/// ドロップするコインデータ
//	/// </summary>
//	inline const std::unordered_map<int, DropCoinData> DROP_COIN_CONFIG =
//	{
//		//	key:敵の種類(int)value:ドロップするコインデータ	敵の種類	ドロップするコイン数	ドロップする確率0～100%
//		{	(int)ENEMY_KIND::WALKER,	DropCoinData(ENEMY_KIND::WALKER,	1,						100)	},
//		{	(int)ENEMY_KIND::CLIMBER,	DropCoinData(ENEMY_KIND::CLIMBER,	1,						100)	},
//		{	(int)ENEMY_KIND::RUSHER,	DropCoinData(ENEMY_KIND::RUSHER,	1,						100)	},
//	};
//
//	/// <summary>
//	///							ドロップするコイン数を返す
//	/// </summary>
//	/// <param name="_kind">	敵の種類				</param>
//	/// <returns>				ドロップするコイン数	</returns>
//	inline int GetDropCoinNum(const ENEMY_KIND& _kind)
//	{
//		// キーが存在しない場合assertを実行する
//		HasUnorderedMapContainerKey(DROP_COIN_CONFIG, (int)_kind);
//
//		// コインをドロップする確率が100%だったら
//		if (DROP_COIN_CONFIG.at((int)_kind).probability == 100)
//			return DROP_COIN_CONFIG.at((int)_kind).dropNum;
//		// 乱数がコインを落とす確率以下だったら
//		if (DROP_COIN_CONFIG.at((int)_kind).probability <= (GetRand(99) + 1))
//			return DROP_COIN_CONFIG.at((int)_kind).dropNum;
//		return 0;
//	}
//}
