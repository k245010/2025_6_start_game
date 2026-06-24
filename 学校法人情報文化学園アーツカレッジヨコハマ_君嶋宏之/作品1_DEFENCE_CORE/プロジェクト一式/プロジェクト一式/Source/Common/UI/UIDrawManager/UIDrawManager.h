#pragma once
#include <unordered_map>
#include <functional>

/// <summary>
/// UIなどの描画を最後にしたい、場合に使用
/// 
/// App.cppでObjectManagerのDraw関数の後で描画される
/// </summary>
class UIDrawManager
{
public:

	/// <summary>
	///				UIDrawManagerインスタンスを返す
	/// </summary>
	/// <returns>	UIDrawManagerインスタンス	</returns>
	static UIDrawManager* GetUIDrawManagerInstance();

	/// <summary>
	/// UIDrawManagerインスタンスをdeleteする
	/// </summary>
	static void DeleteUIDrawManagerInstance();

	/// <summary>
	///									UIの描画関数をコンテナにpushする
	/// </summary>
	/// <param name="_drawFuncition">	UIの描画関数										</param>
	/// <param name="_canDraw">			描画するかどうか									</param>
	/// <returns>						UI関数に対する識別IDナンバー　(削除する際に必須)	</returns>
	int PushUIDrawFunction(const std::function<void()>& _drawFuncition, bool _canDraw = true);

	/// <summary>
	///									UIの描画関数をコンテナから解放する
	/// </summary>
	/// <param name="_drawFuncition">	UIの描画関数に対するIDナンバー	</param>
	void PopUIDrawFunction(const int& _idNumber);

	/// <summary>
	///							個々の関数に対して、描画をするかどうか設定
	/// </summary>
	/// <param name="_number">	UIの描画関数に対する識別ナンバー	</param>
	/// <param name="_set">		描画をするかどうか					</param>
	void SetDrawFlag(const int& _number, bool _set);

	/// <summary>
	///							描画順番を設定する
	/// </summary>
	/// <param name="_number">	UIの描画関数に対する識別ナンバー			</param>
	/// <param name="_order">	描画優先値　値が大きいほど、先に描画する	</param>
	void SetDrawOrder(const int& _number, const int& _order);

	void Update();
	void Draw();

private:

	/// <summary>
	///									UIの描画関数をコンテナから解放する
	/// </summary>
	/// <param name="_drawFuncition">	UIの描画関数に対するIDナンバー	</param>
	void Pop(const int& _idNumber);

	/// <summary>
	/// 描画関数関連のデータ
	/// </summary>
	struct DrawFunctionData
	{
		int idNumber;						// UI描画関数の識別ナンバー
		std::function<void()> drawFunction;	// UI描画関数
		bool canDraw;						// 描画ができるかどうか
		int orderNum;						// 描画優先値　値が大きいほど、先に描画する

		/// <summary>
		///							コンストラクタ
		/// </summary>
		/// <param name="_id">		UI描画関数の識別ナンバー		</param>
		/// <param name="_drawFunc">UI描画関数						</param>
		/// <param name="_canDraw">	描画するかどうか				</param>
		/// <param name="_order">	描画優先値						</param>
		DrawFunctionData(const int& _id, const std::function<void()>& _drawFunc, bool _canDraw,const int& _order) : idNumber(_id), drawFunction(_drawFunc), canDraw(_canDraw), orderNum(_order) {};
		
	};


	static UIDrawManager* uiDrawManagerInstance;

	int uiDrawFunctionDataNumber = 0;					// コンテナのDrawFunctionDataの識別ナンバー
	std::list<int> noUseUIDrawFunctionDataNumberList;	// 使わなくなったコンテナのDrawFunctionDataの識別ナンバーを保存するコンテナ

	UIDrawManager();
	~UIDrawManager();

	std::unordered_map<int, std::list<DrawFunctionData>::iterator> drawFunctionIteratorList;	// 描画関数のコンテナのイテレーターと、識別ナンバーを保持したコンテナ
	std::list<DrawFunctionData> drawFunctionList;												// 描画関数のコンテナ　描画優先値順にソートする
	std::list<int> removeDrawFunctionIDList;													// 削除する描画関数の識別ナンバーを記録するコンテナ

	bool isOrderNumChange	= false;	// 描画優先値が変更されているかどうか
	bool isDrawing			= false;	// 描画中かどうか
};