#include "PostEffect.h"

#include "../../../Library/SceneManager.h"
#include "../../../Library/myDxLib.h"
//#include <DxLib.h>
#include "../../Screen.h"
#include <algorithm>

namespace
{
	// 縮小率そのものはスクリーン再生成が必要なので固定値のままにします。
	constexpr int kBloomDownScale = 8;
	constexpr int kEdgeDownScale = 4;
	constexpr int kEdgeGaussPixelWidth = 18;
	constexpr int kEdgeGaussParam = 900;

	constexpr PostEffect::Parameters kDefaultParameters =
	{
		205,	// 明るい部分の抽出閾値（0-255）
		14,		// ブルームのぼかし幅（ピクセル単位）
		620,	// ブルームのぼかし強さ（大きいほどぼかしが強くなる）
		56,		// ブルームの合成アルファ（0-255）
		1,		// ブルームの重ね回数（1-8）

		12,		// エッジ抽出の閾値（0-255）輝き

		8,		// リムライトのぼかし幅（ピクセル単位）
		260,	// リムライトのぼかし強さ（大きいほどぼかしが強くなる）
		48,		// リムライトの合成アルファ（0-255）
		1,		// リムライトの重ね回数（1-8）
		185,	// リムライトの色 R（0-255）
		205,	// リムライトの色 G（0-255）
		230,	// リムライトの色 B（0-255）

		10,		// AO 風処理のぼかし幅（ピクセル単位）
		360,	// AO 風処理のぼかし強さ（大きいほどぼかしが強くなる）
		92,		// AO 風処理の合成アルファ（0-255）
		2		// AO 風処理の重ね回数（1-8）
	};



	PostEffect::Parameters params = kDefaultParameters;

	int sceneScreen = -1;
	int compositeScreen = -1;

	int bloomHighBrightScreen = -1;
	int bloomDownScaleScreen = -1;
	int bloomGaussScreen = -1;

	int edgeDownScaleScreen = -1;
	int edgeBlurScreen = -1;
	int edgeExpandedScreen = -1;
	int edgeDiffScreen = -1;
	int edgeMaskScreen = -1;

	int sceneWidth = 0;
	int sceneHeight = 0;
	int bloomDownScaleWidth = 0;
	int bloomDownScaleHeight = 0;
	int edgeDownScaleWidth = 0;
	int edgeDownScaleHeight = 0;

	bool bloomEnabled = true;
	bool rimLightEnabled = true;
	bool aoEnabled = true;
	bool useThisFrame = false;
	int statusViewFrames = 0;

	void DeleteScreenHandle(int& handle)
	{
		if (handle >= 0)
		{
			DeleteGraph(handle);
			handle = -1;
		}
	}

	bool IsTargetScene()
	{
		return true; //SceneManager::GetCurrentSceneName() == "PLAY";
	}

