#include "gravityManager.h"
//#include "object.h"
#include "player.h"
#include "stageObjectManager.h"

float GravityManager::worldGravity = 0.0f;
float GravityManager::beforeWorldGravity = 0.0f;

namespace {
	const std::string debugStringGravity[GRAVITY_NUM] = {
			{"無重力"},
			{"PLAYER : 下"},
			{"NORMAL : 下"},
			{"PLAYER : 右下"},
			{"NORMAL : 右下"},
			{"PLAYER : 右"},
			{"NORMAL : 右"},
			{"PLAYER : 右上"},
			{"NORMAL : 右上"},
			{"PLAYER : 上"},
			{"NORMAL : 上"},
			{"PLAYER : 左上"},
			{"NORMAL : 左上"},
			{"PLAYER : 左"},
			{"NORMAL : 左"},
			{"PLAYER : 左下"},
			{"NORMAL : 左下"},
	};

	std::map<GravityState, float> gravity;
	
}

GravityManager::GravityManager(SceneBase* _scene) :GameObject(_scene)
{
	gravityState = DOWN_GRAVITY;
	ChangeWorldGravity(DOWN_GRAVITY);

	/*for (int i = 0; i < 100; i++)
	{
		object[i] = nullptr;
	}*/
	gravity[DOWN_GRAVITY] = 0.0f;
	gravity[DOWN_GRAVITY_START] = 0.0f;
	gravity[DOWN_RIGHT_GRAVITY] = DX_PI_F / 4;
	gravity[DOWN_RIGHT_GRAVITY_START] = DX_PI_F / 4;
	gravity[RIGHT_GRAVITY] = DX_PI_F / 2;
	gravity[RIGHT_GRAVITY_START] = DX_PI_F / 2;
	gravity[UP_RIGHT_GRAVITY] = DX_PI_F / 2 + DX_PI_F / 4;
	gravity[UP_RIGHT_GRAVITY_START] = DX_PI_F / 2 + DX_PI_F / 4;
	gravity[UP_GRAVITY] = DX_PI_F;
	gravity[UP_GRAVITY_START] = DX_PI_F;
	gravity[UP_LEFT_GRAVITY] = -DX_PI_F / 2 - DX_PI_F / 4;
	gravity[UP_LEFT_GRAVITY_START] = -DX_PI_F / 2 - DX_PI_F / 4;
	gravity[LEFT_GRAVITY] = -DX_PI_F / 2;
	gravity[LEFT_GRAVITY_START] = -DX_PI_F / 2;
	gravity[DOWN_LEFT_GRAVITY] = -DX_PI_F / 4;
	gravity[DOWN_LEFT_GRAVITY_START] = -DX_PI_F / 4;
}

GravityManager::~GravityManager()
{
	for (auto& go : gravityObject) {
		go.object->DestroyMe();
		go.object = nullptr;
	}
	gravityObject.clear();
}

void GravityManager::Update()
{
	for (auto i = gravityObject.begin(); i != gravityObject.end();)
	{
		
		if (!i->object->IsActive()) {
			(i->object)->DestroyMe();
			(i->object) = nullptr;
			i = gravityObject.erase(i);
			continue;
		}
		else {
			if (!i->object->GetGActive() || !i->object->GetVisionMove() || i->object->GetsSlowMosion()) {
				i++;
				continue;
			}
			
		}

		switch (mode)
		{
		case GravityManager::PLAYER_CHANGE_MODE:
			if (i->object->GetObjectType() == (int)ObjectType::PLAYER) {
				i->gravityState = NO_GRAVITY;
			}
			else {
				i->gravityState = worldGravityState;
			}
			break;
		case GravityManager::ALL_CHANGE:
			i->gravityState = gravityState;
			break;
		default:
			i->gravityState = worldGravityState;
			break;
		}
		i->object->SetGravityRotation(i->gravityState, 0.0f);
		if (i->object->GetHitStopState().time > 0) {
			i++;
			continue;
		}
		i->object->GravityAddSpeed(i->gravityState);
		i->object->GravityMove(i->gravityState);
		i++;
	}
}

