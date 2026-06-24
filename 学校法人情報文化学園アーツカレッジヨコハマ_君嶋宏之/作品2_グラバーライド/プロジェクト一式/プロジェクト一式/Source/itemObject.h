#pragma once
#include "stageObject.h"
#include <string>
#include "soundManager.h"
#include "effectManager.h"

namespace {

	//一周した後の待機時間
	const int WAITTIME = 2;
	//回る速度
	const float ROTATE_SPEED = 0.1f;
	//通常の補完を掛けるスピード
	const float RATE_ADD_SPEED = 0.01f;
	//アイテム取得時に上に上がる距離
	const float ITEM_UP_POS = 200.0f;
	//アイテム取得時の回転速度
	const float ITEM_GET_ROTATE = 1.0f;
	//アイテム取得時の補完スピード
	const float ITEM_GET_RATE = 0.03f;
}
class SoundManager;
class ItemObject :public StageObject {
public:
	ItemObject(SceneBase* _scene);
	~ItemObject();
	void Update()override;
	void Draw()override;
	
	void CollisionEvent(CollisionBase& me, CollisionBase& target)override;
	virtual void ItemGetReaction();
	virtual void ItemIdle() {};

	enum class ItemState
	{
		STAY,
		GET,
		END,
	};
protected:
	SoundManager* soundManager;

	float waitTime;
	float rate;
	float rateAddSpeed;
	ItemState itemState;
	VECTOR3 getPosition;
	std::string itemGetSound;
};