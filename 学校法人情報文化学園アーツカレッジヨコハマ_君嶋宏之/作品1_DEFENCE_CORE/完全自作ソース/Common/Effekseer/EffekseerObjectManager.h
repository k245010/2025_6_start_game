#pragma once
#include "EffekseerObject.h"
#include "../../../Library/myDxLib.h"
#include <unordered_map>

/// <summary>
/// EffekseerObjectの管理をする
/// </summary>
namespace EffekseerObjectManager
{
	/// <summary>
	/// エフェクトの種類
	/// </summary>
	enum EFFEKSEER_KIND
	{
		EF_NONE = -1,

		EF_ENEMY_SPAWN,
		EF_ENEMY_DEATH,
		EF_ENEMY_HIT,
		EF_PLAYER_HIT,
		EF_TRAP_PUT,
		EF_TRAP_UPGRADE,
		EF_CORE_HIT,
		EF_WALL_BROKEN,
		EF_SLIME_SLOW_DOWN,
		EF_EXPLOSION,

		EF_MAX,
	};

	inline const std::unordered_map<EFFEKSEER_KIND, std::string> EFFEKSEER_KIND_STRING_LIST =
	{
		{EFFEKSEER_KIND::EF_NONE,				"NONE"				},
		{EFFEKSEER_KIND::EF_ENEMY_SPAWN,		"ENEMY_SPAWN"		},
		{EFFEKSEER_KIND::EF_ENEMY_DEATH,		"ENEMY_DEATH"		},
		{EFFEKSEER_KIND::EF_ENEMY_HIT,			"ENEMY_HIT"			},
		{EFFEKSEER_KIND::EF_PLAYER_HIT,			"PLAYER_HIT"		},
		{EFFEKSEER_KIND::EF_TRAP_PUT,			"TRAP_PUT"			},
		{EFFEKSEER_KIND::EF_TRAP_UPGRADE,		"TRAP_UPGRADE"		},
		{EFFEKSEER_KIND::EF_CORE_HIT,			"CORE_HIT"			},
		{EFFEKSEER_KIND::EF_WALL_BROKEN,		"WALL_BROKEN"		},
		{EFFEKSEER_KIND::EF_SLIME_SLOW_DOWN,	"SLIME_SLOW_DOWN"	},
		{EFFEKSEER_KIND::EF_EXPLOSION,			"EXPLOSION"			},
	};

	/// <summary>
	///						エフェクトの種類に対して、対応する文字列を返す
	/// </summary>
	/// <param name="_kind">エフェクトの種類		</param>
	/// <returns>			対応する文字列			</returns>
	inline std::string GetToString(const EFFEKSEER_KIND& _kind)
	{
		if (EFFEKSEER_KIND_STRING_LIST.find(_kind) == EFFEKSEER_KIND_STRING_LIST.end())
			assert(false && "不正なEFFEKSEER_KINDです");

		return EFFEKSEER_KIND_STRING_LIST.at(_kind);	// _kindに対する文字列を返す
	}

	/// <summary>
	///							エフェクトの種類の文字列に対して、対応する列挙体を返す
	/// </summary>
	/// <param name="_kindStr">	エフェクトの種類の文字列	</param>
	/// <returns>				EFFEKSEER_KIND				</returns>
	EFFEKSEER_KIND GetFromEnum(std::string_view _kindStr);

	/// <summary>
	/// エフェクトのロード初期化
	/// </summary>
	void Init();

	void Update();
	void Draw();

	/// <summary>
	/// インスタンスとハンドルの削除
	/// </summary>
	void Release();

	/// <summary>
	/// インスタンスの削除
	/// </summary>
	/// <param name="_effekseerObj"> インスタンスポインタ </param>
	void PopEffect(EffekseerObject* _effekseerObj);

	/// <summary>
	/// 全てのインスタンスの削除
	/// </summary>
	void DeleteAllEffect();

	/// <summary>
	///							エフェクトを生成
	/// </summary>
	/// <param name="_pPos">	生成座標														</param>
	/// <param name="_id">		エフェクトの種類												</param>
	/// <param name="_spped">	再生速度														</param>
	/// <param name="_parent">	追従するための座標ポインタ										</param>
	/// <param name="_change">	追従座標ポインタに対する変更座標 (描画座標ポインタに加算する)	</param>
	void SetEffect(const VECTOR3& _pPos, const int& _id, const float& _spped = 1.0f, const VECTOR3* _parent = nullptr,const VECTOR3& _change = VZero);
}