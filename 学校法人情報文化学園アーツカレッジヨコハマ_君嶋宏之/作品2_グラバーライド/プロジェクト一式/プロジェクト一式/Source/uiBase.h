#pragma once
#include "../Library/gameObject.h"
#include "../Library/myDxLib.h"
#include <vector>

enum class UI_MOVE_INFO_KIND
{
	NORMAL,
	UP,
	DOWN,
};

enum class UI_PRODUCTION_KIND
{
	SHAKE,	//振動
	MOVE,	//直線移動
	RETURN,	//往復
};

class UiBase : public GameObject
{
public:

	UiBase(SceneBase* _scene);
	~UiBase();

	virtual void Update() override;
	virtual void Draw() override;

	
	/// <summary>
	/// 座標をシェイクする
	/// </summary>
	/// <param name="_basePos">		初期値（関数終了時に戻す座標）	</param>
	/// <param name="_position">	シェイクする座標				</param>
	/// <param name="_powerPos">	シェイクするXYの最大値			</param>
	/// <param name="_kind">		基本的にNORMALを入れてください	</param>
	/// <param name="_time">		シェイク時間					</param>
	void Shaking(const VECTOR2F& _basePos,VECTOR2F* _position,VECTOR2F _powerPos, UI_MOVE_INFO_KIND _kind, int _time);

protected:

	int hImage;
	bool onDraw;
	

private:

	struct UiProductionData
	{
		int time;
		UI_PRODUCTION_KIND proKind;
		UI_MOVE_INFO_KIND moveKind;
		VECTOR2F basePos;
		VECTOR2F* position;
		VECTOR2F powerPos;
		VECTOR2F movePos;
	};

	std::vector<UiProductionData> data;

	bool CreateTimeData(UiProductionData _data);
	bool IsSameTimeKind(UI_PRODUCTION_KIND _kind);
	bool DeleteTimeKind(UI_PRODUCTION_KIND _kind);

	void Shake(UiProductionData* _data);
	void Move(UiProductionData* _data);
	void Return(UiProductionData* _data);

	int GetRandNum(const int& maxNum);
};
