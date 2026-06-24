#pragma once
#include "Object.h"
#include <EffekseerForDXLib.h>

class EffectBase : public GameObject
{
public:
	EffectBase(SceneBase* _scene);
	~EffectBase();
	struct EffectData
	{
		VECTOR position;
		VECTOR rotate;
		VECTOR* aloowPos;
		bool active;

		VECTOR size;

		int hPlayHandle;
		int hHandle;

		int time;
		float speed;

		int timeMax;
	};
	//void Update()override;
	//void Update3D();
	//void Update2D();
	template<typename T, typename V,typename C,typename N,typename M,typename P>
	void Update(T _type1,V _type2,C _type3,N _type4,M _type5, P _type6){
		time++;
		if (aloowPos == nullptr) {
			_type1(hPlayHandle, position.x, position.y, position.z);
		}
		else {
			_type1(hPlayHandle, aloowPos->x, aloowPos->y, aloowPos->z);//’Ç]‚µ‚Ä‚¢‚­
		}

		_type2(hPlayHandle, rotate.x, rotate.y, rotate.z);
		_type3(hPlayHandle, size.x, size.y, size.z);
		_type4(hPlayHandle, speed);
		//SetColorPlayingEffekseer3DEffect(hPlayHandle, 255, 0, 0, 255);

		if (time >= timeMax) {
			if (!loop) {
				Stop(_type5);
			}
			else {
				time = 0;
				_type5(hPlayHandle);
				Play(_type6);
			}
		}
	}
	void Draw()override;

	bool IsActive() { return active; }

	void Init(VECTOR pos, VECTOR* _aloow, int _handle, float _speed, int _time,bool _loop,std::string _dataName);
	void Init(VECTOR pos, VECTOR* _aloow, int _handle, float _speed, int _time, bool _loop, std::string _dataName, VECTOR _size, VECTOR _rotate);
	void Init(VECTOR2F pos, VECTOR2F* _aloow, int _handle, float _speed, int _time, bool _loop, std::string _dataName);
	void Init(VECTOR2F pos, VECTOR2F* _aloow, int _handle, float _speed, int _time, bool _loop, std::string _dataName, VECTOR2F _size, VECTOR2F _rotate);

	void CommonInit(int _handle, float _speed, int _time, bool _loop, std::string _dataName);

	template<typename T>
	void Play(T _name) {
		hPlayHandle = _name(hHandle);
	}

	std::string GetName() { return dataName; }

	template<typename T>
	void Stop(T _name) {
		active = false;
		_name(hPlayHandle);
	}

private:

	VECTOR position;
	VECTOR rotate;
	VECTOR* aloowPos;
	bool active;

	VECTOR size;

	int hPlayHandle;
	int hHandle;

	int time;
	float speed;

	int timeMax;
	bool loop;
	std::string dataName;
};
