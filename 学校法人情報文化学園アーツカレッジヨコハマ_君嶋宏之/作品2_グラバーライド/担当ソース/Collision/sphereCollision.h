#pragma once
#include "collisionBase.h"


class SphereCollision : public CollisionBase
{
public:

	SphereCollision(Object* obj);
	~SphereCollision();

	void Draw() override;

	bool CollisionToSphere(SphereCollision* col) override;
	bool CollisionToOBB(OBBCollision* col) override;
	bool CollisionToRay(RayCollision* col) override { return false; }
	
	float CollisionToOBBLen(OBBCollision* col);

	//float GetRadius() { return (object->GetScale().x * object->GetSize()) / 2; }
	float GetRadius() { return object->GetScale().x * (GetVMax(object->GetSize()) / 2); }

	VECTOR GetVelocity() { return object->GetVelocity(); }

private:

	VECTOR L;
	VECTOR thisPosition;
	VECTOR returnVec;

	float CollRadius;
};