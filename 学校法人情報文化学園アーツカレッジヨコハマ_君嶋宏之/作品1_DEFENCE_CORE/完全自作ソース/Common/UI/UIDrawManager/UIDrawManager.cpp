#include "UIDrawManager.h"
#include <assert.h>
#include "../../../../Library/DebugNew.h"

UIDrawManager* UIDrawManager::uiDrawManagerInstance = nullptr;

UIDrawManager::UIDrawManager()
{
}

UIDrawManager::~UIDrawManager()
{
	assert(drawFunctionList.empty() && "描画関数がインスタンス削除と同時に、コンテナから解放されていません。必ずPopUIDrawFunction()を呼んでください");

	drawFunctionIteratorList.clear();
	noUseUIDrawFunctionDataNumberList.clear();
	removeDrawFunctionIDList.clear();
}

UIDrawManager* UIDrawManager::GetUIDrawManagerInstance()
{
	if (uiDrawManagerInstance == nullptr)
	{
		uiDrawManagerInstance = new UIDrawManager();
	}
	return uiDrawManagerInstance;
}

void UIDrawManager::DeleteUIDrawManagerInstance()
{
	if (uiDrawManagerInstance != nullptr)
	{
		delete uiDrawManagerInstance;
		uiDrawManagerInstance = nullptr;
	}
}

int UIDrawManager::PushUIDrawFunction(const std::function<void()>& _drawFuncition, bool _canDraw)
{
	isOrderNumChange = true;

	//  使わなくなったコンテナのDrawFunctionDataの識別ナンバーを保存するコンテナが空でないとき
	if (!noUseUIDrawFunctionDataNumberList.empty())
	{	
		int noUseUIDrawFunctionDataNumber = *noUseUIDrawFunctionDataNumberList.begin();
		// drawFunctionListにDrawFunctionDataとしてpush
		drawFunctionList.emplace_back(DrawFunctionData(noUseUIDrawFunctionDataNumber,_drawFuncition, _canDraw, 0));

		// イテレーターと識別ナンバーをdrawFunctionIteratorListに記録　
		//drawFunctionIteratorList.emplace
		//(
		//	noUseUIDrawFunctionDataNumber,		// key:識別ナンバー
		//	std::prev(drawFunctionList.end())	// value:drawFunctionListのイテレーター
		//);	

		// 使用したので、ナンバーをコンテナから削除
		noUseUIDrawFunctionDataNumberList.pop_front();

		return noUseUIDrawFunctionDataNumber;
	}

	// drawFunctionListにDrawFunctionDataとしてpush
	drawFunctionList.emplace_back(DrawFunctionData(uiDrawFunctionDataNumber,_drawFuncition, _canDraw, 0));

	//// イテレーターと識別ナンバーをdrawFunctionIteratorListに記録　
	//drawFunctionIteratorList.emplace
	//(
	//	uiDrawFunctionDataNumber,			// key:識別ナンバー
	//	// std::prev(iterator)	ひとつ前のイテレーターを返す
	//	std::prev(drawFunctionList.end())	// value:drawFunctionListのイテレーター
	//);

	// 識別ナンバーを進める
	return uiDrawFunctionDataNumber++;
}

void UIDrawManager::PopUIDrawFunction(const int& _idNumber)
{
	// 描画中の場合
	if (isDrawing)
	{
		// 削除する描画関数の識別ナンバーを記録
		removeDrawFunctionIDList.emplace_back(_idNumber);
		return;	// 描画中の場合、ここで削除はしないので、return
	}

	// 描画関数の削除
	Pop(_idNumber);
}

void UIDrawManager::SetDrawFlag(const int& _number, bool _set)
{
	//auto drawFunctionItr = drawFunctionIteratorList.find(_number);

	//if (drawFunctionItr == drawFunctionIteratorList.end())
	//	return;	// ナンバーが見つからなかったら return

	//// 描画フラグを設定
	//drawFunctionItr->second->canDraw = _set;

	for (auto& drawFunction : drawFunctionList)
	{
		// 識別ナンバーが一致したら
		if (drawFunction.idNumber == _number)
		{
			drawFunction.canDraw = _set;
			break;	// 描画フラグを設定したので、break
		}
	}
}

