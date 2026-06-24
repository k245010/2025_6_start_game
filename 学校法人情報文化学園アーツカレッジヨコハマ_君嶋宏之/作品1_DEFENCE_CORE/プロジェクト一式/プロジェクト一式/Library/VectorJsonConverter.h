#pragma once
#include "../nlohmann/json.hpp"
#include "myDxLib.h"

// VECTOR3は名前空間でto_json,from_jsonを定義しないと使えなかったので、ここでの定義はしないで、名前空間myDxlibで定義する ---------------------------

/*
/// <summary>
///							VECTOR3をjsonへ変換する
/// </summary>
/// <param name="_json">	json参照		</param>
/// <param name="_vector">	VECTOR3			</param>
inline void to_json(nlohmann::json& _json, const VECTOR3& _vector)
{
	_json = nlohmann::json
	{
		{ "posX", _vector.x },
		{ "posY", _vector.y },
		{ "posZ", _vector.z },
	};
}

/// <summary>
///							jsonからVECTOR3へ変換する
/// </summary>
/// <param name="_json">	json						</param>
/// <param name="_vector">	VECTOR3参照					</param>
inline void from_json(const nlohmann::json& _json, VECTOR3& _vector)
{
	_json.at("posX").get_to(_vector.x);
	_json.at("posY").get_to(_vector.y);
	_json.at("posZ").get_to(_vector.z);
}
*/

// -------------------------------------------------------------------------------------------------------------------------------------------------

/// <summary>
///							VECTOR3をjsonへ変換する
/// </summary>
/// <param name="_json">	json参照		</param>
/// <param name="_vector">	VECTOR3			</param>
inline void to_json(nlohmann::json& _json, const VECTOR3I& _vector)
{
	_json = nlohmann::json
	{
		{ "posX", _vector.x },
		{ "posY", _vector.y },
		{ "posZ", _vector.z },
	};
}

/// <summary>
///							jsonからVECTOR3へ変換する
/// </summary>
/// <param name="_json">	json						</param>
/// <param name="_vector">	VECTOR3参照					</param>
inline void from_json(const nlohmann::json& _json, VECTOR3I& _vector)
{
	_json.at("posX").get_to(_vector.x);
	_json.at("posY").get_to(_vector.y);
	_json.at("posZ").get_to(_vector.z);
}
