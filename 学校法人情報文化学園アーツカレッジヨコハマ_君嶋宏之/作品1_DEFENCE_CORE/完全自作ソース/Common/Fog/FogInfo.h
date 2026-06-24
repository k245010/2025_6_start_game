#pragma once
#include "../Color/color.h"
#include "../../../nlohmann/json.hpp"

namespace FogInfo
{
	/// <summary>
	/// フォグデータ　DxLibのDX_FOGMODE_EXPフォグモード想定で設計
	/// </summary>
	struct FogData
	{
		float density;	// フォグの濃さ
		Color color;	// フォグの色

		/// <summary>
		/// コンストラクタ
		/// </summary>
		FogData()											: density(0.0f), color(Color(Colors::WHITE)) {}

		/// <summary>
		///							コンストラクタ
		/// </summary>
		/// <param name="_density">	フォグの濃さ	</param>
		/// <param name="_color">	フォグの色　	</param>
		FogData(const float& _density, const Color& _color) : density(_density), color(_color)	 {}
	};

	/// <summary>
	///								FogDataをjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照				</param>
	/// <param name="_fogData">		FogData					</param>
	inline void to_json(nlohmann::json& _json, const FogData& _fogData)
	{
		_json = nlohmann::json
		{
			{"Density",	_fogData.density	},
			{"Color",	_fogData.color		},
		};
	}

	/// <summary>
	///							jsonからFogDataへ変換する
	/// </summary>
	/// <param name="_json">	json					</param>
	/// <param name="_fogData">	FogData参照				</param>
	inline void from_json(const nlohmann::json& _json, FogData& _fogData)
	{
		_json.at("Density").get_to(_fogData.density);
		_json.at("Color").get_to(_fogData.color);
	}
}