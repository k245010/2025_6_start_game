#pragma once
#include "../../../Library/GameObject.h"
#include "../Light/LightInfo.h"

class Color;
class Light : public GameObject
{
public:
	
	Light();
	~Light();

	/// <summary>
	/// ライトの初期化を行う
	/// </summary>
	void Init();

	void Update() override;

private:

	static const int COLOR_MAX = 3;

	/// <summary>
	///								float配列からCOLOR_Fの変数に変換
	/// </summary>
	/// <param name="colorArray">	カラーの配列							</param>
	/// <param name="_color">		COLOR_Fのカラー値						</param>
	void SetArrayToCOLOR_F(float _colorArray[COLOR_MAX], Color& _color);

	/// <summary>
	/// ライトのデータから動的なライトのデータへ値を代入する
	/// </summary>
	void SetLightDataToDynamicLightData();

	int handle			= -1;		// ライトハンドル

	LightInfo::LightData lightData;			// ライトデータ
	LightInfo::LightData dynamicLightData;	// 動的なライトデータ

	//VECTOR3 position	= VZero;	// ライト座標
	//VECTOR3 direction	= VZero;	// ライト方向

	//COLOR_F ambColor	= COLOR_F();// アンビエントカラー
	//COLOR_F spcColor	= COLOR_F();// スペキュラカラー
	//COLOR_F difColor	= COLOR_F();// ディフューズカラー
};
