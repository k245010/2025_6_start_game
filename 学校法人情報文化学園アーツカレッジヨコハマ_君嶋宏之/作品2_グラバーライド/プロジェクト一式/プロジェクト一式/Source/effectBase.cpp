#include "EffectBase.h"
#include "effectBase.h"

EffectBase::EffectBase(SceneBase* _scene) :GameObject(_scene)
{
	active = false;
	aloowPos = nullptr;
	size = VGet(0, 0, 0);
	hPlayHandle = -1;
	hHandle = -1;
	time = 0;
	speed = 0;
	timeMax = -1;
}

EffectBase::~EffectBase()
{
	StopEffekseer3DEffect(hPlayHandle);
	StopEffekseer2DEffect(hPlayHandle);
}

void EffectBase::Draw()
{
	//Object::Draw();
}

void EffectBase::Init(VECTOR pos, VECTOR* _aloow, int _handle, float _speed, int _time, bool _loop, std::string _dataName)
{
	CommonInit(_handle, _speed, _time, _loop, _dataName);
	position = pos;
	aloowPos = _aloow;
}

void EffectBase::Init(VECTOR pos, VECTOR* _aloow, int _handle, float _speed, int _time, bool _loop, std::string _dataName, VECTOR _size, VECTOR _rotate)
{
	Init(pos, _aloow, _handle, _speed, _time, _loop, _dataName);
	size = _size;
	rotate = _rotate;
}

void EffectBase::Init(VECTOR2F pos, VECTOR2F* _aloow, int _handle, float _speed, int _time, bool _loop, std::string _dataName)
{
	CommonInit(_handle, _speed, _time, _loop, _dataName);
	position.x = pos.x;
	position.y = pos.y;
	if (_aloow != nullptr) {
		aloowPos->x = _aloow->x;
		aloowPos->y = _aloow->y;
	}
	else {
		aloowPos = nullptr;
	}
}

void EffectBase::Init(VECTOR2F pos, VECTOR2F* _aloow, int _handle, float _speed, int _time, bool _loop, std::string _dataName, VECTOR2F _size, VECTOR2F _rotate)
{
	Init(pos, _aloow, _handle, _speed, _time,_loop,_dataName);
	size = VGet(_size.x,_size.y, 1.0f);
	rotate = VGet(_rotate.x, _rotate.y, 0.0f);
}

void EffectBase::CommonInit(int _handle, float _speed, int _time, bool _loop,std::string _dataName)
{
	speed = _speed;
	active = true;
	timeMax = _time;
	time = 0;
	hHandle = _handle;
	size = VGet(1.0f, 1.0f, 1.0f);
	rotate = VGet(0.0f, 0.0f, 0.0f);
	loop = _loop;
	dataName = _dataName;
}
