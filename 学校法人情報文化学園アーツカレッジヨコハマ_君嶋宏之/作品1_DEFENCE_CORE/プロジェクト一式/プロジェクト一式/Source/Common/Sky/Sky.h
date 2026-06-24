#pragma once
#include "../../Object3D/Object3D.h"

/// <summary>
/// スカイスフィアのクラス
/// </summary>
class Sky : public Object3D
{
public:

	Sky(const Transform& trans, const int& _hModel);
	~Sky();

	void Update() override;
	void Draw() override;

private:

	float angle;	// 回転角度
};