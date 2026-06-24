#include "GunManager.h"
#include "../../Common/Function/Function.h"
#include "../../Transform/TransformFunction.h"
#include "../../../ImGui/imgui.h"
#include "../../Common/Debug/Debug.h"
#include "../../../Library/DebugNew.h"

namespace
{
	constexpr float IMGUI_SLIDER_WIDTH = 150.0f;	// ImGui::SliderやImGui::Dragの表示幅
}

GunManager* GunManager::gunManagerInstance		= nullptr;
bool GunManager::isCollDeleteGunManagerFunction = false;

GunManager::GunManager()
{
	gunList.clear();
	gunDataSetList.clear();
	gunDataAttachList.clear();
	gunDataRawList.clear();

	for (int i = 0;i < (int)GUN_DATA_SETTING_MEMBER_KIND::MAX;i++)
	{
		isSelect[i] = false;
	}
}

GunManager::~GunManager()
{
	// newしたクラスがGunインスタンスをdeleteする

	gunList.clear();
	gunDataSetList.clear();
	gunDataAttachList.clear();
	gunDataRawList.clear();
}

GunManager* GunManager::GetGunManagerInstance()
{
	//assert(!isCollDeleteGunManagerFunction && "GunManagerインスタンスを削除した後に、newをしています。");

	if (gunManagerInstance == nullptr)
	{
		gunManagerInstance = new GunManager();
	}
	return gunManagerInstance;
}

void GunManager::DeleteGunManagerInstance()
{
	if (gunManagerInstance != nullptr)
	{
		delete gunManagerInstance;
		gunManagerInstance = nullptr;
	}
	isCollDeleteGunManagerFunction = true;
}

void GunManager::PushGunInstance(std::string _gunIDName, Gun* _gun)
{
	assert(!_gunIDName.empty() && "GunIDNameが設定されていません。");

	// すでにキーが存在したら
	if (gunList.find((std::string)_gunIDName) != gunList.end())
	{
		// 文字列の末尾に、ナンバーを付ける
		_gunIDName += std::to_string(gunList[_gunIDName].size());
		// Gunの名前を変更したので、GunクラスのID名を再設定
		_gun->SetGunIDName(_gunIDName);
	}
	// keyの初期化
	gunList[_gunIDName];
	// valueへのpush
	gunList[_gunIDName].emplace_back(_gun);

	// GunDataのpush //
	gunDataSetList.emplace(_gunIDName, GunInfo::GunData(_gun->GetGunData()));
	gunDataAttachList.emplace(_gunIDName, GunInfo::GunData(_gun->GetGunData()));
	gunDataRawList.emplace(_gunIDName, GunInfo::GunData(_gun->GetGunData()));
}

void GunManager::PopGunInstance(Gun* _gun)
{
	if (_gun == nullptr)
		return;	// nullなので削除処理を行わずreturn

	for (auto gunListItr = gunList.begin();gunListItr != gunList.end();)
	{
		for (auto gunItr = gunListItr->second.begin();gunItr != gunListItr->second.end();)
		{
			// Gunインスタンスが一致したら
			if (*gunItr == _gun)
			{
				// Gunをリストから削除
				gunItr = gunListItr->second.erase(gunItr);
				continue;	// 削除できたので、continue
			}
			gunItr++;
		}

		// Gunのリストの中が空だったら
		if (gunListItr->second.empty())
		{
			// 要素を削除
			gunListItr = gunList.erase(gunListItr);
			continue;	// 削除できたので、continue
		}
		gunListItr++;
	}
}

void GunManager::Update()
{
	if (!Debug::GetDebugMode())
		return;	// デバッグモードでなかったらImGuiを表示しない

	ImGui::Begin("GunInfo");

	// ImGui::Slider幅を設定する
	ImGui::PushItemWidth(IMGUI_SLIDER_WIDTH);

	if (!gunDataSetList.empty())
	{
		// どのGunDataを設定するか選択する
		SelectSettingGunDataUpdate();

		// 設定するステージオブジェクトの種類が決定されていたら
		if (selectGunIDName != "NONE")
		{
			if (ImGui::Button("Attach"))
			{
				// 適用処理
				AttachGunData();
			}

			ImGui::SameLine();

			if (ImGui::Button("Reset"))
			{
				// リセット処理
				ResetGunData();
			}

			if (ImGui::Button("Save"))
			{
				// 書き出し処理
				SaveGunData();
			}

			// GunDataの設定
			SettingImGuiGunDataUpdate();
		}
	}

	// ImGui::Slider幅設定を解除
	ImGui::PopItemWidth();

	ImGui::End();
}

