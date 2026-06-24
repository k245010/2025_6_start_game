#include "effectManager.h"
#include "debugScreen.h"
#include "config.h"

namespace {
	const float EFFECT_EDGE = -400.0f;
	const float EFFECT_EDGE_PLUS = 400.0f;
}

EffectManager::EffectManager(SceneBase* _scene) :GameObject(_scene)
{
	loadManager = SceneManager::CommonScene()->FindGameObject<LoadManager>();

	aloowImage = LoadGraph("data\\texture\\backGroundAloow3.png");
}

EffectManager::~EffectManager()
{
	DeleteAllEffect();
	DeleteGraph(aloowImage);
}

void EffectManager::DeleteAllEffect()
{
	for (auto e = effectBase.begin(); e != effectBase.end(); e++) {
		(*e)->DestroyMe();
	}
	effectBase.clear();

	for (auto e = effectBase2D.begin(); e != effectBase2D.end(); e++) {
		(*e)->DestroyMe();
	}

	effectBase2D.clear();

	for (auto e = effectAloow.begin(); e != effectAloow.end(); e++) {
		(*e)->DestroyMe();
	}
	effectAloow.clear();
}

void EffectManager::Update()
{
	for (auto e = effectBase.begin(); e != effectBase.end();) {
		(*e)->Update(SetPosPlayingEffekseer3DEffect, SetRotationPlayingEffekseer3DEffect, SetScalePlayingEffekseer3DEffect,
			SetSpeedPlayingEffekseer3DEffect, StopEffekseer3DEffect, PlayEffekseer3DEffect);
		if (!(*e)->IsActive()) {
			(*e)->DestroyMe();
			(*e) = nullptr;
			e = effectBase.erase(e);
		}
		else {
			e++;
		}
	}

	for (auto e = effectBase2D.begin(); e != effectBase2D.end();) {
		(*e)->Update(SetPosPlayingEffekseer2DEffect, SetRotationPlayingEffekseer2DEffect, SetScalePlayingEffekseer2DEffect,
			SetSpeedPlayingEffekseer2DEffect, StopEffekseer2DEffect, PlayEffekseer2DEffect);
		if (!(*e)->IsActive()) {
			(*e)->DestroyMe();
			(*e) = nullptr;
			e = effectBase2D.erase(e);
		}
		else {
			e++;
		}
	}
	for (auto e = effectAloow.begin(); e != effectAloow.end();) {
		if (!(*e)->IsActive()) {
			(*e)->DestroyMe();
			(*e) = nullptr;
			e = effectAloow.erase(e);
		}
		else {
			e++;
		}
	}

	UpdateEffekseer2D();
	UpdateEffekseer3D();
}

void EffectManager::Draw()
{
	Effekseer_Sync3DSetting();
	DrawEffekseer3D();
	DrawEffekseer2D();
}



int EffectManager::CreateEffekseer(VECTOR3 pos, VECTOR* _aloow, std::string _data, float _speed, int _time, bool _loop)
{
	effectBase.push_back(GetScene()->CreateGameObject<EffectBase>());
	//smokeEffect.push_back(GetScene()->CreateGameObject<EffectBase>());
	for (auto e = effectBase.begin(); e != effectBase.end(); e++) {
		if (!(*e)->IsActive()) {
			(*e)->Init(pos, _aloow, loadManager->GetHandle(_data), _speed, _time, _loop,_data);
			(*e)->Play(PlayEffekseer3DEffect);
			//(*e)->GetScene()->SetDrawOrder(*e, 96);
		}
	}
	return 1;
}

int EffectManager::CreateEffekseer(VECTOR3 pos, VECTOR* _aloow, std::string _data, float _speed, int _time, bool _loop, VECTOR _size, VECTOR _rotate)
{
	effectBase.push_back(GetScene()->CreateGameObject<EffectBase>());
	for (auto e = effectBase.begin(); e != effectBase.end(); e++) {
		if (!(*e)->IsActive()) {
			(*e)->Init(pos, _aloow, loadManager->GetHandle(_data), _speed, _time, _loop, _data,_size,_rotate);
			(*e)->Play(PlayEffekseer3DEffect);
		}
	}

	return 2;
}

int EffectManager::CreateEffekseer(VECTOR2F pos, VECTOR2F* _aloow, std::string _data, float _speed, int _time, bool _loop)
{
	effectBase2D.push_back(GetScene()->CreateGameObject<EffectBase>());
	for (auto e = effectBase2D.begin(); e != effectBase2D.end(); e++) {
		if (!(*e)->IsActive()) {
			(*e)->Init(pos, _aloow, loadManager->GetHandle(_data), _speed, _time, _loop, _data);
			(*e)->Play(PlayEffekseer2DEffect);
		}
	}
	return 3;
}