void UIDrawManager::SetDrawOrder(const int& _number, const int& _order)
{
	//auto drawFunctionItr = drawFunctionIteratorList.find(_number);

	//if (drawFunctionItr == drawFunctionIteratorList.end())
	//	return;	// ナンバーが見つからなかったら return

	//// 現在の描画優先値と異なっていたら
	//if (drawFunctionItr->second->orderNum != _order)
	//{
	//	// 描画優先値を設定
	//	drawFunctionItr->second->orderNum = _order;
	//	isOrderNumChange = true;
	//}

	for (auto& drawFunction : drawFunctionList)
	{
		// 識別ナンバーが一致したら
		if (drawFunction.idNumber == _number)
		{
			// 現在の描画優先値と異なっていたら
			if (drawFunction.orderNum != _order)
			{
				// 描画優先値を設定
				drawFunction.orderNum	= _order;
				isOrderNumChange		= true;
				break;	// 描画優先値を設定したので、break
			}
		}
	}
}

void UIDrawManager::Update()
{
	
}

void UIDrawManager::Draw()
{
	for (auto removeDrawFunctionID : removeDrawFunctionIDList)
	{
		// 描画関数の削除
		Pop(removeDrawFunctionID);
	}
	removeDrawFunctionIDList.clear();

	// 描画優先値が変更されていたら
	if (isOrderNumChange)
	{
		// orderNumが大きい順にソートする
		drawFunctionList.sort([](DrawFunctionData& a, DrawFunctionData& b) { return a.orderNum > b.orderNum; });

		isOrderNumChange = false;
	}
	
	isDrawing = true;

	for (const auto& drawFunction : drawFunctionList)
	{
		if (drawFunction.drawFunction == nullptr)
			continue;	// 描画関数がnullなのでcontinue

		if (!drawFunction.canDraw)
			continue;	// 描画ができないのでcontinue

		// 描画関数の実行
		drawFunction.drawFunction();
	}

	isDrawing = false;
}

void UIDrawManager::Pop(const int& _idNumber)
{
	//for (auto drawFunctionIterator = drawFunctionIteratorList.begin();drawFunctionIterator != drawFunctionIteratorList.end();)
	//{
	//	if (drawFunctionIterator->first == _idNumber)
	//	{
	//		bool isFindItr = false;	// 一致するイテレーターを見つけたかどうか

	//		for (auto drawFunctionItr = drawFunctionList.begin();drawFunctionItr != drawFunctionList.end();)
	//		{
	//			// イテレーターが一致したら
	//			if (drawFunctionIterator->second == drawFunctionItr)
	//			{
	//				// ナンバーを使わなくなったのでコンテナにpush
	//				noUseUIDrawFunctionDataNumberList.emplace_back(drawFunctionIterator->first);

	//				// 描画関数の開放
	//				drawFunctionItr = drawFunctionList.erase(drawFunctionItr);
	//				isFindItr		= true;
	//				break;	// 描画関数を開放したので、break
	//			}
	//			drawFunctionItr++;
	//		}
	//		assert(isFindItr);

	//		// イテレーターの解放
	//		drawFunctionIterator = drawFunctionIteratorList.erase(drawFunctionIterator);
	//		return;	// 削除できたので、return
	//	}
	//	drawFunctionIterator++;
	//}

	bool isRemove = false;

	for (auto drawFunctionItr = drawFunctionList.begin();drawFunctionItr != drawFunctionList.end();)
	{
		// 識別ナンバーが一致したら
		if (drawFunctionItr->idNumber == _idNumber)
		{
			// ナンバーを使わなくなったのでコンテナにpush
			noUseUIDrawFunctionDataNumberList.emplace_back(drawFunctionItr->idNumber);

			isRemove = true;
			// 描画関数の開放
			drawFunctionItr = drawFunctionList.erase(drawFunctionItr);
			break;	// 描画関数を開放したので、break
		}
		drawFunctionItr++;
	}
	assert(isRemove && "IDナンバーが見つかりませんでした。");
}
