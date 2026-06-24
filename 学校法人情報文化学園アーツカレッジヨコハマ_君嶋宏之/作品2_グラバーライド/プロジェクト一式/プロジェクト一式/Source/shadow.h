#pragma once
#include "Object.h"

class Shadow : public Object
{
public:
	Shadow(SceneBase* _scene);
	~Shadow();
	void Update()override;
	void Draw()override;

	void SetShadowPosition(VECTOR _position) { position = _position; }

private:

};