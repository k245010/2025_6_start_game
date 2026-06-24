#pragma once

/// <summary>
/// 罠のインターフェイスクラス
/// </summary>
class ITrap
{
public:
	
	virtual ~ITrap() = default;

	/// <summary>
	///				影響範囲の半径を返す	タレットなら攻撃半径
	/// </summary>
	/// <returns>	影響範囲の半径			</returns>
	virtual float GetImpactRadius() const = 0;

	/// <summary>
	///						メンテナンスを行う	タレットなら弾の補充など
	/// </summary>
	/// <param name="_add">	進行させるメンテナンス値	0.0f～1.0f(0～100%)	</param>
	/// <returns>			現在のメンテナンス値		0.0f～1.0f(0～100%)	</returns>
	virtual float Maintaining(const float& _add) = 0;

	/// <summary>
	///				メンテナンスできるかどうかを返す
	/// </summary>
	/// <returns>	true:メンテナンスできる / false:メンテナンスできない </returns>
	virtual bool CanMaintain() const = 0;

	/// <summary>
	///						引数の0.0f～1.0f(0～100%)分アップグレードを進める
	/// </summary>
	/// <param name="_add">	進行させるアップグレード値	0.0f～1.0f(0～100%)	</param>
	/// <returns>			現在のアップグレード値		0.0f～1.0f(0～100%)	</returns>
	virtual float Upgrading(const float& _add) = 0;
	
	/// <summary>
	///				アップグレードの進行値を0.0fにリセットする
	/// </summary>
	virtual void ResetUpgrading() = 0;

	/// <summary>
	///				現在アップグレード中か返す
	/// </summary>
	/// <returns>	true:アップグレード中 / false:アップグレード中でない	</returns>
	virtual bool IsUpgrading() = 0;

	/// <summary>
	///				アップグレードができるか返す
	/// </summary>
	/// <returns>	true:アップグレードできる / false:アップグレードできない	</returns>
	virtual bool CanUpgrade() = 0;

	/// <summary>
	///				現在のレベルを返す
	/// </summary>
	/// <returns>	現在のレベル				</returns>
	virtual int GetNowLevel() const = 0;

	/// <summary>
	///				自身のトランスフォームを返す
	/// </summary>
	/// <returns>	自身のトランスフォーム			</returns>
	virtual const Transform& GetTransform() const = 0;
};
