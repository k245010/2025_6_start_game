#pragma once
#include "../Library/gameObject.h"

class Light : public GameObject
{
public:
	 Light(SceneBase* _scene);
	~Light();
	void Update()override;
	void Draw()override;

private:
	int hLight;
	VECTOR hLightPos;
};