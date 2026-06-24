#include "itemObject.h"
#include "rgb.h"
#include "collisionBase.h"


ItemObject::ItemObject(SceneBase* _scene):StageObject(_scene)
{
	
	waitTime = 0;
	//scale = VGet(0.4f, 0.4f, 0.4f);
	rate = 0.0f;
	rateAddSpeed = RATE_ADD_SPEED;
	itemState = ItemState::STAY;
    getPosition = VZero;
    itemGetSound = "ƒRƒCƒ“Žæ“¾";
    soundManager = commonFind->CommonFindObject<SoundManager>();
}

ItemObject::~ItemObject()
{
}

void ItemObject::Update()  
{  
   switch (itemState)  
   {  
   case ItemObject::ItemState::STAY:  

       ItemIdle();
       break;  
   case ItemObject::ItemState::GET:  

       if (rate < 1.0f) {  
           rate += rateAddSpeed; 
           rotate.y += ITEM_GET_ROTATE;
           position = VTransform(VGet(getPosition.x,getPosition.y+ EaseInOut<float>(0.0f,ITEM_UP_POS, rate),getPosition.z), MGetRotZ(rotate.z));
           
       }
       else {
           itemState= ItemState::END;
       }
     
       
       break;  
   case ItemObject::ItemState::END:  
       active = false;  
       break;  
   }  
  
}

void ItemObject::Draw()
{
	StageObject::Draw();
}

void ItemObject::CollisionEvent(CollisionBase& me, CollisionBase& target)
{
    
	if (target.GetTagMe() == COLLISION_OBJECT_KIND::PLAYER)
	{   
        me.DeleteTagMe();
        rate = 0.0f;
        getPosition = position;
        rateAddSpeed =ITEM_GET_RATE;
        ItemGetReaction();
	    itemState = ItemState::GET;
	}
}

void ItemObject::ItemGetReaction()
{
    soundManager->PlaySe(itemGetSound);
    effectManager->CreateEffekseer(position, nullptr, "FLASH", 1.0f, 60, false, VECTOR3(0.7f), VZero);
}


