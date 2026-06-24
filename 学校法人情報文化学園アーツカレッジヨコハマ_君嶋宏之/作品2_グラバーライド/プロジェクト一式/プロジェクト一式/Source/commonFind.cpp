#include "commonFind.h"
#include "debugScreen.h"

#include "slowMostionManager.h"
#include "inputManager.h"

#include "loadManager.h"
#include "commonData.h"
#include "soundManager.h"
#include "stageData.h"

#include "font.h"

#include "CsvReader.h"
#include "fader.h"
#include "productionh.h"
#include "configData.h"
CommonFind::CommonFind(SceneBase* _scene):GameObject(_scene)
{
	//transit = _scene->CreateGameObject<TransitorManager>();
	commonFind.emplace_back(_scene->CreateGameObject<ConfigData>());
	
	commonFind.emplace_back(_scene->CreateGameObject<Fader>());
	
	commonFind.emplace_back(_scene->CreateGameObject<LoadManager>());
	
	commonFind.emplace_back(_scene->CreateGameObject<StageData>());
	commonFind.emplace_back(_scene->CreateGameObject<SoundManager>());
	commonFind.emplace_back(_scene->CreateGameObject<Production>());
	//commonFind.emplace_back(_scene->CreateGameObject<SlowMostionManager>());
	commonFind.emplace_back(_scene->CreateGameObject<InputManager>());
	
	commonFind.emplace_back(_scene->CreateGameObject<CommonData>());
	
	
	//commonFind.emplace_back(_scene->CreateGameObject<Font>());

	

}

CommonFind::~CommonFind()
{
	for (auto& com : commonFind) {
		if (com != nullptr) {
			com->DestroyMe();
			com = nullptr;
		}
	}
	commonFind.clear();
}


