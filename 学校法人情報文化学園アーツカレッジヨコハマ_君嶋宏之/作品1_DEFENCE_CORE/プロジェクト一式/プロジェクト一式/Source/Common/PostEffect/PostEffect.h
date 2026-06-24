#pragma once

#define USE_POST_EFFECT (0)	// ポストエフェクトを使うかどうか

/*
 上杉　作
 PostEffect h,cpp
*/

/// <summary>
/// ゲーム画面用のポストエフェクトを管理します。
/// </summary>
namespace PostEffect
{
	/// <summary>
	/// ポストエフェクトの調整用パラメータ。
	/// </summary>
	struct Parameters
	{
		int bloomBrightThreshold;// 明るい部分の抽出閾値（0-255）
		int bloomGaussPixelWidth;// ブルームのぼかし幅（ピクセル単位）
		int bloomGaussParam;	 // ブルームのぼかし強さ（大きいほどぼかしが強くなる）
		int bloomAlpha;			 // ブルームの合成アルファ（0-255）
		int bloomPasses;		 // ブルームの重ね回数（1-8）

		int edgeThreshold;		 // エッジ抽出の閾値（0-255）

		int rimSoftGaussWidth;	 // リムライトのぼかし幅（ピクセル単位）
		int rimSoftGaussParam;	 // リムライトのぼかし強さ（大きいほどぼかしが強くなる）
		int rimAlpha;			 // リムライトの合成アルファ（0-255）
		int rimPasses;			 // リムライトの重ね回数（1-8）
		int rimColorR;			 // リムライトの色 R（0-255）
		int rimColorG;			 // リムライトの色 G（0-255）
		int rimColorB;			 // リムライトの色 B（0-255）

		int aoSoftGaussWidth;	 // AO 風処理のぼかし幅（ピクセル単位）
		int aoSoftGaussParam;	 // AO 風処理のぼかし強さ（大きいほどぼかしが強くなる）
		int aoAlpha;			 // AO 風処理の合成アルファ（0-255）
		int aoPasses;			 // AO 風処理の重ね回数（1-8）
	};
	//初期化
	void Init();
	//終了処理
	void Shutdown();

	/// <summary>
	/// 3D シーン描画の開始前に呼び出します。
	/// </summary>
	void BeginScene();

	/// <summary>
	/// 3D シーン描画の終了後に呼び出します。
	/// </summary>
	void EndScene();
	// エフェクトのオンオフ切り替え
	void ToggleBloomEnabled();
	void ToggleRimLightEnabled();
	void ToggleAmbientOcclusionEnabled();
	// エフェクトのオンオフ状態を参照
	bool IsBloomEnabled();
	bool IsRimLightEnabled();
	bool IsAmbientOcclusionEnabled();

	/// <summary>
	/// 現在の調整値を参照します。
	/// </summary>
	Parameters& GetParameters();

	/// <summary>
	/// 調整値を初期値へ戻します。
	/// </summary>
	void ResetParameters();


	/// <summary>
	/// 調整後に安全な範囲へ丸め込みます。
	/// </summary>
	void ValidateParameters();

}

