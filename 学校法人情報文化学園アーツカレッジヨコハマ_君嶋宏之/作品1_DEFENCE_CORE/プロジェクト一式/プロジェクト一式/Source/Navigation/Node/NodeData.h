#pragma once

/// <summary>
/// 接続相手の接続の仕方の種類
/// </summary>
enum class TARGET_CONNECT_KIND
{
	NORMAL = 0,		// 通常接続
	SPACE,			// 地続きでない空間接続

	MAX,
};

// ToDo 今後はビットシフトを利用することで、複数パターンのNODE_USE_KINDを一つのintで管理できるようにする
/// <summary>
/// ノードの使用用途種類
/// </summary>
enum class NODE_USE_KIND
{
	NORMAL = 0,	// 通常（法線が0,1,0）
	WALL,		// 壁　（法線が0,1,0でない）

	ALL,		// 上記全て
};

/// <summary>
/// ノードがオブジェクトに潰されているかの状態
/// </summary>
enum class NODE_BLOCK_STATE
{
	FREE = 0,	// 潰されていない
	BLOCKED,	// 潰されている
};
