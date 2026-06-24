#pragma once
#include "itemObject.h"

class PotionItem:public ItemObject {
public:
	PotionItem(SceneBase* _scene);
	~PotionItem();
	void ItemIdle()override;
private:
};