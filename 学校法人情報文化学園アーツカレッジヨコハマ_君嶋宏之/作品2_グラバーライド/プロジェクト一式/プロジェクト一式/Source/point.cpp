#include "point.h"
#include "collisionBase.h"
#include "effectManager.h"
namespace {
	const float MAX_RATE = 1.0f;
}
Point::Point(SceneBase* _scene):StageObject(_scene)
{
	checkPointId = -1;
	size = VOne * 200.0f;
	rate = 0.0f;
	wordUpPos = position.y;
	isWord = false;
}

Point::~Point()
{
}

void Point::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int collisionId, int checkId)
{
	StageObject::Init(_hModel, _position, _scale, _rotate, _type,collisionId);
	obbCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_POINT);
	obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);


	checkPointId = checkId;
}

void Point::CollisionEvent(CollisionBase& me, CollisionBase& target)
{
	if (me.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_POINT)
		me.DeleteTagMe();
	effectManager->CreateEffekseer(position, nullptr, "FLASH", 1.0f, 60, false, VGet(0.7f, 0.7f, 0.7f), VZero);
	isWord = true;
}

void Point::Update()
{
	StageObject::Update();
	if (isWord) {
		if (rate < MAX_RATE) {
			rate += 0.02f;
			 wordUpPos= EaseInOut<float>(0.0f,200.0f, rate);
		}
		else {
			rate = 0.0f;
			isWord = false;
		}
	}
}

void Point::Draw()
{
	StageObject::Draw();
	if (isWord) {
		VECTOR3 wp = ConvWorldPosToScreenPos(VGet(position.x, position.y+ wordUpPos, position.z));
		SetFontSize(32);
		DrawFormatStringF(wp.x, wp.y, 0xB1FF08, "checkPoint");
		
		SetFontSize(DEFAULT_FONT_SIZE);
	}
}
