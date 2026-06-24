#pragma once
#include "../../Collision/Collisions/CollisionBase.h"
#include "../../Transform/Transform.h"
#include "../../Stage/StageObjectData.h"
#include "../../Common/Effekseer/EffekseerObjectManager.h"
#include "../../Common/Sound/SoundID.h"
#include "../../Transform/TransformJsonConverter.h"
#include "../../../nlohmann/json.hpp"
#include <set>

class Transform;
struct CollisionHitDamageData;

/// <summary>
/// 着弾点に召喚するステージオブジェクトの情報
/// </summary>
namespace SummonStageObjectInfo
{
	struct SummonStageObjectData
	{
		StageObjectData::STAGE_OBJECT_KIND stageObjectKind;	// 召喚するステージオブジェクトの種類
		Transform transform;								// 召喚するトランスフォーム

		SummonStageObjectData(const StageObjectData::STAGE_OBJECT_KIND& _stageObjectKind, const Transform& _transform)
			: stageObjectKind(_stageObjectKind), transform(_transform) {
		}

		SummonStageObjectData() : stageObjectKind(StageObjectData::STAGE_OBJECT_KIND::NONE), transform(Transform()) {}

		//_ 演算子の定義 _//

		bool operator< (const SummonStageObjectData& _other) const
		{
			return stageObjectKind < _other.stageObjectKind;
		}

		bool operator== (const SummonStageObjectData& _other) const
		{
			return stageObjectKind == _other.stageObjectKind;
		}
	};

	/// <summary>
	///											SummonStageObjectDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">					json参照									</param>
	/// <param name="_summonStageObjectData">	SummonStageObjectData						</param>
	inline void to_json(nlohmann::json& _json, const SummonStageObjectData& _summonStageObjectData)
	{
		_json = nlohmann::json
		{
			{ "StageObjectKind",	_summonStageObjectData.stageObjectKind	},
			{ "Transform",			_summonStageObjectData.transform		},
		};
	}

	/// <summary>
	///											SummonStageObjectDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">					json参照								</param>
	/// <param name="_summonStageObjectData">	SummonStageObjectData					</param>
	inline void from_json(const nlohmann::json& _json, SummonStageObjectData& _summonStageObjectData)
	{
		_json.at("StageObjectKind").get_to(_summonStageObjectData.stageObjectKind);
		_json.at("Transform").get_to(_summonStageObjectData.transform);
	}
}

// SummonStageObjectInfo::SummonStageObjectDataのハッシュの定義 トランスフォームは無視して、StageObjectData::StageObjectKindのみのハッシュを定義
namespace std
{
	// templateの特殊化（SummonStageObjectDataにだけ対応したテンプレート）
	template<>
	// ハッシュにSummonStageObjectDataを定義して、size_tでreturnするようにする
	struct hash<SummonStageObjectInfo::SummonStageObjectData>
	{
		// 関数呼び出し演算子 operator()  // noexcept その関数が例外を投げないの明示化
		size_t operator()(const SummonStageObjectInfo::SummonStageObjectData& summonStageObjData) const noexcept
		{
			return std::hash<int>()(static_cast<int>(summonStageObjData.stageObjectKind));
		}
	};
}

/// <summary>
/// 弾の情報
/// </summary>
namespace BulletInfo
{
	/// <summary>
	/// 弾のデータ
	/// </summary>
	struct BulletData
	{
		Transform transform = Transform();	// トランスフォーム　特に外部入出力の意味はない（BulletDataを使うクラス側の位置で決まるため）ので、ここで初期化
		float speed;						// 速度
		VECTOR3 friction;					// 抵抗値
		bool isGravity;						// 重力の影響を受けるかのフラグ
		float size;							// 直径
		float range;						// 射程距離
		float lifeTime;						// 生存時間
		std::unordered_map<COLLISION_OBJECT_KIND,float> targetDataList;						// 弾を当てる相手とダメージ量のコンテナ
		std::set<SummonStageObjectInfo::SummonStageObjectData> hitPosSummonStageObjectList;	// 着弾点に召喚するステージオブジェクトコンテナ
		std::set<Sound_ID::SOUND_ID> hitPosSoundIDList;										// 着弾時のサウンド
		EffekseerObjectManager::EFFEKSEER_KIND hitPosEffectKind;							// 着弾点のエフェクト

