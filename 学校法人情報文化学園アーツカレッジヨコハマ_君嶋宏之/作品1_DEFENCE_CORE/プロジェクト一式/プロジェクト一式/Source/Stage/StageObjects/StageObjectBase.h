#pragma once
#include "../../Object3D/Object3D.h"
#include "../StageObjectData.h"
#include "../Trap/TrapInterface/ITrap.h"

class StageEditor;
/// <summary>
/// ステージ上のオブジェクトの基底クラス
/// </summary>
class StageObjectBase : public Object3D
{
	friend StageEditor;

public:
	
	/// <summary>
	/// 置かれた場所の種類
	/// </summary>
	enum class PUT_PLACE_KIND
	{
		PUT_FREE = 0,
		PUT_POINT,

		MAX,
	};

	/// <summary>
	/// モデルデータ
	/// </summary>
	struct ModelData
	{
		int modelHandle;	// モデルハンドル
		int useModelIndex;	// 使用するモデルハンドルのインデクス  書き出しの際に、どのモデルを使用したか記録

		/*---------------------------------------------------
		
		仕様：

		１種類のステージオブジェクトに対して、複数のモデルで描画にバリエーションを持たせているオブジェクトがある (罠のアップグレード時のモデル切り替えや背景のオブジェクト)
		
			useModelIndex			0         1    ..... 
			STAGE_OBJECT_KIND -> model1 , model2 , .....

		-----------------------------------------------------*/

		/// <summary>
		///									コンストラクタ	
		/// </summary>
		/// <param name="_hModel">			モデルハンドル						</param>
		/// <param name="_useModelIndex">	使用するモデルハンドルのインデクス	</param>
		ModelData(const int& _hModel,const int& _useModelIndex) : modelHandle(_hModel), useModelIndex(_useModelIndex) {}
		ModelData(const int& _hModel) : modelHandle(_hModel), useModelIndex(0) {}
		ModelData() : modelHandle(-1), useModelIndex(0) {}
	};

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="trans">			トランスフォーム			</param>
	/// <param name="_modelData">		モデルデータ				</param>
	/// <param name="_kind">			ステージオブジェクトの種類	</param>
	/// <param name="_hp">				HP							</param>
	/// <param name="_putPlaceKind">	置いた場所の種類			</param>
	StageObjectBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind,const int& _hp = 0,const PUT_PLACE_KIND& _putPlaceKind = PUT_PLACE_KIND::PUT_POINT);
	virtual ~StageObjectBase() {};

	virtual void Update()	override {};
	virtual void Draw()		override {};

	
	/// <summary>
	///				ステージオブジェクトの種類を返す
	/// </summary>
	/// <returns>	STAGE_OBJECT_KIND				</returns>
	StageObjectData::STAGE_OBJECT_KIND GetStageObjectKind() const { return kind; }

	/// <summary>
	///				置かれた場所の種類を返す
	/// </summary>
	/// <returns>	PUT_PLACE_KIND					</returns>
	PUT_PLACE_KIND GetPutPlaceKind() const { return putPlaceKind; }

	/// <summary>
	///				HPの値を返す
	/// </summary>
	/// <returns>	HP値			</returns>
	float GetHp() const { return hp; }

	/// <summary>
	///				モデルデータを返す
	/// </summary>
	/// <returns>	モデルデータ	</returns>
	const ModelData& GetModelData() const { return modelData; }

protected:

	/// <summary>
	///							モデルの当たり判定情報をセットアップする
	/// </summary>
	/// <param name="_hModel">	モデルハンドル		</param>
	/// <param name="_m">		モデルの行列		</param>
	void SetupCollInfo(const int _hModel,const MATRIX& _m);

	StageObjectData::STAGE_OBJECT_KIND kind;	// ブロックの種類
	PUT_PLACE_KIND putPlaceKind;				// 置かれた場所の種類
	bool active;								// アクティブかどうかのフラグ
	float hp;									// HP
	ModelData modelData;						// モデルデータ
};
