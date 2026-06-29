#pragma once
#include "../../../Library/myDxLib.h"
#include "../../../ImGui/imgui.h"
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>

/// <summary>
///								ImGui::Comboを使ってリスト内の文字列の選択を行う
/// </summary>
/// <param name="_beginName">	ImGui::Comboの名前										</param>
/// <param name="_list">		表示する文字列のリスト									</param>
/// <param name="_selectStr">	選択された文字列参照									</param>
/// <param name="_selectFlag">	選択されたフラグ参照									</param>
inline void DrawImGuiCombo(const std::string& _beginName, const std::unordered_set<std::string>& _list, std::string& _selectStr, bool& _selectFlag)
{
	if (ImGui::BeginCombo(_beginName.c_str(), _selectStr.c_str()))
	{
		for (const auto& list : _list)
		{
			if (ImGui::Selectable(list.c_str()))
			{
				_selectStr	= list;
				_selectFlag = true;
			}
		}
		ImGui::EndCombo();
	}
}

/// <summary>
///								ImGui::Comboを使ってリスト内の文字列の選択を行う
/// </summary>
/// <param name="_beginName">	ImGui::Comboの名前										</param>
/// <param name="_list">		表示する文字列のリスト									</param>
/// <param name="_selectStr">	選択された文字列参照									</param>
/// <param name="_selectFlag">	選択されたフラグ参照									</param>
inline void DrawImGuiCombo(const std::string& _beginName, const std::set<std::string>& _list, std::string& _selectStr, bool& _selectFlag)
{
	if (ImGui::BeginCombo(_beginName.c_str(), _selectStr.c_str()))
	{
		for (const auto& list : _list)
		{
			if (ImGui::Selectable(list.c_str()))
			{
				_selectStr	= list;
				_selectFlag = true;
			}
		}
		ImGui::EndCombo();
	}
}

/// <summary>
///								ImGui::Comboを使ってリスト内の文字列の選択を行う
/// </summary>
/// <param name="_beginName">	ImGui::Comboの名前										</param>
/// <param name="_list">		表示する文字列のリスト									</param>
/// <param name="_selectStr">	選択された文字列参照									</param>
/// <param name="_selectFlag">	選択されたフラグ参照									</param>
inline void DrawImGuiCombo(const std::string& _beginName, const std::vector<std::string>& _list, std::string& _selectStr, bool& _selectFlag)
{
	if (ImGui::BeginCombo(_beginName.c_str(), _selectStr.c_str()))
	{
		for (const auto& list : _list)
		{
			if (ImGui::Selectable(list.c_str()))
			{
				_selectStr	= list;
				_selectFlag = true;
			}
		}
		ImGui::EndCombo();
	}
}

/// <summary>
///								文字列のリストを表示する
/// </summary>
/// <param name="_beginName">	Textを表示するウィンドウの名前		</param>
/// <param name="_stringList">	文字列のリスト						</param>
inline void DrawImGuiTextList(const std::string& _beginName, const std::unordered_map<std::string, std::string>& _stringList, const ImVec2& _sliderSize = ImVec2(225.0f, 100.0f))
{
	ImGui::BeginChild(_beginName.c_str(), _sliderSize, false, 1);

	for (const auto& string : _stringList)
	{
		//_ データの出力 _//

		ImGui::TextUnformatted(string.first.c_str());

		// valueが空だったら
		if (string.second.empty())
			continue;	// 表示する文字列がないので、continue

		ImGui::SameLine();
		ImGui::TextUnformatted(string.second.c_str());
	}
	// データリストの一番下へスクロール　常に最新のデータを見るため
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
}

/// <summary>
///								unordered_mapコンテナがキーを持っているか返す
/// </summary>
/// <param name="_con">			std::unordered_map<TKey, TValue*>のコンテナ	</param>
/// <param name="_key">			調べたいキー								</param>
/// <param name="_doAssert">	assertを実行するかどうか					</param>
/// <returns>					true:持っている / false:持っていない		</returns>
template<typename TKey, typename TValue>
inline bool HasUnorderedMapContainerKey(const std::unordered_map<TKey, TValue>& _con, const TKey& _key, bool _doAssert = false)
{
	const std::unordered_map<TKey, TValue>::const_iterator itr = _con.find(_key);

	// キーが見つからなかったら
	if (itr == _con.end())
	{
		if (_doAssert)
			assert(false && "該当するキーが見つかりませんでした");
		return false;
	}
	return true;
}