int EffectManager::CreateEffekseer(VECTOR2F pos, VECTOR2F* _aloow, std::string _data, float _speed, int _time, bool _loop, VECTOR2F _size, VECTOR2F _rotate)
{
	effectBase2D.push_back(GetScene()->CreateGameObject<EffectBase>());
	for (auto e = effectBase2D.begin(); e != effectBase2D.end(); e++) {
		if (!(*e)->IsActive()) {
			(*e)->Init(pos, _aloow, loadManager->GetHandle(_data), _speed, _time, _loop, _data, _size, _rotate);
			(*e)->Play(PlayEffekseer2DEffect);
		}
	}

	return 4;
}

int EffectManager::StopEffekseer(std::string _data,bool _2D)
{
	if (!_2D) {
		for (auto e = effectBase.begin(); e != effectBase.end();) {
			if ((*e)->GetName() == _data) {
				(*e)->Stop(StopEffekseer3DEffect);
				(*e)->DestroyMe();
				(*e) = nullptr;
				e = effectBase.erase(e);
				//return 0;
			}
			else {
				e++;
			}
		}
	}
	else {
		for (auto e = effectBase2D.begin(); e != effectBase2D.end();) {
			if ((*e)->GetName() == _data) {
				(*e)->Stop(StopEffekseer2DEffect);
				(*e)->DestroyMe();
				(*e) = nullptr;
				e = effectBase2D.erase(e);
			}
			else {
				e++;
			}
		}
	}
	return -1;
}

int EffectManager::StartAloow(VECTOR _pos, GravityState _state, float _angle, float _time)
{
	effectAloow.push_back(GetScene()->CreateGameObject<BackGroundAloow>());
	for (auto e = effectAloow.begin(); e != effectAloow.end(); e++) {
		if (!(*e)->IsActive()) {
			(*e)->Start(_pos, _state,_angle,aloowImage,_time);
			//(*e)->Play(PlayEffekseer3DEffect);
			GetScene()->SetDrawOrder(*e, 85);
		}
	}
	return 1;
}

EffectManager::AloowEffectData EffectManager::GetEffectData(GravityManager* _manager, GravityState _state)
{
	AloowEffectData data;
	GravityState s = _state;
	switch (_state)
	{
	case DOWN_GRAVITY_START:
	case DOWN_GRAVITY:
		s = _manager->GetReverceGravity(_state);
		data.position = VGet(SCREEN_WIDTH_CENTER, EFFECT_EDGE, 0.0f);
		break;
	case DOWN_RIGHT_GRAVITY_START:
		s = UP_RIGHT_GRAVITY_START;
		data.position = VGet(0.0f, EFFECT_EDGE, 0.0f);
		break;
	case DOWN_RIGHT_GRAVITY:
		s = UP_RIGHT_GRAVITY;
		data.position = VGet(0.0f, EFFECT_EDGE, 0.0f);
		break;
	case RIGHT_GRAVITY_START:
	case RIGHT_GRAVITY:
		data.position = VGet(EFFECT_EDGE, SCREEN_HEIGHT_CENTER, 0.0f);
		break;
	case UP_RIGHT_GRAVITY_START:
		s = DOWN_RIGHT_GRAVITY_START;
		data.position = VGet(EFFECT_EDGE, SCREEN_HEIGHT + EFFECT_EDGE_PLUS, 0.0f);
		break;
	case UP_RIGHT_GRAVITY:
		s = DOWN_RIGHT_GRAVITY;
		data.position = VGet(EFFECT_EDGE, SCREEN_HEIGHT + EFFECT_EDGE_PLUS, 0.0f);
		break;
	case UP_GRAVITY_START:
	case UP_GRAVITY:
		s = _manager->GetReverceGravity(_state);
		data.position = VGet(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT + EFFECT_EDGE_PLUS, 0.0f);
		break;
	case UP_LEFT_GRAVITY_START:
		s = DOWN_LEFT_GRAVITY_START;
		data.position = VGet(SCREEN_WIDTH, SCREEN_HEIGHT + EFFECT_EDGE_PLUS, 0.0f);
		break;
	case UP_LEFT_GRAVITY:
		s = DOWN_LEFT_GRAVITY;
		data.position = VGet(SCREEN_WIDTH, SCREEN_HEIGHT + EFFECT_EDGE_PLUS, 0.0f);
		break;
	case LEFT_GRAVITY_START:
	case LEFT_GRAVITY:
		data.position = VGet(SCREEN_WIDTH + EFFECT_EDGE_PLUS, SCREEN_HEIGHT_CENTER, 0.0f);
		break;
	case DOWN_LEFT_GRAVITY_START:
		s = UP_LEFT_GRAVITY_START;
		data.position = VGet(SCREEN_WIDTH, EFFECT_EDGE, 0.0f);
		break;
	case DOWN_LEFT_GRAVITY:
		s = UP_LEFT_GRAVITY;
		data.position = VGet(SCREEN_WIDTH, EFFECT_EDGE, 0.0f);
		break;
	default:
		assert(false);
		break;
	}
	data.angle = _manager->GetGravityAngle(s);
	return data;
}
