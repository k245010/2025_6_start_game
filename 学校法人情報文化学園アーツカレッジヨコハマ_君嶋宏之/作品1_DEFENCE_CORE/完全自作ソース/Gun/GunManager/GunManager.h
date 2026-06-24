#pragma once
#include "../Gun/Gun.h"
#include "../../Stage/StageObjectData.h"
#include "../../../Library/GameObject.h"
#include <unordered_map>
#include <unordered_set>

class Gun;
class GunManager
{
public:

	/// <summary>
	///				GunManagerインスタンスを返す
	/// </summary>
	/// <returns>	GunManagerインスタンス	</returns>
	static GunManager* GetGunManagerInstance();

	/// <summary>
	/// GunManagerインスタンスをdeleteする
	/// </summary>
	static void DeleteGunManagerInstance();

	/// <summary>
	///								Gunクラスのインスタンスをコンテナにpushする
	/// </summary>
	/// <param name="_gunIDName">	GunクラスのID名				</param>
	/// <param name="_gun">			Gunクラスポインタ			</param>
	void PushGunInstance(std::string _gunIDName, Gun* _gun);

	/// <summary>
	///						Gunクラスのインスタンスをコンテナから削除する
	/// </summary>
	/// <param name="_gun">	Gunクラスポインタ			</param>
	void PopGunInstance(Gun* _gun);

	void Update();
	void Draw();

private:

	static GunManager* gunManagerInstance;
	static bool isCollDeleteGunManagerFunction;					// DeleteGunManager関数が呼ばれたかどうか

	GunManager();
	~GunManager();

	/// <summary>
	/// GunDataのメンバの設定で扱う種類
	/// </summary>
	enum class GUN_DATA_SETTING_MEMBER_KIND
	{
		FIRE_EFFECT_KIND = 0,
		SHOT_SOUND_ID_LIST,
		BULLET_TARGET_DATA,
		BULLET_HIT_SOUND,
		BULLET_HIT_SUMMON_OBJ,
		BULLET_HIT_EFFECT,

		MAX
	};

	/// <summary>
	///								ImGui::Comboを使ってリスト内の文字列の選択を行う
	/// </summary>
	/// <param name="_beginName">	ImGui::Comboの名前										</param>
	/// <param name="_list">		表示する文字列のリスト									</param>
	/// <param name="_selectStr">	選択された文字列参照									</param>
	/// <param name="_selectFlag">	選択されたフラグ参照									</param>
	void DrawImGuiCombo(const std::string& _beginName, const std::unordered_set<std::string>& _list, std::string& _selectStr, bool& _selectFlag);

	/// <summary>
	///								文字列のリストを表示する
	/// </summary>
	/// <param name="_beginName">	Textを表示するウィンドウの名前		</param>
	/// <param name="_stringList">	文字列のリスト						</param>
	void DrawImGuiTextList(const std::string& _beginName, const std::unordered_map<std::string, std::string>& _stringList);

	/// <summary>
	///	どのGunDataを設定するか、選択するUpdate
	/// </summary>
	void SelectSettingGunDataUpdate();

	/// <summary>
	///											サウンドIDを設定するUpdate
	/// </summary>
	/// <param name="_textListName">			表示するリストの名前					</param>
	/// <param name="_holdSoundId">				現在保持しているサウンドIDのコンテナ	</param>
	/// <param name="_gunDataSetMemberKind">	設定するGunDataのメンバの種類			</param>
	void SettingSoundIDUpdate(std::string_view _textListName,std::set<Sound_ID::SOUND_ID>& _holdSoundId,const GUN_DATA_SETTING_MEMBER_KIND& _gunDataSettingMemberKind);
	
	/// <summary>
	/// GunDataをImGuiで設定するUpdate
	/// </summary>
	void SettingImGuiGunDataUpdate();

	/// <summary>
	/// GunDataの適用
	/// </summary>
	void AttachGunData();

	/// <summary>
	/// GunDataのリセット
	/// </summary>
	void ResetGunData();

	/// <summary>
	/// GunDateの書き出し
	/// </summary>
	void SaveGunData();

	std::unordered_map<std::string, std::list<Gun*>> gunList;				// Gunクラスを管理するコンテナ		key:ステージオブジェクトの種類 / value:Gunクラスポインタ
	std::unordered_map<std::string, GunInfo::GunData> gunDataSetList;		// 設定するGunDataを管理するコンテナ　		key:ステージオブジェクトの種類 / value:GunData
	std::unordered_map<std::string, GunInfo::GunData> gunDataAttachList;	// 適用するGunDataを管理するコンテナ　	key:ステージオブジェクトの種類 / value:元のGunData(初期化の読み込み時のGunData)
	std::unordered_map<std::string, GunInfo::GunData> gunDataRawList;		// 元のGunDataを管理するコンテナ　	key:ステージオブジェクトの種類 / value:元のGunData(初期化の読み込み時のGunData)

	//GunInfo::GunData setGunData;							// 設定するGunData
	//int selectStageObjectKind	= -1;						// 選択したステージオブジェクトの種類
	std::string selectGunIDName = "NONE";					// 選択したGunクラスID名
	bool isSelectGunData		= false;					// 設定するGunDataを選択したかどうか

	//std::string selectCollisionObjectKindStr;				// 選択した当たり判定オブジェクトの種類の文字列
	float setCollisionDataTargetDamge = 0.0f;				// 設定する当たり判定する相手に与えるダメージ量
	
	Transform setSummonStageObjectTransform;				// 設定する着弾点に召喚するステージオブジェクトのトランスフォーム
	
	std::string selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::MAX];	// 選択した文字列コンテナ
	bool isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::MAX];				// 選択したかのフラグ


	//int setSelectGunDataNum = 0;	// どのGunDataを設定するの選択するナンバー
};