/// <summary>
///									画像の指定矩形部分のみをビルボード描画
/// </summary>
/// <param name="_pos">				座標								</param>
/// <param name="_imageStartPos">	画像を切り取る初めの位置			</param>
/// <param name="_imageSize">		切り取りの大きさ					</param>
/// <param name="_imageCenterPos">	画像の中心座標						</param>
/// <param name="_scale">			大きさ								</param>
/// <param name="_angle">			回転値								</param>
/// <param name="_imageHandle">		画像ハンドル						</param>
/// <param name="_transFlag">		画像の透明度を有効にするかどうか	</param>
/// <param name="_reverseXFlag">	画像のX軸の反転をするかどうか		</param>
/// <param name="_reverseYFlag">	画像のY軸の反転をするかどうか		</param>
inline void DrawRectBillboard3D
(
	const VECTOR3& _pos,
	const VECTOR2I& _imageStartPos,
	const VECTOR2I& _imageSize,
	const VECTOR2& _imageCenterPos,
	const float& _scale,
	const float& _angle,
	const int& _imageHandle,
	const int& _transFlag,
	bool _reverseXFlag = false,
	bool _reverseYFlag = false
)
{
	int rectImage = DerivationGraph(_imageStartPos.x, _imageStartPos.y, _imageSize.x, _imageSize.y, _imageHandle);	// 切り取った画像ハンドル

	// ビルボード描画
	DrawBillboard3D(_pos, 0.0f, 0.5f, _scale, 0.0f, rectImage, _transFlag, _reverseXFlag, _reverseYFlag);

	// 画像ハンドルの削除
	DeleteGraph(rectImage);
}

/// <summary>
///									数字の画像を描画
/// </summary>
/// <param name="_x">				描画座標X							</param>
/// <param name="_y">				描画座標Y							</param>
/// <param name="_num">				描画数字							</param>
/// <param name="_interval">		数字同士の描画間隔					</param>
/// <param name="_numImage">		画像ハンドル						</param>
/// <param name="_width">			画像の幅							</param>
/// <param name="_height">			画像の高さ							</param>
/// <param name="_startY">			画像の描画初めのY座標				</param>
/// <param name="_exRate">			スケール							</param>
/// <param name="_angle">			回転値								</param>
/// <param name="_defaultZeroNum">	数字前の0表記の桁数					</param>
inline void DrawRotaNum
(
	const float& _x, 
	const float& _y, 
	const int& _num, 
	const int& _interval, 
	const int& _numImage, 
	const int& _width, 
	const int& _height, 
	const int& _startY, 
	const float& _scale, 
	const float& _angle, 
	const int& _defaultZeroNum = 0
)
{
	int tmp			= _num;
	int dig			= 0;
	int commaNum	= 0;

	do
	{
		DrawRectRotaGraphF(_x - (_interval * dig) - commaNum * 8, _y, tmp % 10 * _width, _startY, _width, _height, _scale, _angle, _numImage, true, false);
		tmp = tmp / 10;
		dig++;
	} while (tmp > 0 || dig < _defaultZeroNum);
}

/// <summary>
///									数字の画像をビルボード描画
/// </summary>
/// <param name="_pos">				座標					</param>
/// <param name="_num">				描画数字				</param>
/// <param name="_interval">		数字同士の描画間隔		</param>
/// <param name="_imageSize">		画像の大きさ			</param>
/// <param name="_startY">			画像の描画初めのY座標	</param>
/// <param name="_imageCenterPos">	画像の中心座標			</param>
/// <param name="_scale">			大きさ					</param>
/// <param name="_angle">			回転値					</param>
/// <param name="_numImage">		画像ハンドル			</param>
/// <param name="_defaultZeroNum">	数字前の0表記の桁数		</param>
inline void DrawRotaBillboardNum
(
	const VECTOR3& _pos,
	const int& _num,
	const int& _interval,
	const VECTOR2I& _imageSize,
	const int& _startY,
	const VECTOR2& _imageCenterPos,
	const float& _scale,
	const float& _angle,
	const int& _numImage,
	const int& _defaultZeroNum = 0
)
{
	int tmp = _num;
	int dig = 0;
	int commaNum = 0;

	do
	{
		// ビルボード描画
		DrawRectBillboard3D(VECTOR3(_pos.x - (_interval * dig) - commaNum * 8, _pos.y, _pos.z), VECTOR2I(tmp % 10 * _imageSize.x, _startY), _imageSize, _imageCenterPos, _scale, _angle, _numImage, true);
		tmp = tmp / 10;
		dig++;
	} while (tmp > 0 || dig < _defaultZeroNum);
}


/// <summary>
///	クラスポインタからクラス名を返す
/// </summary>
/// <param name="_classPtr">　	クラスポインタ	</param>
/// <returns>					クラス名		</returns>
template<class ClassPointer>
inline std::string GetClassNameString(const ClassPointer* _classPtr)
{
	return std::string(typeid(*_classPtr).name());
}