void GunManager::DrawImGuiCombo(const std::string& _beginName, const std::unordered_set<std::string>& _list, std::string& _selectStr, bool& _selectFlag)
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

void GunManager::DrawImGuiTextList(const std::string& _beginName, const std::unordered_map<std::string, std::string>& _stringList)
{
	ImGui::BeginChild(_beginName.c_str(), ImVec2(IMGUI_SLIDER_WIDTH * 1.5f, 100.0f), false, 1);

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

void GunManager::SelectSettingGunDataUpdate()
{
	std::unordered_set<std::string> gunIDNameCon;	// GunIDNameのコンテナ

	for (const auto& gunData : gunDataSetList)
	{
		// GunIDNameのpush
		gunIDNameCon.emplace(gunData.first);
	}

	DrawImGuiCombo("GunDataList", gunIDNameCon, selectGunIDName, isSelectGunData);
}

void GunManager::SettingSoundIDUpdate(std::string_view _textListName,std::set<Sound_ID::SOUND_ID>& _holdSoundId,const GUN_DATA_SETTING_MEMBER_KIND& _gunDataSettingMemberKind)
{
	std::unordered_set<std::string> soundIDStrCon;							// SOUND_IDの文字列コンテナ
	std::unordered_map<std::string, std::string> soundIDStrDrawListCon;		// SOUND_IDの文字列をリスト表示するコンテナ
	std::string& selectShotSoundIDStr = selectString[(int)_gunDataSettingMemberKind];

	for (const auto& soundIDStr : Sound_ID::SOUND_ID_STRING_LIST)
	{
		// SOUND_IDの文字列をpush
		soundIDStrCon.emplace(soundIDStr.second);
	}

	for (const auto& soundID : _holdSoundId)
	{
		// SOUND_IDの文字列をkeyとして初期化
		soundIDStrDrawListCon[Sound_ID::GetToString(soundID)];
	}

	// 現在設定されているSOUND_IDの文字列をリスト表示
	DrawImGuiTextList(std::string(_textListName).c_str(), soundIDStrDrawListCon);
	// SOUND_IDの文字列の選択
	DrawImGuiCombo("SOUND_ID_LIST", soundIDStrCon, selectShotSoundIDStr, isSelect[(int)_gunDataSettingMemberKind]);

	if (isSelect[(int)_gunDataSettingMemberKind])
	{
		bool isPushButton = false;

		if (ImGui::Button("Add"))
		{
			// 選択したSOUND_IDをpush
			_holdSoundId.emplace(Sound_ID::GetFromEnum(selectShotSoundIDStr));

			isPushButton = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{
			auto soundIDItr = _holdSoundId.find(Sound_ID::GetFromEnum(selectShotSoundIDStr));

			// 選択したSOUND_IDを削除
			soundIDItr		= _holdSoundId.erase(soundIDItr);

			isPushButton	= true;
		}

		if (isPushButton)
		{
			// 文字列の表示を無にする
			selectShotSoundIDStr = "";
			// 選択したフラグをリセット
			isSelect[(int)_gunDataSettingMemberKind] = false;
		}
	}
}

void GunManager::SettingImGuiGunDataUpdate()
{
	HasUnorderedMapContainerKey(gunDataSetList, selectGunIDName, true);

	GunInfo::GunData& selectGunData = gunDataSetList[selectGunIDName];

	//_ 弾の調整 _//
	  
	if(ImGui::CollapsingHeader("BulletData"))
	{
		// インデントの開始
		ImGui::Indent();

		// 弾の速度の調整
		ImGui::DragFloat("Speed", &selectGunData.bulletData.speed, 1.0f, 0.1f, 20000.0f);
		// 弾の抵抗値の調整
		ImGui::DragFloat3("Friction", &selectGunData.bulletData.friction.x, 1.0f, 0.1f, 1000.0f);
		
		//_ 弾の重力フラグ _//

		if (ImGui::Button("IsGravity"))
		{
			selectGunData.bulletData.isGravity = !selectGunData.bulletData.isGravity;
		}
		ImGui::SameLine();

		if (selectGunData.bulletData.isGravity)
			ImGui::Text(":True");
		else
			ImGui::Text(":False");

		// 弾の直径の調整
		ImGui::DragFloat("Size", &selectGunData.bulletData.size, 1.0f, 1.0f, 2000.0f);
		// 弾の射程距離の調整
		ImGui::DragFloat("ShotRange", &selectGunData.bulletData.range, 1.0f, 1.0f, 2000.0f);
		// 弾の生存時間の調整
		ImGui::DragFloat("LifeTime", &selectGunData.bulletData.lifeTime, 1.0f, 0.1f, 30.0f);
		
		//_ 弾を当てる相手とダメージ量のコンテナの調整 _//

		if (ImGui::CollapsingHeader("TargetDataListInfo"))
		{
			std::unordered_map<std::string,std::string> collisionTargetDataStrCon;	// 当たり判定する相手の当たり判定オブジェクトのデータ文字列コンテナ
			std::unordered_set<std::string>	collisionObjKindStrCon;					// 当たり判定オブジェクトの種類の文字列コンテナ

			std::unordered_map<COLLISION_OBJECT_KIND, float>& collisionTargetDataList = selectGunData.bulletData.targetDataList;

			for (const auto& collObjKindStr : CollisionObjectKindInfo::COLLISION_OBJECT_KIND_STRING_LIST)
			{
				// 当たり判定オブジェクトの種類の文字列をpush
				collisionObjKindStrCon.emplace(collObjKindStr.second);
			}

			for (const auto& collTargetData : collisionTargetDataList)
			{
				std::string collTargetKindStr = CollisionObjectKindInfo::GetToString(collTargetData.first);

				// 当たり判定する相手の当たり判定オブジェクトの種類と与えるダメージ量をpush
				collisionTargetDataStrCon.emplace(collTargetKindStr, std::to_string(collTargetData.second));
			}

			// 当たり判定する相手の当たり判定オブジェクトに関するデータのリスト表示
			DrawImGuiTextList("CollisionDataList", collisionTargetDataStrCon);
			// 当たり判定する相手の当たり判定オブジェクトの種類の選択
			DrawImGuiCombo("CollisionObjectKindList", collisionObjKindStrCon, selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_TARGET_DATA], isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_TARGET_DATA]);

			if (isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_TARGET_DATA])
			{
				COLLISION_OBJECT_KIND selectCollisionObjKind = CollisionObjectKindInfo::GetFromEnum(selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_TARGET_DATA]);	// 選択された当たり判定する相手の当たり判定オブジェクトの種類

				ImGui::DragFloat("ToDamageNum", &setCollisionDataTargetDamge, 1.0f, 0.0f, 1000.0f);

				bool isChangeCollData = false;	// 当たり判定データを変更したかどうか

				if (ImGui::Button("Add"))
				{
					if (collisionTargetDataList.find(selectCollisionObjKind) == collisionTargetDataList.end())
					{
						// 選択した当たり判定データの種類と与えるダメージ量をpush
						collisionTargetDataList.emplace(selectCollisionObjKind, setCollisionDataTargetDamge);
					}
					else
					{
						// 選択した当たり判定データの与えるダメージ量を設定
						collisionTargetDataList[selectCollisionObjKind] = setCollisionDataTargetDamge;
					}
					isChangeCollData = true;
				}

				ImGui::SameLine();

				if (ImGui::Button("Delete"))
				{
					auto collDataItr = collisionTargetDataList.find(selectCollisionObjKind);

					// keyが存在したら
					if (collDataItr != collisionTargetDataList.end())
					{
						// 当たり判定データの削除
						collDataItr			= collisionTargetDataList.erase(collDataItr);
						isChangeCollData	= true;
					}
				}

				// 当たり判定データが変更されていたら
				if (isChangeCollData)
				{
					// 選択状況をリセット
					isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_TARGET_DATA] = false;
					// 文字の表示を無にする
					selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_TARGET_DATA] = "";
				}
			}
		}

		//_ 弾の着弾点に召喚するステージオブジェクトコンテナの調整 _//

		if (ImGui::CollapsingHeader("HitPosSummonStageObjectKindListInfo"))
		{
			std::unordered_map<std::string, std::string> hitPosSummonStageObjectKindStrCon;	// 当たり判定する相手の当たり判定オブジェクトのデータ文字列コンテナ
			std::unordered_set<std::string>	stageObjKindStrCon;								// ステージオブジェクトの種類の文字列コンテナ

			std::set<SummonStageObjectInfo::SummonStageObjectData>& hitPosSummonStageObjectList = selectGunData.bulletData.hitPosSummonStageObjectList;

			for (const auto& collObjKindStr : StageObjectData::STAGE_OBJECT_KIND_STRING_LIST)
			{
				// ステージオブジェクトの種類の文字列をpush
				stageObjKindStrCon.emplace(collObjKindStr.second);
			}

			for (const auto& summonStageObj : hitPosSummonStageObjectList)
			{
				// 現在召喚するステージオブジェクトの種類の文字列を初期化
				hitPosSummonStageObjectKindStrCon[StageObjectData::GetToString(summonStageObj.stageObjectKind)];
			}

			// 当たり判定する相手の当たり判定オブジェクトに関するデータのリスト表示
			DrawImGuiTextList("HitPosSummonStageObjectKindList", hitPosSummonStageObjectKindStrCon);
			// 当たった座標に召喚するステージオブジェクトの種類の選択
			DrawImGuiCombo("StageObjectKindList", stageObjKindStrCon, selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_SUMMON_OBJ], isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_SUMMON_OBJ]);

			if (isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_SUMMON_OBJ])
			{
				// トランスフォームの設定
				SetTransformInImGui(setSummonStageObjectTransform);

				SummonStageObjectInfo::SummonStageObjectData selectStageObject = 
					SummonStageObjectInfo::SummonStageObjectData(StageObjectData::GetFromEnum(selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_SUMMON_OBJ]), setSummonStageObjectTransform);	// 選択された召喚するステージオブジェクトの種類

				bool isChangeSummonStageObjectData = false;	// 当たり判定データを変更したかどうか

				if (ImGui::Button("Add"))
				{
					auto summonStageObjKindItr = hitPosSummonStageObjectList.find(selectStageObject);

					// keyが存在したら
					if (summonStageObjKindItr != hitPosSummonStageObjectList.end())
					{
						// 召喚するステージオブジェクトの削除
						summonStageObjKindItr = hitPosSummonStageObjectList.erase(summonStageObjKindItr);
					}

					// 召喚するステージオブジェクトをpush
					hitPosSummonStageObjectList.insert(selectStageObject);
					isChangeSummonStageObjectData = true;
				}

				ImGui::SameLine();

				if (ImGui::Button("Delete"))
				{
					auto summonStageObjKindItr = hitPosSummonStageObjectList.find(selectStageObject);

					// keyが存在したら
					if (summonStageObjKindItr != hitPosSummonStageObjectList.end())
					{
						// 召喚するステージオブジェクトの削除
						summonStageObjKindItr			= hitPosSummonStageObjectList.erase(summonStageObjKindItr);
						isChangeSummonStageObjectData	= true;
					}
				}

				// 当たり判定データが変更されていたら
				if (isChangeSummonStageObjectData)
				{
					// 選択状況をリセット
					isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_SUMMON_OBJ] = false;
					// 文字の表示を無にする
					selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_SUMMON_OBJ] = "";
				}
			}
		}

		//_ 弾の着弾時のサウンドの調整 _//

		if (ImGui::CollapsingHeader("HitPosSoundIDListInfo"))
		{
			// サウンドIDを設定
			SettingSoundIDUpdate("HitPosSoundIDList", selectGunData.bulletData.hitPosSoundIDList, GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_SOUND);
		}

		//_ 弾の着弾点のエフェクトの調整 _//

		selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_EFFECT] = EffekseerObjectManager::GetToString(selectGunData.bulletData.hitPosEffectKind);	// 選択した文字列

		if (ImGui::CollapsingHeader("HitPosEffectKind"))
		{
			std::unordered_set<std::string> effectStrCon;	// EFFEKSEER_KINDの文字列コンテナ

			for (const auto& effekseerKindStr : EffekseerObjectManager::EFFEKSEER_KIND_STRING_LIST)
			{
				// EFFEKSEER_KINDの文字列をpush
				effectStrCon.emplace(effekseerKindStr.second);
			}
			DrawImGuiCombo("EFFEKSEER_KIND_LIST", effectStrCon, selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_EFFECT], isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_EFFECT]);

			if (isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_EFFECT])
			{
				// 選択されたEFFEKSEER_KINDへ変更
				selectGunData.bulletData.hitPosEffectKind = EffekseerObjectManager::GetFromEnum(selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::BULLET_HIT_EFFECT]);
			}
		}

		// インデントの終了
		ImGui::Unindent();
	}

	// 二重線を引く //
	ImGui::Separator();
	ImGui::Separator();

	// 現在マガジンに入っているの弾数の調整
	ImGui::DragInt("Ammo", &selectGunData.ammo, 1.0f, 0, 99999);

	// ammoを除いた弾の保持数の調整
	ImGui::DragInt("HoldBullet", &selectGunData.holdBullet, 1.0f, 0, 99999);

	// １マガジンのサイズの調整
	ImGui::DragInt("MagazineSize", &selectGunData.magazineSize, 1.0f, 0, 99999);

	// 連射率の調整　数値が小さいほど連射できる
	ImGui::DragFloat("FireRate", &selectGunData.fireRate, 1.0f, 0.1f, 1000.0f);

	// リロード時間の調整
	ImGui::DragFloat("ReloadTime", &selectGunData.reloadTime, 0.5f, 0.0f, 100.0f);

	//_ 拡散量の設定 _//

	float spreadDeg = selectGunData.spread * RadToDeg;	// 拡散率(度)
	
	// 拡散量の調整　ショットガンなどの場合、数値が大きいほど拡散する　ラジアン値
	ImGui::DragFloat("Spread", &spreadDeg, 1.0f, 0.0f, 360.0f);
	selectGunData.spread = spreadDeg * DegToRad; // 拡散率(ラジアン)

	// パレット数の調整　ショットガンなどの場合複数同時発射なので、この数を増やす
	ImGui::DragInt("Palette", &selectGunData.palette, 1.0f, 1, 100);

	//_ 発射時の銃口のエフェクトの種類の設定 _//

	selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::FIRE_EFFECT_KIND] = EffekseerObjectManager::GetToString(selectGunData.fireEffectKind);	// 選択した文字列

	if (ImGui::CollapsingHeader("FireEffectKindInfo"))
	{
		std::unordered_set<std::string> effectStrCon;	// EFFEKSEER_KINDの文字列コンテナ

		for (const auto& effekseerKindStr : EffekseerObjectManager::EFFEKSEER_KIND_STRING_LIST)
		{
			// EFFEKSEER_KINDの文字列をpush
			effectStrCon.emplace(effekseerKindStr.second);
		}
		DrawImGuiCombo("EFFEKSEER_KIND_LIST", effectStrCon, selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::FIRE_EFFECT_KIND], isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::FIRE_EFFECT_KIND]);

		if (isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::FIRE_EFFECT_KIND])
		{
			// 選択されたEFFEKSEER_KINDへ変更
			selectGunData.fireEffectKind = EffekseerObjectManager::GetFromEnum(selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::FIRE_EFFECT_KIND]);
		}
	}

	//_ 発射音のサウンドIDコンテナの設定	複数要素の場合はランダムでそれぞれの音を再生 _//

	if (ImGui::CollapsingHeader("ShotSoundIDListInfo"))
	{
		// サウンドIDを設定
		SettingSoundIDUpdate("ShotSoundIDList", selectGunData.shotSoundIDList, GUN_DATA_SETTING_MEMBER_KIND::SHOT_SOUND_ID_LIST);
		/*
		std::unordered_set<std::string> soundIDStrCon;							// SOUND_IDの文字列コンテナ
		std::unordered_map<std::string, std::string> soundIDStrDrawListCon;		// SOUND_IDの文字列をリスト表示するコンテナ
		std::string& selectShotSoundIDStr = selectString[(int)GUN_DATA_SETTING_MEMBER_KIND::SHOT_SOUND_ID_LIST];

		for (const auto& soundIDStr : Sound_ID::SOUND_ID_STRING_LIST)
		{
			// SOUND_IDの文字列をpush
			soundIDStrCon.emplace(soundIDStr.second);
		}

		for (const auto& soundID : selectGunData.shotSoundIDList)
		{
			// SOUND_IDの文字列をkeyとして初期化
			soundIDStrDrawListCon[Sound_ID::GetToString(soundID)];
		}

		// 現在設定されているSOUND_IDの文字列をリスト表示
		DrawImGuiTextList("ShotSoundIDList", soundIDStrDrawListCon);
		// SOUND_IDの文字列の選択
		DrawImGuiCombo("SOUND_ID_LIST", soundIDStrCon, selectShotSoundIDStr, isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::SHOT_SOUND_ID_LIST]);

		if (isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::SHOT_SOUND_ID_LIST])
		{
			bool isPushButton = false;

			if (ImGui::Button("Add"))
			{
				// 選択したSOUND_IDをpush
				selectGunData.shotSoundIDList.emplace(Sound_ID::GetFromEnum(selectShotSoundIDStr));

				isPushButton = true;
			}

			if (ImGui::Button("Delete"))
			{
				auto soundIDItr = selectGunData.shotSoundIDList.find(Sound_ID::GetFromEnum(selectShotSoundIDStr));

				// 選択したSOUND_IDを削除
				soundIDItr = selectGunData.shotSoundIDList.erase(soundIDItr);

				isPushButton = true;
			}

			if (isPushButton)
			{
				// 文字列の表示を無にする
				selectShotSoundIDStr = "";
				// 選択したフラグをリセット
				isSelect[(int)GUN_DATA_SETTING_MEMBER_KIND::SHOT_SOUND_ID_LIST] = false;
			}
		}
		*/
	}
}

