#pragma once
#include "Transform.h"
#include "../Collision/CollisionFunction.h"
#include "../../ImGui/imgui.h"

/// <summary>
///							二つのトランスフォームのメンバが同じか返す
/// </summary>
/// <param name="_trans1">	トランスフォーム１				</param>
/// <param name="_trans2">	トランスフォーム２				</param>
/// <returns>				true:同じ / false:同じでない	</returns>
inline bool TransformAllSame(const Transform& _trans1, const Transform& _trans2)
{
	if (
		VAllSame(_trans1.position,	_trans2.position)	&&
		VAllSame(_trans1.rotation,	_trans2.rotation)	&&
		VAllSame(_trans1.scale,		_trans2.scale)		&&
		VAllSame(_trans1.size,		_trans2.size)		   
		)
	{
		return true;
	}
	return false;
}

/// <summary>
///								ImGuiを使ってトランスフォームを設定する
/// </summary>
/// <param name="_tramsform">	設定するトランスフォーム					</param>
/// <param name="_imguiName">	ImGuiのタイトル名							</param>
/// <param name="_dragSpeed">	ImGuiのドラッグのスピード					</param>
/// <param name="_min">			設定する値の最小値							</param>
/// <param name="_max">			設定する値の最大値							</param>
inline void SetTransformImGuiWindow(Transform& _tramsform, const char* _imguiName, const float& _dragSpeed = 1.0f,const float& _min = -1000.0f,const float& _max = 1000.0f)
{
	ImGui::Begin(_imguiName);

	ImGui::DragFloat3("Position", &_tramsform.position.x, _dragSpeed, _min, _max);
	ImGui::DragFloat3("Rotation", &_tramsform.rotation.x, _dragSpeed, _min, _max);
	ImGui::DragFloat3("Scale", &_tramsform.scale.x, _dragSpeed, _min, _max);
	ImGui::DragFloat3("Size", &_tramsform.size.x, _dragSpeed, _min, _max);

	ImGui::End();
}

/// <summary>
///								ImGuiを使ってトランスフォームを設定する ImGuiのBegin,Endを除いた中処理
/// </summary>
/// <param name="_tramsform">	設定するトランスフォーム					</param>
/// <param name="_dragSpeed">	ImGuiのドラッグのスピード					</param>
/// <param name="_min">			設定する値の最小値							</param>
/// <param name="_max">			設定する値の最大値							</param>
inline void SetTransformImGui(Transform& _tramsform, const float& _dragSpeed = 1.0f, const float& _min = -1000.0f, const float& _max = 1000.0f)
{
	ImGui::DragFloat3("Position", &_tramsform.position.x, _dragSpeed, _min, _max);
	ImGui::DragFloat3("Rotation", &_tramsform.rotation.x, _dragSpeed, _min, _max);
	ImGui::DragFloat3("Scale", &_tramsform.scale.x, _dragSpeed, _min, _max);
	ImGui::DragFloat3("Size", &_tramsform.size.x, _dragSpeed, _min, _max);
}