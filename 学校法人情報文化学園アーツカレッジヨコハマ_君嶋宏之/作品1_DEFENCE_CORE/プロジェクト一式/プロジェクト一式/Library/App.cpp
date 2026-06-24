#include "App.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "Time.h"
#include "../Source/Common/Debug/Debug.h"
#include "../Source/Common/Effekseer/EffekseerObjectManager.h"
#include "../Source/Common/Sound/SoundManager.h"
#include "../Source/Common/PostEffect/PostEffect.h"
#include "../Source/Common/UI/UIDrawManager/UIDrawManager.h"
#include "../Source/Common/ShadowMap/ShadowMap.h"
#include "../Source/Collision/CollisionManager/CollisionManager.h"
#include "../Source/Gun/GunManager/GunManager.h"
#include <DxLib.h>

void AppInit()
{
	ObjectManager::Init();
	SceneManager::Init();
	Time::Init();
	EffekseerObjectManager::Init();
	SoundManager::Init();
	ShadowMap::GetShadowMapInstance()->Init();
	
#if USE_POST_EFFECT
	//PostEffect::ToggleBloomEnabled();
	//PostEffect::ToggleAmbientOcclusionEnabled();
	//PostEffect::ToggleRimLightEnabled();
	PostEffect::Init();
#endif
}

void AppUpdate()
{
	SceneManager::Update();
	EffekseerObjectManager::Update();
	SoundManager::Update();
	ObjectManager::Update();
	UIDrawManager::GetUIDrawManagerInstance()->Update();
	Debug::DebugUpdate();
}

void AppDraw()
{
#if USE_POST_EFFECT
	PostEffect::BeginScene();
#endif
	Time::Refresh();

	//ShadowMap::GetShadowMapInstance()->DrawSetup();
	ObjectManager::Draw();
	//ShadowMap::GetShadowMapInstance()->DrawEnd();
	//ShadowMap::GetShadowMapInstance()->UseShadowMap();
	//ObjectManager::DrawAndShadowMap();
	//ShadowMap::GetShadowMapInstance()->StopUseShadowMap();

	SceneManager::Draw();
	EffekseerObjectManager::Draw();
	UIDrawManager::GetUIDrawManagerInstance()->Draw();
	SoundManager::Draw();
#if USE_POST_EFFECT
	PostEffect::EndScene();
#endif
}

void AppRelease()
{
	Time::Release();
	SceneManager::Release();
	EffekseerObjectManager::Release();
	SoundManager::Release();
	ObjectManager::Release();
	ShadowMap::GetShadowMapInstance()->DeleteShadowMapInstance();
	UIDrawManager::GetUIDrawManagerInstance()->DeleteUIDrawManagerInstance();
	CollisionManager::GetCollisionManagerInstance()->DeleteCollisionManagerInstance();
	GunManager::GetGunManagerInstance()->DeleteGunManagerInstance();
#if USE_POST_EFFECT
	PostEffect::Shutdown();
#endif
}

bool AppIsExit()
{
	return SceneManager::IsExit();
}