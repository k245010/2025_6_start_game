#pragma once
#include "Object.h"
#include "collisionBase.h"
#include "sphereCollision.h"
#include "obbCollision.h"

class StageObject :public Object {
public:
	StageObject(SceneBase* scene);
	~StageObject();
	virtual int Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type,int _collisionId);
	//int Init(VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type);
private:

	/// <summary>
	/// 当たり判定を行わない例外インスタンスのCollisionインスタンスを削除
	/// </summary>
	/// <param name="_type"> ObjectType </param>
	/// <returns> 例外:例外でない </returns>
	bool IsException(const ObjectType& _type);
};
