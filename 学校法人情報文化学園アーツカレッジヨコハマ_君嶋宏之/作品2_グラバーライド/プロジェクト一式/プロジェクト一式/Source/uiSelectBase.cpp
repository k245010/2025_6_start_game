#include "uiSelectBase.h"
#include "../Library/sceneManager.h"
#include "commonFind.h"


UiSelectBase::UiSelectBase(SceneBase* _scene) : UiBase(_scene)
{
	gameManager = GetScene()->FindGameObject<GameManager>();
	font = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<Font>();

	distance = 0.0f;
	titleFontSize = V2Get(3.0f, 3.0f);
}

UiSelectBase::~UiSelectBase()
{
	select.clear();
	button.clear();
}
