#include "obbCollision.h"
#include "sphereCollision.h"
#include "rayCollision.h"

OBBCollision::OBBCollision(Object* obj) : CollisionBase(obj)
{
	object = obj;
	kind = COLLISION_KIND::OBB;

	lastPosition = VGet(700, 0, 0);

	modelDirectionDone = false;

	frameIndex = -1;

	initDirection = false;
}

OBBCollision::~OBBCollision()
{
	DeleteModelDirection();
}

void OBBCollision::ModelDirectionInit(const int hModel)
{
	MV1SetupReferenceMesh(hModel, frameIndex, true);
	polygonList = MV1GetReferenceMesh(hModel, frameIndex, true);
	ModelDirectionInfo();
	modelDirectionDone = true;
}

void OBBCollision::Update()
{
	CollisionBase::Update();
	const int model = object->GetHmodel();

	if(!modelDirectionDone)
	{
		ModelDirectionInit(model);
		ModelDirectionInfo();
	}
}

void OBBCollision::Draw()
{
	// 当たり判定の描画
	VECTOR pos = object->GetPosition();
	VECTOR numV = GetLen(); 
	VECTOR min = VSub(pos, numV);
	VECTOR max = VAdd(pos, numV);
	DrawCube3D(min, max, 0xff00ff, 0xfffff, false);

	DrawFormatString(100, 500, 0xff00ff, "modelDirectionSize %d", modelDirection.size());

	int i = 0;
	for (VECTOR v : modelDirection)
	{
		i++;
		VECTORDraw(V2Get(100, 150 + (i * 50)), v, nullptr);

		VECTOR line = VGet(0, 0, 0);
		line = VMult(v, GetLen());
		line = VScale(line, 2);

		DrawLine3D(object->GetPosition(), VAdd(object->GetPosition(),line), 0xff0000);
	}

	for (int i = 0;i < 3;i++)
	{
		VECTOR line2 = GetVDirection(i);
		line2 = VScale(line2,GetVMem(GetLen(),i));
		DrawLine3D(object->GetPosition(), VAdd(object->GetPosition(), line2), 0x00ff00);
	}
}

bool OBBCollision::CollisionToSphere(SphereCollision* col)
{
	return col->CollisionToOBB(this);
}

bool OBBCollision::CollisionToRay(RayCollision* col)
{
	return col->CollisionToOBB(this);
}

bool OBBCollision::CollisionToOBB(OBBCollision* col)
{
	return false;
}

float OBBCollision::CollisionToOBBLen(OBBCollision* col)
{
	return 0.0f;
}

VECTOR OBBCollision::GetVDirection(int member) const
{
	VECTOR vec = VGet(0, 0, 0);

	SetVMem(vec, member, 1);

	VECTOR test = object->GetRotation();
	MATRIX mRot = MGetIdent();
	mRot = MMult(mRot, MGetRotZ(test.z));
	mRot = MMult(mRot, MGetRotX(test.x));
	mRot = MMult(mRot, MGetRotY(test.y));

	vec = VTransform(vec, mRot);

	return vec;
}

void OBBCollision::ModelDirectionInfo()
{
	DeleteModelDirection();

	initDirection = true;

	// モデルの全ての法線情報とブロックの６面に対する法線の近似値を変数に代入
	for (int i = 0;i < polygonList.PolygonNum;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			VECTOR norm = polygonList.Vertexs[polygonList.Polygons[i].VIndex[j]].Normal;
			norm = GetVMaxMemNormFabs(norm);

			if (modelDirection.size() == 0) 
			{
				AddModelDirection(norm);
			}
			else
			{
				auto itr = modelDirection.begin();
				for (itr;itr != modelDirection.end();itr++)
				{
					VECTOR v = *itr;
					if (VAllSame(v, norm)) 
						break;
				}
				
				if (itr == modelDirection.end()) 
				{
					AddModelDirection(norm);
				}
			}
		}
	}
}

void OBBCollision::AddModelDirection(const VECTOR& vec)
{
	modelDirection.push_back(vec);
}

void OBBCollision::DeleteModelDirection()
{
	if (modelDirection.size() > 0)
	{
		for (auto itr = modelDirection.begin();itr != modelDirection.end();itr++)
		{
			itr = modelDirection.erase(itr);

			if (itr == modelDirection.end())
				break;
		}
	}
}

VECTOR OBBCollision::GetNearModelDirection(const VECTOR& vec)
{
	auto itr = modelDirection.begin();

	if (modelDirection.size() <= 0)
		return VGet(0.0f, 0.0f, 0.0f);

	VECTOR re = *itr; 

	float min = VSize(VSub(VAdd((VECTOR)*itr, object->GetPosition()), vec));

	for (itr;itr != modelDirection.end();itr++)
	{
		VECTOR v = *itr;
		float distance = VSize(VSub(VAdd((VECTOR)*itr, object->GetPosition()), vec));
		if (distance < min)
		{
			min = distance;
			re = *itr;
		}
	}
	return re;
}