#pragma once
#include "Transform.h"
#include "../../nlohmann/json.hpp"

/// <summary>
///								Transform‚ğjson‚Ö•ÏŠ·‚·‚é
/// </summary>
/// <param name="_json">		jsonQÆ						</param>
/// <param name="_transform">	Transform						</param>
inline void to_json(nlohmann::json& _json, const Transform& _transform)
{
	_json = nlohmann::json
	{
		{ "Position",	_transform.position },
		{ "Rotation",	_transform.rotation },
		{ "Scale",		_transform.scale	},
		{ "Size",		_transform.size		}, 
	};
}

/// <summary>
///								Transform‚ğjson‚Ö•ÏŠ·‚·‚é
/// </summary>
/// <param name="_json">		jsonQÆ					</param>
/// <param name="_transform">	Transform					</param>
inline void from_json(const nlohmann::json& _json, Transform& _transform)
{
	_json.at("Position").get_to(_transform.position);
	_json.at("Rotation").get_to(_transform.rotation);
	_json.at("Scale").get_to(_transform.scale);
	_json.at("Size").get_to(_transform.size);
}