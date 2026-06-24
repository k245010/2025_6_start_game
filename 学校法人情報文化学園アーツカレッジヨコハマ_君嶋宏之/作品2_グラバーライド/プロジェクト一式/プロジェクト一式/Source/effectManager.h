#pragma once
#include "loadManager.h"
#include "effectBase.h"
#include "backGroundAloow.h"

class EffectManager : public GameObject
{
public:
	EffectManager(SceneBase* _scene);
	~EffectManager();

	void DeleteAllEffect();

	void Update()override;
	void Draw()override;

	int CreateEffekseer(VECTOR3 pos, VECTOR* _aloow, std::string _data, float _speed, int _time, bool _loop);
	int CreateEffekseer(VECTOR3 pos, VECTOR* _aloow, std::string _data, float _speed, int _time, bool _loop, VECTOR _size, VECTOR _rotate);

	int CreateEffekseer(VECTOR2F pos, VECTOR2F* _aloow, std::string _data, float _speed, int _time, bool _loop);
	int CreateEffekseer(VECTOR2F pos, VECTOR2F* _aloow, std::string _data, float _speed, int _time, bool _loop, VECTOR2F _size, VECTOR2F _rotate);

	int StopEffekseer(std::string _data,bool _2D);

	int StartAloow(VECTOR _pos,GravityState _state,float _angle, float _time);


	struct AloowEffectData
	{
		VECTOR position;
		float angle;
	};

	AloowEffectData GetEffectData(GravityManager* _manager,GravityState _state);

private:
	
	std::list<EffectBase*> effectBase;
	std::list<EffectBase*> effectBase2D;
	LoadManager* loadManager;
	std::list<BackGroundAloow*> effectAloow;
	int aloowImage;
};
