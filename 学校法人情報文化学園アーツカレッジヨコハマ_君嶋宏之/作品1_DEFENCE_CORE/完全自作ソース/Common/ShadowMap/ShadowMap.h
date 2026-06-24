#pragma once
#include "../../../Library/myDxLib.h"

class ShadowMap
{
public:

	/// <summary>
	///				静的メソッドでShadowMapインスタンスを返す
	/// </summary>
	/// <returns>	ShadowMapインスタンス					</returns>
	static ShadowMap* GetShadowMapInstance();

	/// <summary>
	///	静的メソッドでShadowMapのインスタンスを削除する
	/// </summary>
	static void DeleteShadowMapInstance();

	/// <summary>
	/// シャドウマップの初期化
	/// </summary>
	void Init();

	/// <summary>
	/// シャドウマップへの描画の準備
	/// </summary>
	void DrawSetup();

	/// <summary>
	/// シャドウマップへの描画の終了
	/// </summary>
	void DrawEnd();

	/// <summary>
	///								影を落とす方向を設定する
	/// </summary>
	/// <param name="_direction">	影を落とす方向			</param>
	void SetShadowCasterDirection(const VECTOR3& _direction);

	/// <summary>
	/// 描画に使用するシャドウマップを設定
	/// </summary>
	void UseShadowMapSetting();

	/// <summary>
	/// 描画に使用するシャドウマップの設定の解除
	/// </summary>
	void StopUseShadowMapSetting();

private:

	ShadowMap();
	~ShadowMap();

	static ShadowMap* instance;	// シャドウマップクラスのインスタンス

	int shadowMapHandle = -1;	// シャドウマップハンドル
};
