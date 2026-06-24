#include "BootScene.h"
#include "../Common/Fade/Fade.h"
#include "../Common/Input/Input.h"
#include "../Common/Debug/Debug.h"
#include "../../Library/SceneManager.h"
#include "../../Library/DebugNew.h"

BootScene::BootScene()
{
	new Fade();
	Input* inputManager = new Input(true,true,true);
	Debug::InitDebug(inputManager);
}

BootScene::~BootScene()
{
}

void BootScene::Update()
{
	SceneManager::ChangeScene("TITLE"); // ‹N“®‚ªI‚í‚Á‚½‚çTitle‚ğ•\¦
}

void BootScene::Draw()
{
}
