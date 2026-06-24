#include "StageEditor.h"
#include "../StageManager/StageManager.h"
#include "../BlockController/BlockController.h"
#include "../StageObjects/StageObjectBase.h"
#include "../../Transform/TransformFunction.h"
#include "../../Common/Input/Input.h"
#include "../../Common/Function/Function.h"
#include "../../../ImGui/imgui.h"
#include <unordered_set>
#include <assert.h>

StageEditor::StageEditor()
{
	selectBlockModelKindString = "0";
}

void StageEditor::Init(StageManager* _stageManager)
{
	assert(_stageManager != nullptr);
	stageManager	= _stageManager;

	input			= FindGameObject<Input>();
}

StageEditor::~StageEditor()
{
}

void StageEditor::Update()
{
	if (editStageObject != nullptr)
	{
		// 削除が決定されていたら
		if (editStageObject->DestroyRequested())
		{
			editStageObject = nullptr;
		}
	}

	// レイキャストの始点終点の座標設定
	SetRaycastPosition();

	// ステージオブジェクトの選択Update
	SelectStageObjectUpdate();

	ImGui::Begin("StageEditor");

	//_ 書き出し _//

	if (ImGui::Button("Save"))
	{
		// ステージデータの書き出し
		stageManager->SaveStage(stageManager->GetLoadStageNum());
	}

	if (editStageObject != nullptr)
	{
		std::string selectObjectName = "SelectObject : " + StageObjectData::GetToString(editStageObject->GetStageObjectKind());	// 選択したステージオブジェクトの種類名

		// 選択したステージオブジェクトの種類名を表示
		ImGui::Text(selectObjectName.c_str());

		// ステージオブジェクトのトランスフォーム設定
		SetTransformImGui(editStageObject->transform, 5.0f, -100000.0f, 100000.0f);

		// ブロック系のモデルハンドルを変更して、見た目を変化
		ChangeBlockModelHandleImGui();

		//_ 削除 _//

		if (ImGui::Button("Delete"))
		{
			// ステージオブジェクトの削除
			stageManager->DeleteStageObject(editStageObject);
			editStageObject	= nullptr;
		}

		ImGui::SameLine();

		if (ImGui::Button("Clone"))
		{
			// 全く同じ情報を持ったステージオブジェクトを生成する
			CreateCloneStageObject();
		}

		if (ImGui::Button("TransformReset"))
		{
			// 編集前のステージオブジェクトのトランスフォームへ設定
			editStageObject->transform = editBeforeStageObjectTransform;
		}

		if (ImGui::Button("TransformCopy"))
		{
			// トランスフォームをコピー
			copyTransform = editStageObject->transform;
		}

		if (ImGui::Button("TransformPaste"))
		{
			// コピーしたトランスフォームを代入
			editStageObject->transform = copyTransform;
		}

		isChangeEditStageObject = false;
	}

	if (ImGui::CollapsingHeader("PutStageObjectKindList"))
	{
		std::unordered_set<std::string> stageObjectKindStrCon;	// ステージオブジェクトの種類の文字列コンテナ

		for (const auto& collObjKindStr : StageObjectData::STAGE_OBJECT_KIND_STRING_LIST)
		{
			// ステージオブジェクトの種類の文字列をpush
			stageObjectKindStrCon.emplace(collObjKindStr.second);
		}

		// 設置するステージオブジェクトの種類の選択
		DrawImGuiCombo("StageObjectKindList", stageObjectKindStrCon, selectPutStageObjectKindString, isSelectPutStageObjectKind);
	}

	// 設置するステージオブジェクトが設定されていたら
	if (isSelectPutStageObjectKind)
	{
		// ステージオブジェクトを配置するUpdate
		PutStageObjectUpdate();
	}

	ImGui::End();
}

void StageEditor::Draw()
{

}

void StageEditor::SelectStageObjectUpdate()
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;	// ImGui上にマウスポインタがなかったら return

	if (!input->GetKeyConfigPut("_D_SELECT_STAGE_OBJECT"))
		return;	// ステージオブジェクトを選択する入力を行っていなかったら return

	// マウス座標のレイキャストで当たったステージオブジェクトを代入
	editStageObject	= stageManager->RaycastStageObject(raycastStartPos, raycastEndPos, {});

	// 編集するステージオブジェクトの準備処理
	SetupEditStageObject();
}

bool StageEditor::PutStageObjectUpdate()
{
	if (!input->GetKeyConfigPut("_D_PUT_STAGE_OBJECT"))
		return false;	// ステージオブジェクトを配置するキーを押していなかったら return
	
	if (!stageManager->CheckRaycastStageObject(raycastStartPos, raycastEndPos, {}, &putStageObjectTransform.position))
		return false;	// マウス座標のレイキャストがステージオブジェクトに当たっていなかったら return

	// ステージオブジェクトの設置と編集するステージオブジェクトポインタへの代入
	editStageObject = stageManager->CreateStageObject(putStageObjectTransform.position, StageObjectData::GetFromEnum(selectPutStageObjectKindString), false);

	// 編集するステージオブジェクトの準備処理 true:成功 / false:失敗
	return SetupEditStageObject();
}

