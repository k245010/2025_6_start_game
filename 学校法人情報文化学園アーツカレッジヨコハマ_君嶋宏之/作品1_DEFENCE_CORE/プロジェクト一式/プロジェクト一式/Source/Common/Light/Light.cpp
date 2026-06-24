#include "Light.h"
#include "LightDataSerializer/LightDataSerializer.h"
#include "../../Common/ShadowMap/ShadowMap.h"
#include "../../Scene/Scenes/PlayScene.h"
#include "../../../ImGui/imgui.h"

Light::Light()
{
}

void Light::Init()
{
	// デフォルトのライトをオフ
	SetLightEnable(FALSE);

	// ライトハンドルを生成
	handle		= CreateDirLightHandle(LightInfo::DEFAULT_LIGHT_DIR);
	
	lightData	= LightDataSerializer::GetLightData(PlayScene::startStageNumber);
	
	// 動的なライトデータを初期化
	SetLightDataToDynamicLightData();

	// 座標を設定
	SetLightPositionHandle(handle, dynamicLightData.position);
	// 方向を設定
	SetLightDirectionHandle(handle, dynamicLightData.direction);
}

Light::~Light()
{
	// ライトハンドルの削除
	DeleteLightHandle(handle);
}

void Light::Update()
{
#ifdef _DEBUG

	float ambColorArray[COLOR_MAX] =	// アンビエントカラーの配列
	{
		dynamicLightData.ambColor.floatColor4.r,
		dynamicLightData.ambColor.floatColor4.g,
		dynamicLightData.ambColor.floatColor4.b,
	};
	float spcColorArray[COLOR_MAX] =	// スペキュラカラーの配列
	{
		dynamicLightData.spcColor.floatColor4.r,
		dynamicLightData.spcColor.floatColor4.g,
		dynamicLightData.spcColor.floatColor4.b,
	};
	float difColorArray[COLOR_MAX] =	// ディフューズカラーの配列
	{
		dynamicLightData.difColor.floatColor4.r,
		dynamicLightData.difColor.floatColor4.g,
		dynamicLightData.difColor.floatColor4.b,
	};

	ImGui::Begin("Light");

	ImGui::ColorEdit3("AmbColor", ambColorArray);
	ImGui::ColorEdit3("SpcColor", spcColorArray);
	ImGui::ColorEdit3("DifColor", difColorArray);

	if (ImGui::Button("Save"))
	{
		// ライトデータを書き出し
		LightDataSerializer::Save(PlayScene::startStageNumber, dynamicLightData);
	}

	ImGui::End();

	// float配列からCOLOR_Fの変数に変換 //
	SetArrayToCOLOR_F(ambColorArray, dynamicLightData.ambColor);
	SetArrayToCOLOR_F(spcColorArray, dynamicLightData.spcColor);
	SetArrayToCOLOR_F(difColorArray, dynamicLightData.difColor);

	// ライトのアンビエントカラーを設定
	SetLightAmbColorHandle(handle, dynamicLightData.ambColor.floatColor4);

	// ライトのスペキュラカラーを設定
	SetLightSpcColorHandle(handle, dynamicLightData.spcColor.floatColor4);

	// ライトのディフューズカラーを設定
	SetLightDifColorHandle(handle, dynamicLightData.difColor.floatColor4);

#endif // _DEBUG
}

void Light::SetArrayToCOLOR_F(float _colorArray[COLOR_MAX], Color& _color)
{
	_color.floatColor4.r = _colorArray[0];
	_color.floatColor4.g = _colorArray[1];
	_color.floatColor4.b = _colorArray[2];
}

void Light::SetLightDataToDynamicLightData()
{
	dynamicLightData = lightData;
}