void GunManager::AttachGunData()
{
	gunDataAttachList = gunDataSetList;

	for (auto& gunList : gunList)
	{
		for (auto& gun : gunList.second)
		{
			std::string gunIDName = gun->GetGunIDName();

			HasUnorderedMapContainerKey(gunDataAttachList, gunIDName, true);

			// GunクラスのGunDataの再設定
			gun->SetGunData(gunDataAttachList[gunIDName]);
			// Gunクラスが現在保持している弾数を再設定　(GunDataだけかえても、GunクラスのholdBullet変数は変わらないので適用)
			gun->SetHoldBullet(gunDataAttachList[gunIDName].holdBullet);
			// Gunクラスの現在のマガジン内の弾数を再設定　(GunDataだけかえても、Gunクラスのammo変数は変わらないので適用)
			gun->SetAmmo(gunDataAttachList[gunIDName].ammo);
		}
	}
}

void GunManager::ResetGunData()
{
	// gunDataSetListを元データに戻す
	gunDataSetList = gunDataRawList;
}

void GunManager::SaveGunData()
{
	for (auto& gunDataSet : gunDataSetList)
	{
		
		if (gunList[gunDataSet.first].empty())
			continue;	// コンテナ内が空なので、continue
		if (!HasUnorderedMapContainerKey(gunList, gunDataSet.first))
			continue;	// キーが存在しないので、contiune

		Gun* gun = *gunList[gunDataSet.first].begin();

		// GunDataの書き出し
		GunDataSerializer::SaveGunData(gun->GetLoadGunDataFilePath(), gunDataSet.second);
	}
}

void GunManager::Draw()
{
}