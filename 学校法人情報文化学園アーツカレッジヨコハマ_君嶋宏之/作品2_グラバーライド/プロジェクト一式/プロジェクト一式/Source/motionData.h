#pragma once
#include "loadBase.h"
/// <summary>
/// モーションハンドルをロードするためのクラス中身はモデルロードとかわらない
/// </summary>
class MotionLoad:public LoadBase {
public:
	MotionLoad(SceneBase* _scene);
	~MotionLoad();
	void Load()override;
};