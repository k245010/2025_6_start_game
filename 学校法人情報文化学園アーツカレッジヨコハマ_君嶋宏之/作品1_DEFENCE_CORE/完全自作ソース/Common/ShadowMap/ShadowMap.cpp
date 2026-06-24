#include "ShadowMap.h"
#include "../Light/Light.h"
#include "../../../Library/myDxLib.h"

namespace
{
	const VECTOR3 START_SHADOW_MAP_DRAW_AREA	= VECTOR3(-9000.0f, -9000.0f, -9000.0f);// シャドウマップエリアの開始地点
	const VECTOR3 END_SHADOW_MAP_DRAW_AREA		= VECTOR3(9000.0f, 9000.0f, 9000.0f);	// シャドウマップエリアの終了地点

	constexpr int SHADOW_MAP_SIZE = 8192;	// シャドウマップのサイズ　数値は大きいほど影は鮮明になる　値は2の乗数にする!
}

ShadowMap* ShadowMap::instance = nullptr;

ShadowMap::ShadowMap()
{
}

ShadowMap::~ShadowMap()
{
	// シャドウマップの削除
	DeleteShadowMap(shadowMapHandle);
}

ShadowMap* ShadowMap::GetShadowMapInstance()
{
	if (instance == nullptr)
	{
		instance = new ShadowMap();
	}
	return instance;
}

void ShadowMap::DeleteShadowMapInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

void ShadowMap::Init()
{
	// シャドウマップハンドルの作成
	shadowMapHandle = MakeShadowMap(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

	// ライトの方向からシャドウマップの方向を設定
	SetShadowCasterDirection(LightInfo::DEFAULT_LIGHT_DIR);
	
	// シャドウマップの描画範囲を設定
	SetShadowMapDrawArea(shadowMapHandle, START_SHADOW_MAP_DRAW_AREA, END_SHADOW_MAP_DRAW_AREA);
}

void ShadowMap::SetShadowCasterDirection(const VECTOR3& _direction)
{
	// ライトの方向からシャドウマップの方向を設定
	SetShadowMapLightDirection(shadowMapHandle, _direction);
}

void ShadowMap::DrawSetup()
{
	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(shadowMapHandle);
}

void ShadowMap::DrawEnd()
{
	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();
}

void ShadowMap::UseShadowMapSetting()
{
	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, shadowMapHandle);
}

void ShadowMap::StopUseShadowMapSetting()
{
	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);
}