void GravityManager::Draw()
{
#ifdef _DEBUG
	for (auto i = gravityObject.begin(); i != gravityObject.end(); i++)
	{
		if (!i->object->GetGActive() || !i->object->GetVisionMove()) {
			if (i->object->GetObjectType() == (int)ObjectType::WALK_ENEMY || i->object->GetObjectType() == (int)ObjectType::PLAYER) {
				VECTOR drawPos = ConvWorldPosToScreenPos(i->object->GetPosition());
				VECTOR2I drawPos2 = V2Get((int)drawPos.x, (int)drawPos.y);
				VECTORDraw(drawPos2, dynamic_cast<CharaBase*>(i->object)->GetPosition(), nullptr);
			}
			continue;
		}

		catsString = debugStringGravity[i->gravityState];
		debugDraw = catsString.c_str();

		VECTOR drawPos = ConvWorldPosToScreenPos(i->object->GetPosition());
		DrawFormatStringF(drawPos.x, drawPos.y, 0xff00000, "%s", debugDraw);
	}
#endif
}

void GravityManager::CreateGravityObject(Object* _object)
{
	CreateGravityObject(_object, worldGravityState);
}

void GravityManager::CreateGravityObject(Object* _object, GravityState _gState)
{
	GravityObject object{ 0 };
	object.object = _object;
	object.gravityState = _gState;
	gravityObject.emplace_back(object);
}

int GravityManager::ChangeGravityObject(Object* _object, GravityState _gState)
{
	for (auto i = gravityObject.begin(); i != gravityObject.end(); i++)
	{
		if (i->object == _object)
		{
			i->gravityState = _gState;
			return 1;
		}
	}
	return -1;
}

void GravityManager::RemoveGravityObject(Object* _object)
{
	for (auto i = gravityObject.begin(); i != gravityObject.end();)
	{
		if (i->object == _object)
		{
			// 削除
			i->object->DestroyMe();
			i->object = nullptr;
			i = gravityObject.erase(i);
			break;
		}
		else {
			++i;
		}
	}
}

void GravityManager::ClearGravityObject()
{
	gravityObject.clear();
}

void GravityManager::SetGravityChangeMode(GravityChangeMode _mode, GravityState _gravityState)
{
	SetGravityChangeMode(_mode);
	gravityState = _gravityState;

	gravityState = MultGravityState(gravityState);
}

void GravityManager::SetGravityChangeMode(GravityChangeMode _mode)
{
	mode = _mode;
}

void GravityManager::ChangeWorldGravity(GravityState _gravityState)
{
	beforeWorldGravity = worldGravity;
	beforeWorldGravityState = worldGravityState;
	worldGravityState = _gravityState;
	worldGravity = GetGravityAngle(worldGravityState);

}

float GravityManager::GetGravityAngle(GravityState _state)
{
	float angle = NULL;
	auto g = gravity.find(_state);
	if (g == gravity.end()) {
		angle = NULL;
	}
	else {
		angle = g->second;
	}
	return angle;
}

GravityState GravityManager::GetReverceGravity(GravityState _state)
{
	GravityState state = NO_GRAVITY;
	switch (_state)
	{
	case DOWN_GRAVITY:
		state = UP_GRAVITY;
		break;
	case DOWN_GRAVITY_START:
		state = UP_GRAVITY_START;
		break;
	case DOWN_RIGHT_GRAVITY:
		state = UP_LEFT_GRAVITY;
		break;
	case DOWN_RIGHT_GRAVITY_START:
		state = UP_LEFT_GRAVITY_START;
		break;
	case RIGHT_GRAVITY:
		state = LEFT_GRAVITY;
		break;
	case RIGHT_GRAVITY_START:
		state = LEFT_GRAVITY_START;
		break;
	case UP_RIGHT_GRAVITY:
		state = DOWN_LEFT_GRAVITY;
		break;
	case UP_RIGHT_GRAVITY_START:
		state = DOWN_LEFT_GRAVITY_START;
		break;
	case UP_GRAVITY:
		state = DOWN_GRAVITY;
		break;
	case UP_GRAVITY_START:
		state = DOWN_GRAVITY_START;
		break;
	case UP_LEFT_GRAVITY:
		state = DOWN_RIGHT_GRAVITY;
		break;
	case UP_LEFT_GRAVITY_START:
		state = DOWN_RIGHT_GRAVITY_START;
		break;
	case LEFT_GRAVITY:
		state = RIGHT_GRAVITY;
		break;
	case LEFT_GRAVITY_START:
		state = RIGHT_GRAVITY_START;
		break;
	case DOWN_LEFT_GRAVITY:
		state = UP_RIGHT_GRAVITY;
		break;
	case DOWN_LEFT_GRAVITY_START:
		state = UP_RIGHT_GRAVITY_START;
		break;
	}
	return state;
}

