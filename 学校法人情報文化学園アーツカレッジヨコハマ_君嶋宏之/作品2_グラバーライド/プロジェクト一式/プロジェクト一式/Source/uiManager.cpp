#include "../Library/sceneManager.h"
#include "uiManager.h"
#include "playerUi.h"
#include "function.h"

namespace
{
	enum class SCENE_KIND
	{
		TITLE = 0,
		PLAY,
		SELECT,

		MAX,
	};
	SCENE_KIND sceneKind;

	std::string copyScene;
	std::string scene[] =
	{
		"TitleScene",
		"PlayScene",
		"SelectScene",
	};
	constexpr int SCENE_MAX = (int)SCENE_KIND::MAX;
}

UiManager::UiManager(SceneBase* _scene) : GameObject(_scene)
{
	playerUi = nullptr;
}

UiManager::~UiManager()
{
	if (playerUi != nullptr)
	{
		playerUi->DestroyMe();
		playerUi = nullptr;
	}
}

void UiManager::Update()
{
	int nowScene = GetNowSceneNumber();
	CreateUiObject(nowScene);

	switch (nowScene)
	{
	case (int)SCENE_KIND::TITLE:


		break;
	case (int)SCENE_KIND::PLAY:

		if (playerUi != nullptr)
		{
			playerUi->Update();
		}
		break;
	case (int)SCENE_KIND::SELECT:


		break;
	default:
		break;
	}
}

void UiManager::Draw()
{
	switch (GetNowSceneNumber())
	{
	case (int)SCENE_KIND::TITLE:


		break;
	case (int)SCENE_KIND::PLAY:

		if (playerUi != nullptr)
		{
			playerUi->Draw();
		}
		break;
	case (int)SCENE_KIND::SELECT:


		break;
	default:
		break;
	}
}

bool UiManager::SceneSame(std::string& name)
{
	if (SceneManager::GetSceneName() == name)
	{
		return true;
	}
	return false;
}

bool UiManager::CreateUiObject(int _scene)
{
	bool create = true;
	switch (_scene)
	{
	case (int)SCENE_KIND::TITLE:

		break;
	case (int)SCENE_KIND::PLAY:

		if (playerUi == nullptr)
			playerUi = GetScene()->CreateGameObject<PlayerUi>();
		break;
	case (int)SCENE_KIND::SELECT:
		
		break;
	default:

		create = false;
		break;
	}
	return create;
}

int UiManager::GetNowSceneNumber()
{
	for (int i = 0;i < SCENE_MAX;i++)
	{
		if (SceneSame(scene[i]))
		{
			return i;
		}
	}
	CreateMessageBox("シーンの取得に失敗したのかな？", "UiManagerのGetNowSceneNumberでエラー");
	return -1;
}
