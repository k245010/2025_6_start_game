#include "potion.h"

PotionItem::PotionItem(SceneBase* _scene):ItemObject(_scene)
{
}

PotionItem::~PotionItem()
{
}

void PotionItem::ItemIdle()
{
    if (waitTime <= 0) {

        if (rate < 1.0f) {
            rate += rateAddSpeed;
        }
        else {
            waitTime = WAITTIME;
            //rotate.y = 0.0f;
            rate = 0.0f;
        }
        position.y += SmoothStep<float>(-5,5, rate);
    }
    else {
        waitTime--;
    }
}
