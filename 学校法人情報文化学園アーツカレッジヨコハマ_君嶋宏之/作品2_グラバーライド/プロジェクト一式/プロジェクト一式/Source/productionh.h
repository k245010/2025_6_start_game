#pragma once
#include "../Library/gameObject.h"
class SoundManager;
class Fader;
class LoadManager;
class Production :public GameObject {
public:
	Production(SceneBase* _scene);
	~Production();
	void Draw()override;
	void Update()override;
	void SceneProductionChange(const std::string _nextScene);
private:
	enum class LoadState {
		STAY,

		FadeOut,
		WaitForLoad,
		FIN,
	};
	LoadState loadState;
	std::string nextScene;
	Fader* fade;
	LoadManager* loadManager;
	SoundManager* sound;
	
};