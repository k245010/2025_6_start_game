#include "coin.h"
#include "stageScore.h"

Coin::Coin(SceneBase* _scene) :ItemObject(_scene)
{
	itemGetSound = "ƒRƒCƒ“Žæ“¾";
	stageScore = _scene->FindGameObject<StageScore>();
}

Coin::~Coin()
{
}

void Coin::ItemGetReaction()
{
	soundManager->PlaySe(itemGetSound);
	effectManager->CreateEffekseer(position, nullptr, "FLASH", 1.0f, 60, false,VGet(0.7f,0.7f,0.7f),VZero);
	stageScore->AddGetCoin();
}

void Coin::ItemIdle()
{
    if (waitTime <= 0) {

        if (rate < 1.0f) {
            rate += rateAddSpeed;
        }
        else {
            waitTime = WAITTIME;
            rotate.y = 0.0f;
            rate = 0.0f;
        }
        rotate.y = SmoothStep<float>(rotate.y, DX_PI_F * 2, rate);
    }
    else {
        waitTime--;
    }
}

