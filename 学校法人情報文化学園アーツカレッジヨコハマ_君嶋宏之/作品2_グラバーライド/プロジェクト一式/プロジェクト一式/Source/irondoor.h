#pragma once
#include "switchMoveObject.h"

class IronDoor : public SwitchMoveObject
{
public:
	IronDoor(SceneBase* _scene);
	~IronDoor();

	int Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId, int _switchId);

	void Update()override;
	void CollisionEvent(CollisionBase& me, CollisionBase& target)override;
	/// <summary>
	/// “–‚½‚è”»’èposition‚ÌŽí—Þ
	/// </summary>
	enum class SphereCollisionType {
		TOP,
		BOTTOM,
		MAX,
	};

	void SwitchObjectMoveProcess()override;
	void SwitchObjectBackProcess()override;
	/// <summary>
	/// “–‚½‚è”»’è—p‚Ìposition‚ðŽæ“¾
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	VECTOR3 GetCollisionPosition(SphereCollisionType type) { return collisionPositionV[(int)type]; }
	//void SetWaitTime();
private:
	std::vector<SphereCollision*>sphereCollisionV;
	std::vector<VECTOR3> collisionPositionV;
	int waitTime;//“–‚½‚Á‚½Œã‚Ì‘Ò‚¿ŽžŠÔ
	bool timeStart;//‘Ò‚¿ŽžŠÔ‚ÌŠJŽn
};