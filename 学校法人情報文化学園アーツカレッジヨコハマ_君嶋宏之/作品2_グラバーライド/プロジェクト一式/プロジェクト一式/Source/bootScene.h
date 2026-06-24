#pragma once
#include "../Library/sceneBase.h"

class ModelLoad;
class CommonFind;
class SlowMostionManager;
class InputManager;

class BootScene : public SceneBase {
public:
	BootScene();
	~BootScene();
	void Update() override;
	void Draw() override;
private:

	ModelLoad* modelLoad;
	CommonFind* commonFind;
	SlowMostionManager* slowMosion;
	InputManager* inputManager;
};