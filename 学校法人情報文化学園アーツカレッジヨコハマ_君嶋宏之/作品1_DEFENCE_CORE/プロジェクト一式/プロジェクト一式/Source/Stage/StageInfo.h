#pragma once

namespace StageInfo
{
	static constexpr int MAX_STAGE_NUM	= 5;						// ステージの最大数
	constexpr const float BLOCK_SIZE	= 100.0f * 2.2f;			// ブロックの直径
	constexpr float PUT_GRID_SIZE		= StageInfo::BLOCK_SIZE;	// オブジェクトを置く感覚グリッド
	constexpr float BLOCK_PUT_POS_Y		= 0.0f;						// ブロック配置でのデフォルトY座標
}