	bool CreateScreens()
	{
		sceneWidth = Screen::WIDTH;
		sceneHeight = Screen::HEIGHT;
		bloomDownScaleWidth = (sceneWidth + kBloomDownScale - 1) / kBloomDownScale;
		bloomDownScaleHeight = (sceneHeight + kBloomDownScale - 1) / kBloomDownScale;
		edgeDownScaleWidth = (sceneWidth + kEdgeDownScale - 1) / kEdgeDownScale;
		edgeDownScaleHeight = (sceneHeight + kEdgeDownScale - 1) / kEdgeDownScale;

		DeleteScreenHandle(sceneScreen);
		DeleteScreenHandle(compositeScreen);
		DeleteScreenHandle(bloomHighBrightScreen);
		DeleteScreenHandle(bloomDownScaleScreen);
		DeleteScreenHandle(bloomGaussScreen);
		DeleteScreenHandle(edgeDownScaleScreen);
		DeleteScreenHandle(edgeBlurScreen);
		DeleteScreenHandle(edgeExpandedScreen);
		DeleteScreenHandle(edgeDiffScreen);
		DeleteScreenHandle(edgeMaskScreen);

		/*sceneScreen = MakeScreen(sceneWidth, sceneHeight, FALSE);
		compositeScreen = MakeScreen(sceneWidth, sceneHeight, FALSE);
		bloomHighBrightScreen = MakeScreen(sceneWidth, sceneHeight, FALSE);
		bloomDownScaleScreen = MakeScreen(bloomDownScaleWidth, bloomDownScaleHeight, FALSE);
		bloomGaussScreen = MakeScreen(bloomDownScaleWidth, bloomDownScaleHeight, FALSE);
		edgeDownScaleScreen = MakeScreen(edgeDownScaleWidth, edgeDownScaleHeight, FALSE);
		edgeBlurScreen = MakeScreen(edgeDownScaleWidth, edgeDownScaleHeight, FALSE);
		edgeExpandedScreen = MakeScreen(sceneWidth, sceneHeight, FALSE);
		edgeDiffScreen = MakeScreen(sceneWidth, sceneHeight, FALSE);
		edgeMaskScreen = MakeScreen(sceneWidth, sceneHeight, FALSE);*/

		sceneScreen = MakeScreen(sceneWidth, sceneHeight, TRUE);
		compositeScreen = MakeScreen(sceneWidth, sceneHeight, TRUE);
		bloomHighBrightScreen = MakeScreen(sceneWidth, sceneHeight, TRUE);
		bloomDownScaleScreen = MakeScreen(bloomDownScaleWidth, bloomDownScaleHeight, TRUE);
		bloomGaussScreen = MakeScreen(bloomDownScaleWidth, bloomDownScaleHeight, TRUE);
		edgeDownScaleScreen = MakeScreen(edgeDownScaleWidth, edgeDownScaleHeight, TRUE);
		edgeBlurScreen = MakeScreen(edgeDownScaleWidth, edgeDownScaleHeight, TRUE);
		edgeExpandedScreen = MakeScreen(sceneWidth, sceneHeight, TRUE);
		edgeDiffScreen = MakeScreen(sceneWidth, sceneHeight, TRUE);
		edgeMaskScreen = MakeScreen(sceneWidth, sceneHeight, TRUE);

		const bool ok =
			sceneScreen >= 0 &&
			compositeScreen >= 0 &&
			bloomHighBrightScreen >= 0 &&
			bloomDownScaleScreen >= 0 &&
			bloomGaussScreen >= 0 &&
			edgeDownScaleScreen >= 0 &&
			edgeBlurScreen >= 0 &&
			edgeExpandedScreen >= 0 &&
			edgeDiffScreen >= 0 &&
			edgeMaskScreen >= 0;

		if (!ok)
		{
			//LOG_W("[PostEffect] MakeScreen failed. Post effects disabled.");
			DeleteScreenHandle(sceneScreen);
			DeleteScreenHandle(compositeScreen);
			DeleteScreenHandle(bloomHighBrightScreen);
			DeleteScreenHandle(bloomDownScaleScreen);
			DeleteScreenHandle(bloomGaussScreen);
			DeleteScreenHandle(edgeDownScaleScreen);
			DeleteScreenHandle(edgeBlurScreen);
			DeleteScreenHandle(edgeExpandedScreen);
			DeleteScreenHandle(edgeDiffScreen);
			DeleteScreenHandle(edgeMaskScreen);
			return false;
		}

		return true;
	}

	bool CanUseAnyEffect()
	{
		return sceneScreen >= 0 && compositeScreen >= 0 && (bloomEnabled || rimLightEnabled || aoEnabled);
	}

	void ExpandSmallScreenToFull(int srcHandle, int destHandle)
	{
		SetDrawScreen(destHandle);
		ClearDrawScreen();
		//SetDrawMode(DX_DRAWMODE_BILINEAR);
		SetDrawMode(DX_DRAWMODE_NEAREST);
		DrawExtendGraph(0, 0, sceneWidth, sceneHeight, srcHandle, FALSE);
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}

	void BuildEdgeMask()
	{
		// 画面全体を小さくしてからぼかし、元画像との差分でエッジを抽出します。
		GraphFilterBlt(sceneScreen, edgeDownScaleScreen, DX_GRAPH_FILTER_DOWN_SCALE, kEdgeDownScale);
		GraphFilterBlt(edgeDownScaleScreen, edgeBlurScreen, DX_GRAPH_FILTER_GAUSS, kEdgeGaussPixelWidth, kEdgeGaussParam);

		ExpandSmallScreenToFull(edgeBlurScreen, edgeExpandedScreen);

		GraphBlendBlt(edgeExpandedScreen, sceneScreen, edgeDiffScreen, 255, DX_GRAPH_BLEND_DIFFERENCE);

		//GraphFilterBlt(edgeDiffScreen,edgeMaskScreen,DX_GRAPH_FILTER_BRIGHT_CLIP,DX_CMP_LESS,params.edgeThreshold,TRUE,GetColor(0, 0, 0),255);
		
		//GraphFilterBlt(edgeDiffScreen,edgeMaskScreen,DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_GREATER,params.edgeThreshold,TRUE,GetColor(0, 0, 0),255);
		GraphFilterBlt(
			edgeDiffScreen,
			edgeExpandedScreen,
			DX_GRAPH_FILTER_GAUSS,
			8,
			200);

		GraphFilterBlt(
			edgeExpandedScreen,
			edgeMaskScreen,
			DX_GRAPH_FILTER_BRIGHT_CLIP,
			DX_CMP_GREATER,
			params.edgeThreshold,
			TRUE,
			GetColor(0, 0, 0),
			255);
	}

