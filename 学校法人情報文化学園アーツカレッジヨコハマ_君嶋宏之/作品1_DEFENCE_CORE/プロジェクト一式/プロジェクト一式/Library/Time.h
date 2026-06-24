#pragma once

/// <summary>
/// １フレームの時間を計測する
/// 最近の20フレームの平均時間を測定し、
/// DeltaTime()で、その時間を秒で返す
/// 60フレームの場合は、0.016が返る
/// </summary>
namespace Time {
	/// <summary>
	/// 最初に１回だけ呼んでもらう
	/// 計測のための初期化をする
	/// </summary>
	void Init();

	/// <summary>
	/// 毎フレーム１回呼んでもらう
	/// WindowMessageが入った直後がベスト
	/// </summary>
	void Refresh();

	/// <summary>
	/// １フレームの経過時間を返す
	/// ただし、最大で２フレーム分の時間
	/// </summary>
	/// <returns>経過時間（秒）</returns>
	float DeltaTime();

	/// <summary>
	/// ゲーム用の１フレームの経過時間を返す
	/// ただし、最大で２フレーム分の時間
	/// </summary>
	/// <returns>経過時間（秒）</returns>
	float GameDeltaTime();

	/// <summary>
	/// ゲーム用の１フレームの経過時間の倍率を設定する　1.0f デフォルト
	/// </summary>
	void SetGameDeltaTimeRate(float _rate);

	/// <summary>
	/// ゲーム用の１フレームの経過時間の倍率を返す
	/// </summary>
	/// <returns> ゲーム用の１フレームの経過時間の倍率 </returns>
	float GetGameDeltaTimeRate();

	/// <summary>
	/// ゲーム用の１フレームの経過時間の倍率をリセットする
	/// </summary>
	void ResetGameDeltaTime();

	/// <summary>
	/// 最後に１回だけ呼んでもらう
	/// </summary>
	void Release();

	/// <summary>
	/// １秒間のFPS値を返す
	/// </summary>
	/// <returns> １秒間のFPS </returns>
	float GetAverageFPS();

	/// <summary>
	/// の最低FPSを返す
	/// </summary>
	/// <returns> の最低FPS </returns>
	float GetWorstFPS();

	/// <summary>
	/// 記録をしているFPS値をすべてリセットする
	/// </summary>
	void ResetSaveFPS();
};
