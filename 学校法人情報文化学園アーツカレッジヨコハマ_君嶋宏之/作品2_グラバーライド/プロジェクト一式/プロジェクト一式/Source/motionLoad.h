#pragma once
#include "loadBase.h"
/// <summary>
/// モーションハンドルをロードするためのクラス中身はモデルロードとかわらない
/// </summary>
class MotionLoad:public LoadBase {

	MotionLoad(SceneBase* _scene);
	~MotionLoad();

};