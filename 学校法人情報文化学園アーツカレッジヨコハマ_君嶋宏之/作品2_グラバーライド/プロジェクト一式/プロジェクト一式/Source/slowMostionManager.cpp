#include "slowMostionManager.h"
#include "Object.h"
#include "function.h"

SlowMostionManager::SlowMostionManager(SceneBase* _scene) :GameObject(_scene)
{

}

SlowMostionManager::~SlowMostionManager()
{
	for (auto& slow : slowObject) {
		slow->DestroyMe();
		slow = nullptr;
	}
	slowObject.clear();
}

void SlowMostionManager::Update()
{
	SlowModeState state = SlowMosion();
	if (state == SLOW_START) {
		for (auto i = slowObject.begin(); i != slowObject.end(); i++) {
			(*i)->SlowModeChange(true);
		}
	}
	else if(state == SLOW_STOP){
		for (auto i = slowObject.begin(); i != slowObject.end(); i++) {
			(*i)->SlowModeChange(false);
		}
	}
	for (auto i = slowObject.begin(); i != slowObject.end(); i++) {
		if (!(*i)->IsActive()) {
			(*i)->DestroyMe();
			(*i) = nullptr;
			i = slowObject.erase(i);
			if (i==slowObject.end()) {
				break;
			}
		}
	}
}

void SlowMostionManager::SlowObject(Object* object)
{
	slowObject.push_back(object);
}

void SlowMostionManager::DeleteSlowObject()
{
	for (auto& slow : slowObject) {
		slow->DestroyMe();
		slow = nullptr;
	}
	slowObject.clear();
}

void SlowMostionManager::RemoveSlowObject(Object* _object)
{
	for (auto i = slowObject.begin(); i != slowObject.end();)
	{
		if (*i == _object)
		{
			// íœ
			(*i)->DestroyMe();
			(*i) = nullptr;
			i = slowObject.erase(i);
			break;
		}
		else {
			i++;
		}
	}
}