		/// <summary>
		///													コンストラクタ
		/// </summary> 
		/// <param name="_trans">							トランスフォーム									</param>
		/// <param name="_speed">							速度												</param>
		/// <param name="_friction">						抵抗値												</param>
		/// <param name="_isGravirty">						重力の影響を受けるかのフラグ						</param>
		/// <param name="_size">							直径												</param>
		/// <param name="_range">							射程距離											</param>
		/// <param name="_lifeTime">						生存時間											</param>
		/// <param name="_targetDataList">					弾を当てる相手とダメージ量のコンテナ				</param>
		/// <param name="_hitPosSummonStageObjectList">		着弾点に召喚するステージオブジェクトの種類コンテナ	</param>
		/// <param name="_hitPosSoundIDList">				着弾時のサウンド									</param>
		/// <param name="_hitPosEffectKind">				着弾点のエフェクト									</param>
		/// <param name="_kind">							弾の種類											</param>
		BulletData
		(
			const Transform& _trans,
			const float& _speed,
			const VECTOR3& _friction,
			bool _isGravity,
			const float& _size,
			const float& _range,
			const float& _lifeTime,
			const std::unordered_map<COLLISION_OBJECT_KIND, float>& _targetDataList,
			const std::set<SummonStageObjectInfo::SummonStageObjectData>& _hitPosSummonStageObjectList,
			const std::set<Sound_ID::SOUND_ID> _hitPosSoundIDList,
			const EffekseerObjectManager::EFFEKSEER_KIND _hitPosEffectKind
		)
			: transform(_trans),
			speed(_speed),
			friction(_friction),
			isGravity(_isGravity),
			size(_size),
			range(_range),
			lifeTime(_lifeTime),
			targetDataList(_targetDataList),
			hitPosSummonStageObjectList(_hitPosSummonStageObjectList),
			hitPosSoundIDList(_hitPosSoundIDList),
			hitPosEffectKind(_hitPosEffectKind)
		{};

		BulletData(const BulletData& _bulletData)
		{
			transform						= _bulletData.transform;
			speed							= _bulletData.speed;
			friction						= _bulletData.friction;
			isGravity						= _bulletData.isGravity;
			size							= _bulletData.size;
			range							= _bulletData.range;
			lifeTime						= _bulletData.lifeTime;
			targetDataList					= _bulletData.targetDataList;
			hitPosSummonStageObjectList		= _bulletData.hitPosSummonStageObjectList;
			hitPosSoundIDList				= _bulletData.hitPosSoundIDList;
			hitPosEffectKind				= _bulletData.hitPosEffectKind;
		}

		BulletData() :
			transform(Transform()),
			speed(0.0f),
			friction(VZero),
			isGravity(false),
			size(5.0f),
			range(5000.0f),
			lifeTime(99.0f),
			targetDataList(std::unordered_map<COLLISION_OBJECT_KIND, float>{ {COLLISION_OBJECT_KIND::ENEMY,10.0f} }),
			hitPosSummonStageObjectList(std::set<SummonStageObjectInfo::SummonStageObjectData>{}),
			hitPosSoundIDList(std::set<Sound_ID::SOUND_ID> {}),
			hitPosEffectKind(EffekseerObjectManager::EFFEKSEER_KIND::EF_NONE)
		{};
	};

	/// <summary>
	///								BulletDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照						</param>
	/// <param name="_bulletData">	BulletData						</param>
	inline void to_json(nlohmann::json& _json, const BulletData& _bulletData)
	{
		_json = nlohmann::json
		{
			{ "Speed",								_bulletData.speed							},
			{ "Friction",							_bulletData.friction						},
			{ "IsGravity",							_bulletData.isGravity						},
			{ "Size",								_bulletData.size							},
			{ "Range",								_bulletData.range							},
			{ "LifeTime",							_bulletData.lifeTime						},
			{ "TargetDataList",						_bulletData.targetDataList					},
			{ "HitPosSummonStageObjectList",		_bulletData.hitPosSummonStageObjectList		},
			{ "HitPosSoundIDList",					_bulletData.hitPosSoundIDList				},
			{ "HitPosEffectKind",					_bulletData.hitPosEffectKind				},
		};
	}

	/// <summary>
	///								BulletDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照					</param>
	/// <param name="_bulletData">	BulletData					</param>
	inline void from_json(const nlohmann::json& _json, BulletData& _bulletData)
	{
		_json.at("Speed").get_to(_bulletData.speed);
		_json.at("Friction").get_to(_bulletData.friction);
		_json.at("IsGravity").get_to(_bulletData.isGravity);
		_json.at("Size").get_to(_bulletData.size);
		_json.at("Range").get_to(_bulletData.range);
		_json.at("LifeTime").get_to(_bulletData.lifeTime);
		_json.at("TargetDataList").get_to(_bulletData.targetDataList);
		_json.at("HitPosSummonStageObjectList").get_to(_bulletData.hitPosSummonStageObjectList);
		_json.at("HitPosSoundIDList").get_to(_bulletData.hitPosSoundIDList);
		_json.at("HitPosEffectKind").get_to(_bulletData.hitPosEffectKind);
	}
}

