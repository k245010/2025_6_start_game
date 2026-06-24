#pragma once

/// <summary>
/// 敵の種類
/// </summary>
enum class ENEMY_KIND
{
	WALKER = 0,	// 地上を移動する敵
	CLIMBER,	// 地上と壁を移動する
	RUSHER,		// 地上を移動する敵
	CRUSHER,	// 地上を移動する敵

	MAX,
};
