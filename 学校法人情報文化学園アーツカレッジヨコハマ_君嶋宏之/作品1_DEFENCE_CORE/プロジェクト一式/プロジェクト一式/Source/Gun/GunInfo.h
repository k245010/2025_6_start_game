#pragma once
#include "../Shot/Bullet/BulletInfo.h"

/// <summary>
/// Gunの情報
/// </summary>
namespace GunInfo
{
	/// <summary>
	/// 銃のデータ
	/// </summary>
	struct GunData
	{
		BulletInfo::BulletData bulletData;						// 弾のデータ
		int ammo;												// 現在マガジンに入っているの弾数
		int holdBullet;											// ammoを除いた弾の保持数
		int magazineSize;										// １マガジンのサイズ
		float fireRate;											// 連射率　数値が小さいほど連射できる
		float reloadTime;										// リロード時間
		float spread;											// 拡散量　ショットガンなどの場合、数値が大きいほど拡散する　ラジアン値
		int palette;											// パレット数　ショットガンなどの場合複数同時発射なので、この数を増やす
		EffekseerObjectManager::EFFEKSEER_KIND fireEffectKind;	// 発射時の銃口のエフェクトの種類
		std::set<Sound_ID::SOUND_ID> shotSoundIDList;			// 発射音のサウンドIDコンテナ	複数要素の場合はランダムでそれぞれの音を再生

		/// <summary>
		///									コンストラクタ
		/// </summary>
		/// <param name="_bulletData">		弾のデータ																	</param>
		/// <param name="_ammo">			現在マガジンに入っているの弾数												</param>
		/// <param name="_holdBullet">		弾の最大保持数																</param>
		/// <param name="_magazineSize">	１マガジンのサイズ															</param>
		/// <param name="_damage">			ダメージ量																	</param>
		/// <param name="_fireRate">		連射率　数値が大きいほど連射できる											</param>
		/// <param name="_reloadTime">		リロード時間																</param>
		/// <param name="_spread">			拡散量　ショットガンなどの場合、数値が大きいほど拡散する　ラジアン値		</param>
		/// <param name="_palette">			パレット数　ショットガンなどの場合複数同時発射なので、この数を増やす		</param>
		/// <param name="_fireEffectKind">	発射時の銃口のエフェクトの種類												</param>
		/// <param name="_shotSoundIDList">	発射音のサウンドIDコンテナ	複数要素の場合はランダムでそれぞれの音を再生	</param>
		GunData
		(
			const BulletInfo::BulletData& _bulletData,
			const int& _ammo,
			const int& _holdBullet,
			const int& _magazineSize,
			const float& _fireRate,
			const float& _reloadTime,
			const float& _spread = 0.0f,
			const int& _palette = 1,
			const EffekseerObjectManager::EFFEKSEER_KIND& _fireEffectKind = EffekseerObjectManager::EFFEKSEER_KIND::EF_NONE,
			const std::set<Sound_ID::SOUND_ID> _shotSoundIDList = std::set<Sound_ID::SOUND_ID>{ Sound_ID::SOUND_ID::NORMAL_BULLET_SHOT_SE }
		)
			: bulletData(_bulletData),
			ammo(_ammo),
			holdBullet(_holdBullet),
			magazineSize(_magazineSize),
			fireRate(_fireRate),
			reloadTime(_reloadTime),
			spread(_spread),
			palette(_palette),
			fireEffectKind(_fireEffectKind),
			shotSoundIDList(_shotSoundIDList)
		{
		};

		GunData(const GunData& _gunData)
		{
			bulletData		= _gunData.bulletData;
			ammo			= _gunData.ammo;
			holdBullet		= _gunData.holdBullet;
			magazineSize	= _gunData.magazineSize;
			fireRate		= _gunData.fireRate;
			reloadTime		= _gunData.reloadTime;
			spread			= _gunData.spread;
			palette			= _gunData.palette;
			fireEffectKind	= _gunData.fireEffectKind;
			shotSoundIDList = _gunData.shotSoundIDList;
		}

		GunData() :
			bulletData(BulletInfo::BulletData()),
			ammo(0),
			holdBullet(30),
			magazineSize(10),
			fireRate(5.0f),
			reloadTime(1.0f),
			spread(0.0f),
			palette(1),
			fireEffectKind(EffekseerObjectManager::EFFEKSEER_KIND::EF_NONE),
			shotSoundIDList(std::set<Sound_ID::SOUND_ID>{ Sound_ID::SOUND_ID::NORMAL_BULLET_SHOT_SE })
		{
		};

		/// <summary>
		/// コンテナを解放する
		/// </summary>
		void ReleaseContainer()
		{
			// コンテナのクリア //
			bulletData.hitPosSoundIDList.clear();
			bulletData.targetDataList.clear();
			bulletData.hitPosSummonStageObjectList.clear();
			shotSoundIDList.clear();
		}
	};

	/// <summary>
	///								GunDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照					</param>
	/// <param name="_gunData">		GunData						</param>
	inline void to_json(nlohmann::json& _json, const GunData& _gunData)
	{
		_json = nlohmann::json
		{
			{ "BulletData",		_gunData.bulletData		},
			{ "Ammo",			_gunData.ammo			},
			{ "HoldBullet",		_gunData.holdBullet		},
			{ "MagazineSize",	_gunData.magazineSize	},
			{ "FireRate",		_gunData.fireRate		},
			{ "ReloadTime",		_gunData.reloadTime		},
			{ "Spread",			_gunData.spread			},
			{ "Palette",		_gunData.palette		},
			{ "FireEffectKind",	_gunData.fireEffectKind	},
			{ "ShotSoundIDList",_gunData.shotSoundIDList},
		};
	}

	/// <summary>
	///							jsonからGunDataへ変換する
	/// </summary>
	/// <param name="_json">	json						</param>
	/// <param name="_gunData">	GunData参照					</param>
	inline void from_json(const nlohmann::json& _json, GunData& _gunData)
	{
		_json.at("BulletData").get_to(_gunData.bulletData);
		_json.at("Ammo").get_to(_gunData.ammo);
		_json.at("HoldBullet").get_to(_gunData.holdBullet);
		_json.at("MagazineSize").get_to(_gunData.magazineSize);
		_json.at("FireRate").get_to(_gunData.fireRate);
		_json.at("ReloadTime").get_to(_gunData.reloadTime);
		_json.at("Spread").get_to(_gunData.spread);
		_json.at("Palette").get_to(_gunData.palette);
		_json.at("FireEffectKind").get_to(_gunData.fireEffectKind);
		_json.at("ShotSoundIDList").get_to(_gunData.shotSoundIDList);
	}
}