void StageEditor::CreateCloneStageObject()
{
	if (editStageObject == nullptr)
		return;	// nullptrの場合は処理しないで return

	int saveUseModelIndex = editStageObject->GetModelData().useModelIndex;	// 使用するモデルインデクスを保存

	// ステージオブジェクトの設置と編集するステージオブジェクトポインタへの代入
	editStageObject = stageManager->CreateStageObject(editStageObject->transform.position, editStageObject->kind, false);

	// モデル情報を適用する
	ApplyStageObjectModelInfo(GetBlockModelHandles(), saveUseModelIndex);

	// 編集するステージオブジェクトの準備処理
	SetupEditStageObject();
}

void StageEditor::SetRaycastPosition()
{
	VECTOR2I mousePoint = VECTOR2I();	// マウス座標

	// マウスポインタの画面座標を代入
	GetMousePoint(&mousePoint.x, &mousePoint.y);

	// マウスポインタがある画面上の座標に該当する３Ｄ空間上の Near 面の座標を取得
	raycastStartPos = ConvScreenPosToWorldPos(VGet((float)mousePoint.x, (float)mousePoint.y, 0.0f));

	// マウスポインタがある画面上の座標に該当する３Ｄ空間上の Far 面の座標を取得
	raycastEndPos	= ConvScreenPosToWorldPos(VGet((float)mousePoint.x, (float)mousePoint.y, 1.0f));
}

bool StageEditor::SetupEditStageObject()
{
	if (editStageObject == nullptr)
		return false;	// ステージオブジェクトを生成できなかったら return

	// 編集前のトランスフォームを保存
	editBeforeStageObjectTransform = editStageObject->transform;

	isChangeEditStageObject = true;

	return true;
}

void StageEditor::ChangeBlockModelHandleImGui()
{
	const std::vector<int> modelHandles = GetBlockModelHandles();	// モデルハンドルコンテナ

	if (modelHandles.size() <= 1)
		return;	// モデルハンドルが１種類しかなかったら return

	std::vector<std::string> blockModelKindStrCon;	// ブロックモデルの種類文字列コンテナ	0～の数字文字を入れる
	// サイズを確保
	blockModelKindStrCon.resize(modelHandles.size());

	int count = 0;	// インデクスのカウント
	for (auto strItr = blockModelKindStrCon.begin();strItr != blockModelKindStrCon.end();strItr++, count++)
	{
		// 0～の文字列を設定
		*strItr = std::to_string(count);
	}

	bool isSelect = false;	// 選択したかどうか

	// ブロックモデルのリストを表示
	DrawImGuiCombo("BlockModels", blockModelKindStrCon, selectBlockModelKindString, isSelect);

	if (isSelect)
	{
		int selectNumber = stoi(selectBlockModelKindString);	// 選択されたナンバー

		// モデル情報を適用する
		ApplyStageObjectModelInfo(modelHandles, selectNumber);
	}

	// 編集するステージオブジェクトが変更されていたら
	if (isChangeEditStageObject)
	{
		int index = 0;
		for (auto modelItr = modelHandles.begin();modelItr != modelHandles.end();modelItr++, index++)
		{
			if (*modelItr != editStageObject->hModel)
				continue;	// モデルハンドルが一致していないので continue

			// 数字を文字列数字へ変換
			selectBlockModelKindString = std::to_string(index);
		}
	}
}

void StageEditor::ApplyStageObjectModelInfo(const std::vector<int>& _modelContainer, const int& _useModelIndex)
{
	assert(_useModelIndex >= 0 && _useModelIndex < _modelContainer.size());

	// モデルハンドルの変更  モデルハンドルの削除は、BlockControllerが行っています
	editStageObject->hModel = _modelContainer.at(_useModelIndex);

	// モデルデータの使用するモデルインデクスを設定
	editStageObject->modelData.useModelIndex = _useModelIndex;
}

const std::vector<int> StageEditor::GetBlockModelHandles() const
{
	StageObjectData::STAGE_OBJECT_KIND stageObjectKind = editStageObject->GetStageObjectKind();	// 編集するステージオブジェクトの種類

	if (!StageObjectData::IsBlockStageObjectKind(stageObjectKind))
		return {};	// ステージオブジェクトがブロック系でなかったら return

	const std::unordered_map<int, std::vector<int>>& blockModelHandles = BlockController::GetBlockController()->GetModelHandles();	// ブロックのモデルハンドルコンテナ

	HasUnorderedMapContainerKey(blockModelHandles, (int)stageObjectKind, true);

	// モデルハンドルコンテナを返す
	return blockModelHandles.at((int)stageObjectKind);
}
