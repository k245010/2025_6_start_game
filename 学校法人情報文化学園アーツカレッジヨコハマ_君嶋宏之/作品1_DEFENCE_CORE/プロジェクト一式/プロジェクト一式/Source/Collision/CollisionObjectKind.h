#pragma once
#include <unordered_map>
//#include <map>
//#include <vector>
//#include <string>

/// <summary>
/// 当たり判定オブジェクトの種類　当たり判定をするオブジェクトの種類分必要
/// </summary>
enum class COLLISION_OBJECT_KIND
{
	NONE = 0,
	GROUND_BLOCK,
	SPIKE_BLOCK,
	WALL_BLOCK,
	CORE_BLOCK,
	SLIME_PATCH,
	TURRET,
	TURRET_RAY,
	BULLET,
	COIN,
	JUMP_PAD,
	BOMB,
	BOMB_EXPLOSION,
	ENEMY,
	ENEMY_ATTACK,
	PLAYER,
	BACK_BLOCK,

	ERASER,

	MAX
};

/// <summary>
/// 当たり判定のオブジェクトの種類の情報
/// </summary>
namespace CollisionObjectKindInfo
{
	inline const std::unordered_map<COLLISION_OBJECT_KIND, std::string> COLLISION_OBJECT_KIND_STRING_LIST =	// enumのCOLLISION_OBJECT_KINDを文字列として持つコンテナ
	{
		{COLLISION_OBJECT_KIND::NONE,			"NONE"			},
		{COLLISION_OBJECT_KIND::GROUND_BLOCK,	"GROUND_BLOCK"	},
		{COLLISION_OBJECT_KIND::SPIKE_BLOCK,	"SPIKE_BLOCK"	},
		{COLLISION_OBJECT_KIND::WALL_BLOCK,		"WALL_BLOCK"	},
		{COLLISION_OBJECT_KIND::CORE_BLOCK,		"CORE_BLOCK"	},
		{COLLISION_OBJECT_KIND::SLIME_PATCH,	"SLIME_PATCH"	},
		{COLLISION_OBJECT_KIND::TURRET,			"TURRET"		},
		{COLLISION_OBJECT_KIND::TURRET_RAY,		"TURRET_RAY"	},
		{COLLISION_OBJECT_KIND::BULLET,			"BULLET"		},
		{COLLISION_OBJECT_KIND::COIN,			"COIN"			},
		{COLLISION_OBJECT_KIND::JUMP_PAD,		"JUMP_PAD"		},
		{COLLISION_OBJECT_KIND::BOMB,			"BOMB"			},
		{COLLISION_OBJECT_KIND::BOMB_EXPLOSION,	"BOMB_EXPLOSION"},
		{COLLISION_OBJECT_KIND::ENEMY,			"ENEMY"			},
		{COLLISION_OBJECT_KIND::ENEMY_ATTACK,	"ENEMY_ATTACK"	},
		{COLLISION_OBJECT_KIND::PLAYER,			"PLAYER"		},
		{COLLISION_OBJECT_KIND::ERASER,			"ERASER"		},
	};

	/// <summary>
	///						当たり判定オブジェクトの種類に対して、対応する文字列を返す
	/// </summary>
	/// <param name="_kind">当たり判定オブジェクトの種類	</param>
	/// <returns>			対応する文字列					</returns>
	inline std::string GetToString(const COLLISION_OBJECT_KIND& _kind)
	{
		if (COLLISION_OBJECT_KIND_STRING_LIST.find(_kind) == COLLISION_OBJECT_KIND_STRING_LIST.end())
			assert(false && "不正なCOLLISION_OBJECT_KINDです");

		return COLLISION_OBJECT_KIND_STRING_LIST.at(_kind);	// _kindに対する文字列を返す
	}

	/// <summary>
	///							当たり判定オブジェクトの種類の文字列に対して、対応する列挙体を返す
	/// </summary>
	/// <param name="_kindStr">	当たり判定オブジェクトの種類の文字列	</param>
	/// <returns>				COLLISION_OBJECT_KIND					</returns>
	inline COLLISION_OBJECT_KIND GetFromEnum(const std::string_view _kindStr)
	{
		for (const auto& collisionObjKindStr : COLLISION_OBJECT_KIND_STRING_LIST)
		{
			// 文字列が一致したら
			if (collisionObjKindStr.second == _kindStr)
			{
				return collisionObjKindStr.first;	// 文字列が見つかったので列挙体を返す
			}
		}
		assert(false && "不正なのCOLLISION_OBJECT_KIND文字列です");
		return COLLISION_OBJECT_KIND::NONE;
	}

	//inline std::string GetToStringContainer(const std::list<COLLISION_OBJECT_KIND> _list);
	//inline std::string GetToStringContainer(const std::vector<COLLISION_OBJECT_KIND> _list);

	//template<class T, typename = std::enable_if_t<std::is_same_v<typename T::value_type, std::string>>>

	//template<typename ContainerIterator>
	//inline std::vector<std::string> GetToStringContainer(ContainerIterator&& _container,const size_t _containerSize)
	//{
	//	std::vector<std::string> stringList;	// 当たり判定オブジェクトの種類を文字列に変えて保存するコンテナ
	//	int loopCount = 0;

	//	while(loopCount++ < _containerSize)
	//	{
	//		// 
	//		stringList.emplace_back(GetToString(_container++));
	//	}
	//	return stringList;
	//}
	
}

