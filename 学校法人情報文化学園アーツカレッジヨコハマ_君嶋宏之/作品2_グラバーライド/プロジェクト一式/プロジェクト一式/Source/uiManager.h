#pragma once
#include "../Library/gameObject.h"
#include <string>

class PlayerUi;
class UiManager : public GameObject
{
public:

	UiManager(SceneBase* _scene);
	~UiManager();

	void Update() override;
	void Draw() override;

	

	bool SceneSame(std::string& name);

private:

	bool CreateUiObject(int _scene);
	int GetNowSceneNumber();

	PlayerUi* playerUi;
};