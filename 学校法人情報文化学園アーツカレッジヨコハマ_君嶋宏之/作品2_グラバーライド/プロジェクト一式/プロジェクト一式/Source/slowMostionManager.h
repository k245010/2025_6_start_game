#pragma once
#include "../Library/gameObject.h"

class Object;

class SlowMostionManager: public GameObject
{
public:

	enum SlowModeState
	{
		SLOW_START,
		SLOW_STOP,
		NO_SLOW,
	};

	SlowMostionManager(SceneBase* _scene);
	~SlowMostionManager();
	void Update()override;

	void SlowObject(Object* object);

	void DeleteSlowObject();
	void RemoveSlowObject(Object* _object);

private:
	std::list<Object*> slowObject;
};
