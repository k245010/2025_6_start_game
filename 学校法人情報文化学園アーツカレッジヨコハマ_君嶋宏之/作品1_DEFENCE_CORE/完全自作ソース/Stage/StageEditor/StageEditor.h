#pragma once
#include "../../Transform/Transform.h"
#include <string>
#include <vector>

class Input;
class Transform;
class StageManager;
class StageObjectBase;
class StageEditor
{
	friend StageManager;
	friend StageObjectBase;

public:

	StageEditor();
	~StageEditor();

	/// <summary>
	///								StageManagerポインタ変数を初期化
	/// </summary>
	/// <param name="_stageManager">StageManagerポインタ		</param>
	void Init(StageManager* _stageManager);

	void Update();
	void Draw();

private:

	/// <summary>
	/// ステージオブジェクトを選択するUpdate
	/// </summary>
	void SelectStageObjectUpdate();

	/// <summary>
	///				ステージオブジェクトを配置するUpdate
	/// </summary>
	/// <returns>	true:設置した / false:設置していない	</returns>
	bool PutStageObjectUpdate();

	/// <summary>
	/// 全く同じ情報を持ったステージオブジェクトを生成する
	/// </summary>
	void CreateCloneStageObject();

	/// <summary>
	/// レイキャストの始点終点の座標を設定する
	/// </summary>
	void SetRaycastPosition();

	/// <summary>
	///				編集するステージオブジェクトの準備処理
	/// </summary>
	/// <returns>	true:成功 / false:失敗	</returns>
	bool SetupEditStageObject();

	/// <summary>
	/// ブロック系のモデルハンドルを変更して、見た目を変化
	/// ImGuiが含まれているので、前後でImGui::Begin(),ImGui::End()を呼んでください
	/// </summary>
	void ChangeBlockModelHandleImGui();

	/// <summary>
	///									ステージオブジェクトのモデル情報の適用
	/// </summary>
	/// <param name="_modelContainer">	モデルのコンテナ									</param>
	/// <param name="_useModelIndex">	適用させるモデルのモデルコンテナインデクス			</param>
	void ApplyStageObjectModelInfo(const std::vector<int>& _modelContainer,const int& _useModelIndex);

	/// <summary>
	///				ブロック系のモデルハンドルをコンテナで返す
	/// </summary>
	/// <returns>	モデルハンドルのコンテナ		</returns>
	const std::vector<int> GetBlockModelHandles() const;

	StageManager* stageManager			= nullptr;
	StageObjectBase* editStageObject	= nullptr;
	Input* input						= nullptr;

	bool isChangeEditStageObject		= false;// 編集するステージオブジェクトが変更したかどうか	
	std::string selectBlockModelKindString;		// 選択したブロックモデルの種類文字列

	Transform editBeforeStageObjectTransform;	// 編集前のステージオブジェクトのトランスフォームを保存
	Transform copyTransform;					// コピーしたトランスフォームを保存

	Transform putStageObjectTransform;			// 設置するステージオブジェクトのトランスフォーム
	bool isSelectPutStageObjectKind		= false;// 設置するステージオブジェクトの種類の選択フラグ
	std::string selectPutStageObjectKindString;	// 設置するステージオブジェクトの種類の選択された文字列


	VECTOR3 raycastStartPos = VZero;	// レイキャストの始点座標
	VECTOR3 raycastEndPos	= VZero;	// レイキャストの終点座標
};
