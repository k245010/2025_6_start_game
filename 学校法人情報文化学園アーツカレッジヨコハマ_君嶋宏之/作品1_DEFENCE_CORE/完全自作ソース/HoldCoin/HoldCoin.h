#pragma once
#include "../../Library/GameObject.h"
#include "../../nlohmann/json.hpp"

/// <summary>
/// 保持するコインの情報
/// </summary>
namespace HoldCoinInfo
{
	/// <summary>
	/// 保持するコインデータ
	/// </summary>
	struct HoldCoinData
	{ 
		int holdCoinNum;	// 保持するコイン数
	};

	/// <summary>
	///								HoldCoinDataからjsonへ変換する
	/// </summary>
	/// <param name="_json">		json参照				</param>
	/// <param name="_holdCoinData">HoldCoinData			</param>
	inline void to_json(nlohmann::json& _json, const HoldCoinData& _holdCoinData)
	{
		_json = nlohmann::json
		{
			{ "HoldCoinNum",		_holdCoinData.holdCoinNum	},
		};
	}

	/// <summary>
	///								jsonからHoldCoinDataへ変換する
	/// </summary>
	/// <param name="_json">		json						</param>
	/// <param name="_holdCoinData">	int参照					</param>
	inline void from_json(const nlohmann::json& _json, HoldCoinData& _holdCoinData)
	{
		_json.at("HoldCoinNum").get_to(_holdCoinData.holdCoinNum);
	}
}

/// <summary>
/// プレイヤーが保持するコインの管理
/// </summary>
class HoldCoin : public GameObject
{
public:

	HoldCoin();
	~HoldCoin();

	void Update()		override;
	void CoinDraw();

	/// <summary>
	///				現在保持しているコイン数を返す
	/// </summary>
	/// <returns>	現在保持しているコイン数	</returns>
	int GetHoldCoin() { return holdCoin; }

	/// <summary>
	///						コインを追加する
	/// </summary>
	/// <param name="_add"> 追加するコイン数 </param>
	void AddCoin(int _add);

	/// <summary>
	///						保持しているコインから引数分引く
	/// </summary>
	/// <param name="_sub"> 引く値																	</param>
	/// <returns>			true: 保持している値が引いたことで0になった	/ false:0になっていない		</returns>
	bool SubCoin(int _sub);

	/// <summary>
	/// 消費量に対してコインが不足していたことを伝える	HoldCoinのUI処理などを実行するために使用
	/// </summary>
	void SetInsufficientCoin() { isInsufficientCoin = true; }

private:

	/// <summary>
	/// コインのデータを変更する
	/// </summary>
	void ChangeHoldCoinDataUpdate();

	/// <summary>
	///				保持するコインデータが保存されているファイルパスを返す
	/// </summary>
	/// <returns>	保持するコインデータが保存されているファイルパスの文字列				</returns>
	std::string GetHoldCoinDataFilePath();

	/// <summary>
	///								保持するコインデータを書き出し
	/// </summary>
	/// <param name="_holdCoinData">書き出す保持コインデータ					</param>
	void SaveHoldCoinData(const HoldCoinInfo::HoldCoinData& _holdCoinData);

	/// <summary>
	///								保持するコインデータをロードする
	/// </summary>
	/// <param name="_holdCoinData">保持するコインデータ参照	</param>
	void LoadHoldCoinData(HoldCoinInfo::HoldCoinData& _holdCoinData);

	int holdCoin				= 0;	// 保持しているコイン数
	bool isInsufficientCoin		= false;// コインが消費量に対して、不足していたかどうか
	float time					= 0.0f;	// 時間
	int alpha					= 255;	// 不透明度

	HoldCoinInfo::HoldCoinData setStartHoldCoinData;	// 設定するゲーム開始時の保持するコインデータ

	struct DrawCoin
	{
		int holdCoin	= 0;			// 描画する保持しているコイン数
		int subCoin		= 0;			// 描画する保持しているコイン数から引く値(自然数)
		int addCoin		= 0;			// 描画する保持しているコイン数から足す値

		VECTOR2 position;
	};
	DrawCoin drawCoin;

	enum UI_KIND
	{
		C_COIN,
		C_NUMBER,

		C_MAX,
	};

	int coinUIImage[C_MAX];
};
