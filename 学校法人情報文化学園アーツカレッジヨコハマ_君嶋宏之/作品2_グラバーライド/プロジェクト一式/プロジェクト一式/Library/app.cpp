#include "App.h"
#include "sceneManager.h"
#include "time.h"

void AppInit()
{
	Time::Init();
	SceneManager::Start();
}

void AppUpdate()
{
	Time::Refresh();
	SceneManager::Update();
}

void AppDraw()
{
	SceneManager::Draw();
}

void AppRelease()
{
	SceneManager::Release();
}