	void ApplyBloom()
	{
#if 0
		//GraphFilterBlt(sceneScreen, bloomHighBrightScreen, DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_LESS, params.bloomBrightThreshold, TRUE, GetColor(0, 0, 0), 255);
		GraphFilterBlt(sceneScreen, bloomHighBrightScreen, DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_GREATER, params.bloomBrightThreshold, TRUE, GetColor(0, 0, 0), 255);

		GraphFilterBlt(bloomHighBrightScreen, bloomDownScaleScreen, DX_GRAPH_FILTER_DOWN_SCALE, kBloomDownScale);
		GraphFilterBlt(bloomDownScaleScreen, bloomGaussScreen, DX_GRAPH_FILTER_GAUSS, params.bloomGaussPixelWidth, params.bloomGaussParam);

		SetDrawMode(DX_DRAWMODE_BILINEAR);
		SetDrawBlendMode(DX_BLENDMODE_ADD, params.bloomAlpha);
		for (int i = 0; i < params.bloomPasses; ++i)
		{
			DrawExtendGraph(0, 0, sceneWidth, sceneHeight, bloomGaussScreen, FALSE);
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		SetDrawMode(DX_DRAWMODE_NEAREST);
#else
		// 先に縮小してノイズを平均化
		GraphFilterBlt(
			sceneScreen,
			bloomDownScaleScreen,
			DX_GRAPH_FILTER_DOWN_SCALE,
			kBloomDownScale);

		// 高輝度部分だけ抽出
		GraphFilterBlt(
			bloomDownScaleScreen,
			bloomGaussScreen,
			DX_GRAPH_FILTER_BRIGHT_CLIP,
			DX_CMP_GREATER,
			params.bloomBrightThreshold,
			TRUE,
			GetColor(0, 0, 0),
			255);

		// ガウスぼかし
		GraphFilterBlt(
			bloomGaussScreen,
			bloomDownScaleScreen,
			DX_GRAPH_FILTER_GAUSS,
			params.bloomGaussPixelWidth,
			params.bloomGaussParam);

		// 合成
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		SetDrawBlendMode(DX_BLENDMODE_ADD, params.bloomAlpha);

		for (int i = 0; i < params.bloomPasses; ++i)
		{
			DrawExtendGraph(
				0,
				0,
				sceneWidth,
				sceneHeight,
				bloomDownScaleScreen,
				FALSE);
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		SetDrawMode(DX_DRAWMODE_NEAREST);
#endif
	}

	void ApplyAmbientOcclusionLike()
	{
		// 抽出済みエッジを少し広げ、黒で重ねるだけの軽量 AO 風処理です。
		GraphFilterBlt(edgeMaskScreen, edgeExpandedScreen, DX_GRAPH_FILTER_GAUSS, params.aoSoftGaussWidth, params.aoSoftGaussParam);

		SetDrawMode(DX_DRAWMODE_BILINEAR);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, params.aoAlpha);
		SetDrawBright(0, 0, 0);
		for (int i = 0; i < params.aoPasses; ++i)
		{
			DrawGraph(0, 0, edgeExpandedScreen, FALSE);
		}
		SetDrawBright(255, 255, 255);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}

	void ApplyRimLightLike()
	{
		// エッジを少しだけ太らせて青白く加算し、見た目で差が分かるよう強めにします。
		GraphFilterBlt(edgeMaskScreen, edgeExpandedScreen, DX_GRAPH_FILTER_GAUSS, params.rimSoftGaussWidth, params.rimSoftGaussParam);

		SetDrawMode(DX_DRAWMODE_BILINEAR);
		SetDrawBlendMode(DX_BLENDMODE_ADD, params.rimAlpha);
		SetDrawBright(params.rimColorR, params.rimColorG, params.rimColorB);
		for (int i = 0; i < params.rimPasses; ++i)
		{
			DrawGraph(0, 0, edgeExpandedScreen, FALSE);
		}

		SetDrawBright(255, 255, 255);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}

	void ResetDrawStateForUi()
	{
		// ImGui や 2D UI 側へ影響が残らないよう、描画状態を戻します。
		SetDrawScreen(DX_SCREEN_BACK);
		SetDrawArea(0, 0, sceneWidth - 1, sceneHeight - 1);
		SetDrawMode(DX_DRAWMODE_NEAREST);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		SetDrawBright(255, 255, 255);
		SetUseZBufferFlag(TRUE);
		SetWriteZBufferFlag(TRUE);
		SetUseZBuffer3D(TRUE);
		SetWriteZBuffer3D(TRUE);
	}
}

namespace PostEffect
{
	void Init()
	{
		statusViewFrames = 240;
		ResetParameters();

		if (!CreateScreens())
		{
			bloomEnabled = false;
			rimLightEnabled = false;
			aoEnabled = false;
		}
	}

