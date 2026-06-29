#pragma once
#include "../EntityBase.h"

class OBBCollision;
class RayCollision;
struct CollisionHitInfoData;
class Gun;
class Gauge;
/// <summary>
/// タレットの基底クラス
/// </summary>
class TurretBase : public EntityBase , public ITrap
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム				</param>
	/// <param name="_modelData">		モデルモデルデータ				</param>
	/// <param name="_kind">			ステージオブジェクトの種類		</param>
	/// <param name="_hp">				HP								</param>
	/// <param name="_putPlaceKind">	置いた場所の種類				</param>
	TurretBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind,const int& _hp, const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	virtual ~TurretBase();

	virtual void Update()	override;
	virtual void Draw()		override;

	/// <summary>
	///				攻撃半径を返す
	/// </summary>
	/// <returns>	攻撃半径			</returns>
	float GetImpactRadius() const override { return shotDistance; }

	/// <summary>
	///						罠のメンテナンスを行う	タレットなら弾の補充など
	/// </summary>
	/// <param name="_add">	進行させるメンテナンス値	0.0f～1.0f(0～100%)	</param>
	/// <returns>			現在のメンテナンス値		0.0f～1.0f(0～100%)	</returns>
	float Maintaining(const float& _add) override;

	/// <summary>
	///				メンテナンスできるかどうかを返す
	/// </summary>
	/// <returns>	true:メンテナンスできる / false:メンテナンスできない </returns>
	bool CanMaintain() const override { return (maintainNum <= 1.0f); }

	/// <summary>
	///						引数の0.0f～1.0f(0～100%)分アップグレードを進める　トラップのレベルが最大だったら、-1.0fを返す
	/// </summary>
	/// <param name="_add">	進行させるアップグレード値	0.0f～1.0f(0～100%)													</param>
	/// <returns>			トラップのレベルが最大でなかったら、現在のアップグレード値	0.0f～1.0f(0～100%) / トラップのレベルが最大だったら、-1.0fを返す	</returns>
	float Upgrading(const float& _add) override;

	/// <summary>
	///				アップグレードの進行値を0.0fにリセットする
	/// </summary>
	void ResetUpgrading() override { upgradingNum = 0.0f; }

	/// <summary>
	///				現在アップグレード中か返す
	/// </summary>
	/// <returns>	true:アップグレード中 / false:アップグレード中でない	</returns>
	bool IsUpgrading() override { return isUpgrading; }

	/// <summary>
	///				アップグレードができるか返す
	/// </summary>
	/// <returns>	true:アップグレードできる / false:アップグレードできない	</returns>
	bool CanUpgrade() override;

	/// <summary>
	///				弾を発射できるか返す
	/// </summary>
	/// <returns>	true:弾を発射できる / false:発射できない	</returns>
	bool CanShot() const { return canShot; }

	/// <summary>
	///				現在のレベルを返す
	/// </summary>
	/// <returns>	現在のレベル				</returns>
	int GetNowLevel() const override { return trapLevel; }

	/// <summary>
	///				自身のトランスフォームを返す
	/// </summary>
	/// <returns>	自身のトランスフォーム			</returns>
	const Transform& GetTransform() const override { return transform; }

	/// <summary>
	///						罠情報の描画をするかどうか設定
	/// </summary>
	/// <param name="_set">	true:描画する / false:描画しない	</param>
	//void SetTrapInfoDrawFlag(bool _set) {}

	/// <summary>
	/// 弾の残弾数のUI描画
	/// </summary>
	void BulletInfoDraw();

	/// <summary>
	/// 弾を発射できない時のUI描画
	/// </summary>
	void CanNotShotDraw();

	/// <summary>
	/// 罠のアップグレード関連のUI描画
	/// </summary>
	void TrapUpgradInfoDraw();

	/// <summary>
	/// メンテナンス警告の描画
	/// </summary>
	void MaintainWarnigDraw();

	/// <summary>
	/// メンテナンスキーの描画
	/// </summary>
	void MaintainKeyDraw();

protected:

	/// <summary>
	/// エンティティが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	bool HitEntity(const CollisionHitInfoData& _targetData) override;

	/// <summary>
	/// 発射Update
	/// </summary>
	void ShotUpdate();

	/// <summary>
	/// GunクラスのInitを呼ぶ
	/// </summary>
	void GunInit();

	//VECTOR3 GetPlayerToSquareDistance() { EntityController::GetEntityController()->GetPlayerPosition() -  }

	Gun* gun						= nullptr;	// 弾を発射するためにGunクラスを使う
	Gauge* trapUpgradingNumGauge	= nullptr;		// 罠のアップグレード進行値ゲージ

	VECTOR3 shotRay			= VZero;	// 発射のレイベクトル

	//float shotSpeed			= 0.0f;
	float shotDistance		= 0.0f;
	//float shotInterval		= 0.5f;
	//float shotIntervalCount = 0.0f;

	float maintainNum		= 1.0f;	// 現在のメンテナンス進行値
	float upgradingNum		= 0.0f;	// 現在のアップグレード進行数
	int trapLevel			= 1;	// 現在の罠レベル
	bool isUpgrading		= false;// アップグレード中かどうか


	bool canShot			= true;	// 発射できるかどうか

	bool isDeleteDuplicateModel = false;	// DuplicateModelハンドルを削除したかどうか

	enum BILLBOARD_IMAGE_KIND
	{
		BILL_WARNING = 0,		// メンテナンスしないといけない警告
		BILL_MAINTENANCE_KEY,	// メンテナンスkey

		BILL_MAX,
	};

	int billboardImageHandl[BILL_MAX];// ビルボード描画する画像ハンドル

	enum IMAGE_KIND
	{				
		IMAGE_BULLET_NUM = 0,		// 残弾ナンバー
		IMAGE_UPGRADE_COST_NUM,		// アップグレードコストの数字
		IMAGE_UPGRADE_COST_LETTER,	// アップグレードコストの文字
		IMAGE_MOUSE_RIGHT_CLICK,	// マウスの右クリック
		IMAGE_WARNING,				// 発射できないときの警告

		IMAGE_MAX
	};

	int imageHandle[IMAGE_MAX];
};