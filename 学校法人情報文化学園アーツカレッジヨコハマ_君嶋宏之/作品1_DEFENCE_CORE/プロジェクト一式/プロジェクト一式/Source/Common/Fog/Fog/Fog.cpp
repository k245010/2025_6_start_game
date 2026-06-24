#include "Fog.h"
#include "../FogSerializer/FogDataSerializer.h"
#include "../../Color/color.h"
#include "../../../Scene/Scenes/PlayScene.h"
#include "../../../../ImGui/imgui.h"

namespace
{
	constexpr float START_POINT = 0.0f;		// フォグの効果が始まる画面からの開始距離
	constexpr float END_POINT	= 10000.0f; // フォグの効果が始まる画面からの終了距離
}

Fog::Fog()
{
	// フォグを有効にする
	SetFogEnable(true);

	FogInfo::FogData fogData = FogDataSerializer::GetFogData(PlayScene::startStageNumber);

	drawData = DrawData(START_POINT, END_POINT,0.000005f,false);
	drawData.density = fogData.density;
	
	// フォグのfloatカラーを設定する
	ColorFunction::ByteColorToNormalizeColor(color[0], color[1], color[2], fogData.color.r, fogData.color.g, fogData.color.b);

	// フォグの色を設定
	SetNormalizeColorToByteColor();

	// 距離でフォグを描画するとき
	if (drawData.isDistanceDraw)
	{
		// フォグモードを距離に設定
		SetFogMode(DX_FOGMODE_LINEAR);

		// フォグの開始距離と終了距離を設定
		SetFogStartEnd(drawData.start, drawData.end);
	}
	else
	{
		// フォグモードを濃さに設定
		SetFogMode(DX_FOGMODE_EXP);

		// フォグの濃さを設定
		SetFogDensity(drawData.density);
	}
}

Fog::~Fog()
{
	// フォグを有効にする
	SetFogEnable(false);
}

void Fog::Update()
{
#ifdef _DEBUG

	ImGui::Begin("Fog");

	ImGui::Checkbox("FogModeDistance", &drawData.isDistanceDraw);
	 
	if(drawData.isDistanceDraw)
	{ 
		ImGui::SliderFloat("DrawStart", &drawData.start, 0.0f, 1000.0f);
		ImGui::SliderFloat("DrawEnd", &drawData.end, drawData.start, drawData.end + 10000.0f);
	}
	else
	{
		ImGui::DragFloat("DrawDensity", &drawData.density, 0.0000001f, 0.0f, 1.0f,"%.7f");
	}
	ImGui::ColorEdit3("Color", color);

	if (ImGui::Button("Save"))
	{
		FogInfo::FogData fogData = FogInfo::FogData(drawData.density, Color(color[0], color[1], color[2]));

		// フォグデータの書き出し
		FogDataSerializer::Save(PlayScene::startStageNumber, fogData);
	}

	ImGui::End();

	// フォグの色を設定
	SetNormalizeColorToByteColor();

	// 距離でフォグを描画するとき
	if (drawData.isDistanceDraw)
	{
		// フォグモードを距離に設定
		SetFogMode(DX_FOGMODE_LINEAR);

		// フォグの開始距離と終了距離を設定
		SetFogStartEnd(drawData.start, drawData.end);
	}
	else
	{
		// フォグモードを濃さに設定
		SetFogMode(DX_FOGMODE_EXP);

		// フォグの濃さを設定
		SetFogDensity(drawData.density);
	}

	// フォグの開始距離と終了距離を設定
	SetFogStartEnd(drawData.start, drawData.end);

#endif // _DEBUG
}

void Fog::SetColor(unsigned char _red, unsigned char _green, unsigned char _blue)
{
	// フォグの色を設定
	SetFogColor(_red, _green, _blue);
}

void Fog::SetDrawStartAndEnd(const float& _start, const float& _end)
{
	// フォグの開始距離と終了距離を設定
	SetFogStartEnd(drawData.start, drawData.end);
}

void Fog::SetNormalizeColorToByteColor()
{
	// byteRGBカラー //
	unsigned char red	= 0;
	unsigned char green = 0;
	unsigned char blue	= 0;

	// 正規化されたいるカラーを8bitカラーの255に変換 //
	ColorFunction::NormalizeColorToByteColor(red, green, blue, color[0], color[1], color[2]);

	// フォグの色を設定
	SetColor(red, green, blue);
}
