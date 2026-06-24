#pragma once
#include "../../../Library/GameObject.h"
#include "../../../nlohmann/json.hpp"
#include "../Color/Color.h"
#include "../../../Library/VectorJsonConverter.h"

namespace LightInfo
{
	const VECTOR3 DEFAULT_LIGHT_DIR = VECTOR3(0.05f, -0.07f, -0.06f);	// ライトの向き

	struct LightData
	{
		VECTOR3 position;	// ライト座標
		VECTOR3 direction;	// ライト方向

		Color ambColor;		// アンビエントカラー
		Color spcColor;		// スペキュラカラー
		Color difColor;		// ディフューズカラー

		LightData() : position(VZero), direction(VZero), ambColor(Colors::WHITE), spcColor(Colors::WHITE), difColor(Colors::WHITE) {}
		LightData
		(
			const VECTOR3& _pos,
			const VECTOR3& _dir,
			const Color& _ambColor,
			const Color& _spcColor,
			const Color& _difColor
		)
			: position(_pos),
			direction(_dir),
			ambColor(_ambColor),
			spcColor(_spcColor),
			difColor(_difColor)
		{
		}
	};

	/// <summary>
	///							LightDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">	json参照				</param>
	/// <param name="_light">	LightData				</param>
	inline void to_json(nlohmann::json& _json, const LightData& _light)
	{
		_json = nlohmann::json
		{
			{"Position",	_light.position	},
			{"Direction",	_light.direction},
			{"AmbColor",	_light.ambColor	},
			{"SpcColor",	_light.spcColor	},
			{"DifColor",	_light.difColor	},
		};
	}

	/// <summary>
	///							jsonからLightDataへ変換する
	/// </summary>
	/// <param name="_json">	json						</param>
	/// <param name="_light">	LightData参照				</param>
	inline void from_json(const nlohmann::json& _json, LightData& _light)
	{
		_json.at("Position").get_to(_light.position);
		_json.at("Direction").get_to(_light.direction);
		_json.at("AmbColor").get_to(_light.ambColor);
		_json.at("SpcColor").get_to(_light.spcColor);
		_json.at("DifColor").get_to(_light.difColor);
	}
}