GravityState GravityManager::MultGravityState(GravityState _nowState)
{
	return MultGravityState(_nowState, worldGravityState);
}

GravityState GravityManager::MultGravityState(GravityState _nowState, GravityState _world)
{
	int mult = -1;
	switch (_world)
	{
	case DOWN_GRAVITY_START:
	case DOWN_GRAVITY:
		mult = 0;
		break;
	case DOWN_RIGHT_GRAVITY_START:
	case DOWN_RIGHT_GRAVITY:
		mult = 1;
		break;
	case RIGHT_GRAVITY_START:
	case RIGHT_GRAVITY:
		mult = 2;
		break;
	case UP_RIGHT_GRAVITY_START:
	case UP_RIGHT_GRAVITY:
		mult = 3;
		break;
	case UP_GRAVITY_START:
	case UP_GRAVITY:
		mult = 4;
		break;
	case UP_LEFT_GRAVITY_START:
	case UP_LEFT_GRAVITY:
		mult = 5;
		break;
	case LEFT_GRAVITY_START:
	case LEFT_GRAVITY:
		mult = 6;
		break;
	case DOWN_LEFT_GRAVITY_START:
	case DOWN_LEFT_GRAVITY:
		mult = 7;
		break;
	default:
		//CreateMessageBox("重力方向変換ミス", "error!!");
		//assert(false);
		break;
	}
	int stateNum = (_nowState + (mult * 2));
	while (stateNum >= GRAVITY_NUM)
	{
		stateNum -= GRAVITY_NUM - 1;
	}
	return (GravityState)stateNum;
}

GravityState GravityManager::GetPlayerReverceChangeGravity(GravityState _state)
{
	GravityState state = MultGravityState(_state);
	return GetReverceGravity(state);
}

GravityState GravityManager::NearGravityNumState(float _angle, float _add)
{
	const int CompareNum = 5;
	float min = 100;
	int index = -1;
	if (_angle <= _add) {
		float Num[CompareNum]{};
		GravityState g[CompareNum]{
			UP_GRAVITY,
			UP_LEFT_GRAVITY,
			UP_RIGHT_GRAVITY,
			RIGHT_GRAVITY,
			LEFT_GRAVITY,
		};
		for (int i = 0; i < CompareNum; i++) {
			Num[i] = abs(_angle - abs(GetGravityAngle(g[i]) + _add));
			if (min >= Num[i]) {
				min = Num[i];
				index = i;
			}
		}
		if (index != -1) {
			return g[index];
		}
		else {
			return NO_GRAVITY;
		}
	}
	else {
		float Num[CompareNum]{};
		GravityState g[CompareNum]{
			DOWN_GRAVITY,
			DOWN_LEFT_GRAVITY,
			DOWN_RIGHT_GRAVITY,
			RIGHT_GRAVITY,
			LEFT_GRAVITY,
		};
		for (int i = 0; i < CompareNum; i++) {
			Num[i] = abs(_angle - abs(GetGravityAngle(g[i]) + _add));
			if (min >= Num[i]) {
				min = Num[i];
				index = i;
			}
		}
		if (index != -1) {
			return g[index];
		}
		else {
			return NO_GRAVITY;
		}
	}
	
}

GravityState GravityManager::AngleToGravityStateGet(float _angle,bool _start)
{
	GravityState _state = NO_GRAVITY;
	const float SMALL_DIFF = 0.05f;
	int stateNum = 0;
	for (int g = 0;  g < GRAVITY_NUM; g++) {
		if (gravity[(GravityState)g] + SMALL_DIFF >= _angle && gravity[(GravityState)g] - SMALL_DIFF <= _angle) {
			_state = (GravityState)g;
		}
	}
	stateNum = _state;
	if (_start) {
		stateNum -= 1;
		_state = (GravityState)stateNum;
	}
	return _state;
}