	void Shutdown()
	{
		DeleteScreenHandle(sceneScreen);
		DeleteScreenHandle(compositeScreen);
		DeleteScreenHandle(bloomHighBrightScreen);
		DeleteScreenHandle(bloomDownScaleScreen);
		DeleteScreenHandle(bloomGaussScreen);
		DeleteScreenHandle(edgeDownScaleScreen);
		DeleteScreenHandle(edgeBlurScreen);
		DeleteScreenHandle(edgeExpandedScreen);
		DeleteScreenHandle(edgeDiffScreen);
		DeleteScreenHandle(edgeMaskScreen);
		useThisFrame = false;
		statusViewFrames = 0;
	}

	void BeginScene()
	{
		useThisFrame = IsTargetScene() && CanUseAnyEffect();

		if (useThisFrame)
		{
			SetDrawScreen(sceneScreen);
			ClearDrawScreen();
		}
		else
		{
			SetDrawScreen(DX_SCREEN_BACK);
		}
	}

	void EndScene()
	{
		if (!useThisFrame)
		{
			ResetDrawStateForUi();
			return;
		}

		ValidateParameters();

		// まず sceneScreen を合成用スクリーンへコピーし、
		// 以降のエフェクトは compositeScreen に順番に積みます。
		// これで Bloom / Rim / AO が互いを消し合いにくくなります。
		SetDrawScreen(compositeScreen);
		ClearDrawScreen();
		DrawGraph(0, 0, sceneScreen, FALSE);

		// Rim / AO は同じエッジマスクを共有してコストを抑えます。
		if (rimLightEnabled || aoEnabled)
		{
			BuildEdgeMask();
		}

		SetDrawScreen(compositeScreen);

		if (aoEnabled)
		{
			ApplyAmbientOcclusionLike();
		}

		if (rimLightEnabled)
		{
			ApplyRimLightLike();
		}

		if (bloomEnabled)
		{
			ApplyBloom();
		}

		// 全エフェクト合成後の結果だけを裏画面へ戻します。
		SetDrawScreen(DX_SCREEN_BACK);
		DrawGraph(0, 0, compositeScreen, FALSE);
		ResetDrawStateForUi();
	}

	void ToggleBloomEnabled()
	{
		bloomEnabled = !bloomEnabled;
		statusViewFrames = 240;
	}

	void ToggleRimLightEnabled()
	{
		rimLightEnabled = !rimLightEnabled;
		statusViewFrames = 240;
	}

	void ToggleAmbientOcclusionEnabled()
	{
		aoEnabled = !aoEnabled;
		statusViewFrames = 240;
	}

	bool IsBloomEnabled()
	{
		return bloomEnabled;
	}

	bool IsRimLightEnabled()
	{
		return rimLightEnabled;
	}

	bool IsAmbientOcclusionEnabled()
	{
		return aoEnabled;
	}

	Parameters& GetParameters()
	{
		return params;
	}

	void ResetParameters()
	{
		params = kDefaultParameters;
		ValidateParameters();
		statusViewFrames = 240;
	}



	void ValidateParameters()
	{
		params.bloomBrightThreshold = std::clamp(params.bloomBrightThreshold, 0, 255);
		params.bloomGaussPixelWidth = std::clamp(params.bloomGaussPixelWidth, 1, 64);
		params.bloomGaussParam = std::clamp(params.bloomGaussParam, 1, 2000);
		params.bloomAlpha = std::clamp(params.bloomAlpha, 0, 255);
		params.bloomPasses = std::clamp(params.bloomPasses, 1, 8);

		params.edgeThreshold = std::clamp(params.edgeThreshold, 0, 255);

		params.rimSoftGaussWidth = std::clamp(params.rimSoftGaussWidth, 1, 64);
		params.rimSoftGaussParam = std::clamp(params.rimSoftGaussParam, 1, 2000);
		params.rimAlpha = std::clamp(params.rimAlpha, 0, 255);
		params.rimPasses = std::clamp(params.rimPasses, 1, 8);
		params.rimColorR = std::clamp(params.rimColorR, 0, 255);
		params.rimColorG = std::clamp(params.rimColorG, 0, 255);
		params.rimColorB = std::clamp(params.rimColorB, 0, 255);

		params.aoSoftGaussWidth = std::clamp(params.aoSoftGaussWidth, 1, 64);
		params.aoSoftGaussParam = std::clamp(params.aoSoftGaussParam, 1, 2000);
		params.aoAlpha = std::clamp(params.aoAlpha, 0, 255);
		params.aoPasses = std::clamp(params.aoPasses, 1, 8);
	}


}
