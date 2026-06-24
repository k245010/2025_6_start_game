#pragma once
#include "BlockBase.h"

class SwampBlock : public BlockBase
{
public:
	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_hModel">			モデルハンドル					</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	SwampBlock(const Transform& _trans, const int& _hModel,const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_FREE)
		: BlockBase(_trans, _hModel, StageObjectData::STAGE_OBJECT_KIND::SWAMP_BLOCK,_hp,_putPlaceKind)	
	{ 
		transform.size = VOne; 
	}

	~SwampBlock() {};

private:

};
