#include "uiBase.h"
#include "../ImGui/imgui.h"

UiBase::UiBase(SceneBase* _scene) : GameObject(_scene)
{
	hImage = -1;
	onDraw = true;
}

UiBase::~UiBase()
{
	data.clear();
}

void UiBase::Update()
{
	for (auto itr = data.begin();itr != data.end();itr++)
	{
		UiProductionData& iData = *itr;
		iData.time--;
		
		if (iData.time > 0)
		{
			switch (iData.proKind)
			{
			case UI_PRODUCTION_KIND::SHAKE:
				Shake(&iData);
				break;
			case UI_PRODUCTION_KIND::MOVE:
				break;
			case UI_PRODUCTION_KIND::RETURN:
				break;
			default:
				break;
			}
		}
		else
		{
			DeleteTimeKind(iData.proKind);
			break;
		}
	}
}

void UiBase::Draw()
{
	if (!onDraw)
		return;
}

void UiBase::Shaking(const VECTOR2F& _basePos, VECTOR2F* _position, VECTOR2F powerPos, UI_MOVE_INFO_KIND _kind, int _time)
{
	UiProductionData initData;
	initData.basePos = _basePos;
	initData.position = _position;
	*initData.position = _basePos;
	initData.powerPos = powerPos;
	initData.proKind = UI_PRODUCTION_KIND::SHAKE;
	initData.moveKind = _kind;
	initData.time = _time;
	CreateTimeData(initData);
}

bool UiBase::CreateTimeData(UiProductionData _data)
{
	if (!IsSameTimeKind(_data.proKind))
	{
		data.push_back(_data);
		return true;
	}
	return false;
}

bool UiBase::IsSameTimeKind(UI_PRODUCTION_KIND _kind)
{
	if (data.size() > 0)
	{
		for (auto itr = data.begin();itr != data.end();itr++)
		{
			UiProductionData iData = *itr; //‚±‚±‚ÅAtime‚ðã‘‚«‚µ‚½‚Ù‚¤‚ª‚¢‚¢‚©‚à
			if (iData.proKind == _kind)
			{
				return true;
			}
		}
	}
	return false;
}

bool UiBase::DeleteTimeKind(UI_PRODUCTION_KIND kind)
{
	if (data.size() > 0)
	{
		for (auto itr = data.begin();itr != data.end();)
		{
			UiProductionData& iData = *itr;
			if (iData.proKind == kind)
			{
				*iData.position = iData.basePos;
				data.erase(itr);
				return true;
			}
			else
			{
				itr++;
			}
		}
	}
	return false;
}

void UiBase::Shake(UiProductionData* _data)
{
	float rand = (float)GetRandNum((int)_data->powerPos.x);
	_data->position->x = 0.0f ? 0.0f : rand + _data->basePos.x;

	rand = (float)GetRandNum((int)_data->powerPos.y);
	_data->position->y = 0.0f ? 0.0f : rand + _data->basePos.y;
}

void UiBase::Move(UiProductionData* _data)
{

}

void UiBase::Return(UiProductionData* _data)
{

}

int UiBase::GetRandNum(const int& maxNum)
{
	return GetRand(maxNum * 2) - maxNum;
}
