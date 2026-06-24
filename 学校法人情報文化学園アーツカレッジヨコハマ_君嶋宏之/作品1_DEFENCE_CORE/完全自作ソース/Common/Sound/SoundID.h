#pragma once
#include <string>
#include <unordered_map>
#include <assert.h>

/** 西 作
SoundId.cpp,h
**/

namespace Sound_ID {

	/// <summary>
	/// サウンドのID
	/// </summary>
	enum SOUND_ID
	{
		//_ BGM _//

		TITLE_BGM = 1000,
		VICTORY_BGM,
		DEFEAT_BGM,
		SETUP_BGM,
		DEFENCE_BGM,
		
		//_ ジングル _//

		VICTORY_JINGLE = 2000,
		DEFEAT_JINGLE,
		WAVE_START_JINGLE,
		WAVE_END_JINGLE,

		//_ SE _//

		// UI関連　
		SELECT_PUSH_SE = 3000,
		TRAP_SELECTING_SE,
		CHANGE_START_STAGE_NUM_SE,
		DO_NOT_SE,

		// GAME関連
		SPIKE_PUT_SE = 3010,
		WALL_PUT_SE,
		TURRET_PUT_SE,
		NORMAL_BULLET_SHOT_SE,
		SLIME_BULLET_SHOT_SE,
		SLIME_BULLET_HIT_SE,
		SLIME_BULLET_ATTACH_SE,
		BULLET_EMPTY_SE,

		WALL_DAMAGE1_SE = 3018,
		WALL_DAMAGE2_SE,
		WALL_BROKEN_SE,
		CORE_DAMAGE_SE,
		CORE_BROKEN_SE,

		ENEMY_DAMAGE_SE = 3024,
		ENEMY_ATTACK_SE,
		ENEMY_DEATH_SE,

		PLAYER_WALK_SE = 3028,
		PLAYER_DAMAGE_SE,
		PLAYER_DEATH_SE,

		COIN_ADD_SE = 3032,
		COIN_GET_SE,
		COIN_SUB_SE,

		TRAP_UPGRADE_SE = 3036,
		TRAP_MAINTENANCE_SE,

		SOUND_ID_END	// SOUND_IDの末尾
	};

	inline const std::unordered_map<SOUND_ID, std::string> SOUND_ID_STRING_LIST =
	{
		{SOUND_ID::TITLE_BGM,					"TITLE_BGM"					},
		{SOUND_ID::VICTORY_BGM,					"VICTORY_BGM"				},
		{SOUND_ID::DEFEAT_BGM,					"DEFEAT_BGM"				},
		{SOUND_ID::SETUP_BGM,					"SETUP_BGM"					},
		{SOUND_ID::DEFENCE_BGM,					"DEFENCE_BGM"				},
		{SOUND_ID::VICTORY_JINGLE,				"VICTORY_JINGLE"			},
		{SOUND_ID::DEFEAT_JINGLE,				"DEFEAT_JINGLE"				},
		{SOUND_ID::WAVE_START_JINGLE,			"WAVE_START_JINGLE"			},
		{SOUND_ID::WAVE_END_JINGLE,				"WAVE_END_JINGLE"			},
		{SOUND_ID::SELECT_PUSH_SE,				"SELECT_PUSH_SE"			},
		{SOUND_ID::TRAP_SELECTING_SE,			"TRAP_SELECTING_SE"			},
		{SOUND_ID::CHANGE_START_STAGE_NUM_SE,	"CHANGE_START_STAGE_NUM_SE"	},
		{SOUND_ID::DO_NOT_SE,					"DO_NOT_SE"					},
		{SOUND_ID::SPIKE_PUT_SE,				"SPIKE_PUT_SE"				},
		{SOUND_ID::WALL_PUT_SE,					"WALL_PUT_SE"				},
		{SOUND_ID::TURRET_PUT_SE,				"TURRET_PUT_SE"				},
		{SOUND_ID::NORMAL_BULLET_SHOT_SE,		"NORMAL_BULLET_SHOT_SE"		},
		{SOUND_ID::SLIME_BULLET_SHOT_SE,		"SLIME_BULLET_SHOT_SE"		},
		{SOUND_ID::SLIME_BULLET_HIT_SE,			"SLIME_BULLET_HIT_SE"		},
		{SOUND_ID::SLIME_BULLET_ATTACH_SE,		"SLIME_BULLET_ATTACH_SE"	},
		{SOUND_ID::BULLET_EMPTY_SE,				"BULLET_EMPTY_SE"			},
		{SOUND_ID::WALL_DAMAGE1_SE,				"WALL_DAMAGE1_SE"			},
		{SOUND_ID::WALL_DAMAGE2_SE,				"WALL_DAMAGE2_SE"			},
		{SOUND_ID::WALL_BROKEN_SE,				"WALL_BROKEN_SE"			},
		{SOUND_ID::CORE_DAMAGE_SE,				"CORE_DAMAGE_SE"			},
		{SOUND_ID::CORE_BROKEN_SE,				"CORE_BROKEN_SE"			},
		{SOUND_ID::ENEMY_DAMAGE_SE,				"ENEMY_DAMAGE_SE"			},
		{SOUND_ID::ENEMY_ATTACK_SE,				"ENEMY_ATTACK_SE"			},
		{SOUND_ID::ENEMY_DEATH_SE,				"ENEMY_DEATH_SE"			},
		{SOUND_ID::PLAYER_WALK_SE,				"PLAYER_WALK_SE"			},
		{SOUND_ID::PLAYER_DAMAGE_SE,			"PLAYER_DAMAGE_SE"			},
		{SOUND_ID::PLAYER_DEATH_SE,				"PLAYER_DEATH_SE"			},
		{SOUND_ID::COIN_ADD_SE,					"COIN_ADD_SE"				},
		{SOUND_ID::COIN_GET_SE,					"COIN_GET_SE"				},
		{SOUND_ID::COIN_SUB_SE,					"COIN_SUB_SE"				},
		{SOUND_ID::TRAP_UPGRADE_SE,				"TRAP_UPGRADE_SE"			},
		{SOUND_ID::TRAP_MAINTENANCE_SE,			"TRAP_MAINTENANCE_SE"		},
	};

	/// <summary>
	///						サウンドのIDに対して、対応する文字列を返す
	/// </summary>
	/// <param name="_kind">サウンドのID					</param>
	/// <returns>			対応する文字列					</returns>
	inline std::string GetToString(const SOUND_ID& _kind)
	{
		if (SOUND_ID_STRING_LIST.find(_kind) == SOUND_ID_STRING_LIST.end())
			assert(false && "不正なSOUND_IDです");

		return SOUND_ID_STRING_LIST.at(_kind);	// _kindに対する文字列を返す
	}

	/// <summary>
	///							サウンドIDの種類の文字列に対して、対応する列挙体を返す
	/// </summary>
	/// <param name="_kindStr">	サウンドIDの種類の文字列	</param>
	/// <returns>				EFFEKSEER_KIND				</returns>
	SOUND_ID GetFromEnum(std::string_view _kindStr);

	void InitID();
	
	/// <summary>
	/// サウンドのSOUND_ID列挙体と音量のペアコンテナを削除する
	/// </summary>
	void DeleteSoundID();

	std::string GetSoundID(SOUND_ID _type);

	void SetSoundId(std::string	_sound, SOUND_ID _type);
}