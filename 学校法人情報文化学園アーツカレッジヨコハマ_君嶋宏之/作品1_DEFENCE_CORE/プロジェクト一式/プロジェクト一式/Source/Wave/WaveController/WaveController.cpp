#include "WaveController.h"
#include "../../Stage/StageManager/StageManager.h"
#include "../../Enemy/EnemyManager/EnemyManager.h"
#include "../../Game/GameController.h"
#include "../../Common/CsvReader/CsvReader.h"
#include "../../Screen.h"
#include "../../Player/Player.h"
#include "../../Camera/Camera.h"
#include "../../Common/Debug/Debug.h"
#include "../../Common/Function/function.h"
#include "../../Common/FileSystemUtils/FileSystemUtils.h"
#include "../../Common/UI/UIDrawManager/UIDrawManager.h"
#include "../../Wave/WaveDataSerializer/WaveDataSerializer.h"
#include "../../../ImGui/imgui.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace
{
	//const int WAVE_MAX_NUM = 5;				// ウェーブの最大数
	constexpr float ENEMY_SUMMON_COUNT		= 2.0f;	// 敵の召喚間隔
	constexpr int MAX_PLAYER_SLIME_BULLET	= 10;	// プレイヤーのスライム弾の最大数

	constexpr int SPAWN_POSITION_ROUND_NUMBER = 100;	// 召喚座標の丸めるレベルの数値
}

WaveController::WaveController()
{
	createEnemyContainer.clear();
	summonPositionContainer.clear();

	waveState		= WAVE_STATE::STAY;

	stageNumber		= 1;
	waveNumber		= 1;
	waveMax			= 1;
	isWaveAllWin	= false;

	summonCountdown = 0;

	waveUIImage[W_WAVE]		= LoadGraph("data/texture/letter/wave.png");
	waveUIImage[W_FINAL]	= LoadGraph("data/texture/letter/wave_final.png");
	waveUIImage[W_NUMBER]	= LoadGraph("data/texture/letter/num_red.png");

	// UIDraw関数を登録
	uiFunctionIDNumber = UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this] {UIDraw();});
	// 描画順を後に回す
	UIDrawManager::GetUIDrawManagerInstance()->SetDrawOrder(uiFunctionIDNumber, -1);
}

WaveController::~WaveController()
{
	createEnemyContainer.clear();
	summonPositionContainer.clear();
	saveWaveDataList.clear();

	for (int i = 0;i < W_MAX;i++)
	{
		DeleteGraph(waveUIImage[i]);
	}

	// UIDraw関数を解放
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(uiFunctionIDNumber);
}

void WaveController::Init()
{
	stageManager	= FindGameObject<StageManager>();
	enemyManager	= FindGameObject<EnemyManager>();
	gameController	= FindGameObject<GameController>();
	player			= FindGameObject<Player>();
	camera			= FindGameObject<Camera>();

	assert
	(
		stageManager != nullptr || 
		enemyManager != nullptr || 
		player != nullptr || 
		camera != nullptr
	);
	
	// プレイヤーのスライム弾を最大までリロード
	player->PushSlimeBulletMax();

	if (stageManager != nullptr)
		stageNumber = stageManager->GetLoadStageNum();

	// ウェーブ情報の設定
	SetWaveInfo();

	// ウェーブ情報を設定するための初期化
	int loadSuccessCount	= 0;	// ロードが成功した回数
	int loadWaveNum			= 0;	// ロードするウェーブ数
	int loopCount			= 0;	// ループ回数をカウント

	// 最大ファイル数に対してロードしたファイルが少ない時かつ、ループ回数が100未満のとき	// 100未満での範囲でとめることで、無限ループを防ぐ
	while (loadSuccessCount < waveMax && loopCount++ < 100)
	{
		// ロードするウェーブ数を進める
		loadWaveNum++;

		std::unordered_multimap<int, VECTOR3I> loadWave = std::move(GetWaveData(stageNumber, loadWaveNum));

		if (loadWave.empty())
			continue;
		// ウェーブのロードが成功した回数を進める
		loadSuccessCount++;

		for (const auto& wave : loadWave)
		{
			// 現在csvに保存されているウェーブデータをpush
			saveWaveDataList[loadWaveNum].emplace_back(WaveEnemyInfo::WaveEnemyData(wave.first, wave.second));
		}
	}
}

void WaveController::Update()
{
	// デバッグモードがオンの時
	if (Debug::GetDebugMode())
	{
		// ウェーブ情報の設定
		WaveSettingUpdate();
	}

	switch (waveState)
	{
	case WaveController::WAVE_STATE::STAY:
		
		// 敵のルートを描画
		enemyManager->SetDrawNextSummonEnemyRoot(true);
		break;
	case WaveController::WAVE_STATE::SUMMON:

		// 召喚の更新
		SummonUpdate();
		break;
	case WaveController::WAVE_STATE::FINISH:

		// 敵がすべて倒されたら
		if (!enemyManager->IsEnemySummon())
		{
			//_ すべてのウェーブを完了していたら _//

			if (waveNumber >= waveMax)
			{
				waveState = WAVE_STATE::END;	
				break;	// ウェーブの完全終了状態にする
			}

			//_ 次のウェーブへ _//

			waveNumber++;					// 次のウェーブに進める
			waveState = WAVE_STATE::STAY;	// 待機状態にする

			// ウェーブ情報の設定
			SetWaveInfo();

			// プレイヤーのスライム弾を最大までリロード
			player->PushSlimeBulletMax();
		}
		break;
	case WaveController::WAVE_STATE::END:

		isWaveAllWin = true;
		break;
	default:
		assert(false);
		break;
	}
}

int WaveController::GetWaveFileMax(const int& _stageNum)
{
	char setPath[CHAR_MAX];
	// ディレクトリパスを代入する
	sprintfDx(setPath, "data/stage/stageData/stage_%.2d/wave", _stageNum);
	
	return FileSystemUtils::GetFileCount(setPath);
}

void WaveController::RoundToNearestNumber(int& _number, const int& _roundToNearest)
{
	int addNum = _roundToNearest - 1;	// _numberに対して加算する値

	if (_number < 0)
	{
		// 符号を逆にする
		addNum *= -1;
	}
	// 丸めた値を_numberへ設定
	_number = (int)(((_number + addNum) / _roundToNearest) * _roundToNearest);
}

void WaveController::SetWaveInfo()
{
	// ウェーブの最大数の代入
	waveMax = GetWaveFileMax(stageNumber);

	// ウェーブ情報をロード
	LoadWave(stageNumber, waveNumber);

	// ロードしたウェーブ情報を次のウェーブで召喚する敵として設定
	enemyManager->SetNextSummonEnemyInfo(createEnemyContainer);
}

void WaveController::SummonUpdate()
{
	// カウントダウンが0以下だったら
	if ((summonCountdown -= Time::GameDeltaTime()) <= 0.0f)
	{
		if (createEnemyContainer.empty())
			assert(false && "ウェーブ情報が設定されていません");

		auto itr	= createEnemyContainer.begin();	// コンテナの先頭要素

		// エネミーマネージャーに敵の生成をリクエスト
		enemyManager->RequestSummonEnemy(VECTOR3(itr->second), (ENEMY_KIND)itr->first);
		// リクエストをしたので、コンテナから削除
		itr			= createEnemyContainer.erase(itr);

		// カウントダウンの設定
		summonCountdown = ENEMY_SUMMON_COUNT;
	}

	// すべての敵の生成が終わったら
	if (createEnemyContainer.empty())
	{
		waveState		= WAVE_STATE::FINISH;	// ウェーブ状態を終了にする
		summonCountdown = 0.0f;					// カウントダウンのリセット
	}
}

void WaveController::WaveSettingUpdate()
{
	//_ ウェーブ情報を保存する _//

	ImGui::Begin("WaveInfo");
	ImGui::SliderInt("waveNum", &saveWaveNum, 1, 10);
	ImGui::Separator();

	ImGui::Text("[ Wave %d ]", saveWaveNum);

	ImGui::SliderInt("enemyKind", &saveWaveData.enemyKind, (int)ENEMY_KIND::WALKER, (int)ENEMY_KIND::MAX - 1);
	ImGui::SliderInt3("summonPos", &saveWaveData.summonPosition.x, -10000, 10000);

	// XZ座標の値を丸める //
	RoundToNearestNumber(saveWaveData.summonPosition.x, SPAWN_POSITION_ROUND_NUMBER);
	RoundToNearestNumber(saveWaveData.summonPosition.z, SPAWN_POSITION_ROUND_NUMBER);

	if (ImGui::Button("cameraPosToSummonPos"))
	{
		// 出現座標を現在のカメラ座標として設定
		saveWaveData.summonPosition = camera->GetPosition();
	}

	// ToDo:地面に接したY座標に調整
	//if(ImGui::Button())

	if (ImGui::Button("AddSave"))
	{
		// 今設定された敵一体のウェーブ情報をpush
		saveWaveDataList[saveWaveNum].emplace_back(saveWaveData);
		// 今設定された敵一体のウェーブ情報を保存	csvデータの末尾に追加される
		//SaveWave(std::unordered_map<int, std::list<WaveEnemyInfo::WaveEnemyData>>{{saveWaveNum, { saveWaveData }}}, stageNumber);

		// 全ての情報を上書きする　ToDo:JsonSerializerの末尾追加が実装できたら、引数をfalseにして末尾追加する
		SaveWaveToJson(saveWaveDataList, true);
	}

	// AddSaveとBackDeleteを横に羅列
	ImGui::SameLine();

	if (ImGui::Button("BackDelete"))
	{
		// saveWaveDataListにキーが存在したら
		if (HasUnorderedMapContainerKey(saveWaveDataList, saveWaveNum))
		{
			if (!saveWaveDataList[saveWaveNum].empty())
			{
				// 末尾の敵一体のウェーブ情報を削除
				saveWaveDataList[saveWaveNum].pop_back();

				// 全ての情報を上書きする	// ToDo:削除するたびに全てのデータの再保存(上書き保存)するのは、効率が悪いので、一行データの削除に変更
				SaveWaveToJson(saveWaveDataList, true);
			}
		}
	}

	// AddとBackDeleteとAllDeleteを横に羅列
	ImGui::SameLine();

	if (ImGui::Button("AllDelete"))
	{
		// saveWaveDataListにキーが存在したら
		if (HasUnorderedMapContainerKey(saveWaveDataList, saveWaveNum))
		{
			// saveWaveNumと一致する全てのデータの削除
			saveWaveDataList[saveWaveNum].clear();

			// 全ての情報を上書きする
			SaveWaveToJson(saveWaveDataList, true);
		}
	}
	
	//if (ImGui::Button("OverrideSave"))
	//{
	//	if (!saveWaveDataList.empty())
	//	{
	//		// 全てのデータを上書き保存する
	//		SaveWave(saveWaveDataList, stageNumber, true);
	//	}
	//}
	ImGui::End();

	//_ 保存したウェーブデータの描画 _//

	ImGui::Begin("SaveWaveDataText");
	// saveWaveDataListにキーが存在したら
	if (HasUnorderedMapContainerKey(saveWaveDataList, saveWaveNum))
	{
		for (auto& waveData : saveWaveDataList[saveWaveNum])
		{
			// データをstringとして代入 // 上杉大好きです。付き合ってください?(??＞?＜?)。??
			std::string data 
				= "pos: "
				+ std::to_string(waveData.summonPosition.x) + ","
				+ std::to_string(waveData.summonPosition.y) + ","
				+ std::to_string(waveData.summonPosition.z) + " "
				+ "kind: "
				+ std::to_string((int)waveData.enemyKind);

			// ウェーブデータのテキスト出力
			ImGui::TextUnformatted(data.c_str());
		}
		// ログの一番下へスクロール　常に最新のログを見るため
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}
	ImGui::End();
}

void WaveController::WaveStart()
{
	if (enemyManager->IsEnemySummon() || waveState == WAVE_STATE::SUMMON)
		assert(false && "まだ敵が存在しています または 召喚中です");

	waveState = WAVE_STATE::SUMMON;
	// 敵のルートを描画しない
	enemyManager->SetDrawNextSummonEnemyRoot(false);
}

/*
void WaveController::SaveWave(const std::unordered_map<int, std::list<WaveEnemyInfo::WaveEnemyData>>& _saveCon,const int& _stageNum,bool _override)
{
	char set[CHAR_MAX];
	std::string filePath;

	std::list<std::string> data;	// ファイルに書き出す情報を行ごとにまとめる string出ないと出力できない

	std::ofstream file;
	int lastSaveWaveNum		= -1;	// 一つ前に書き出したウェーブのウェーブナンバー
	bool isFirstOpenFile	= true;	// ファイルを初めてオープンしたかどうか

	//__ ウェーブ情報の書き出し __//

	for (const auto& saveWave : _saveCon)
	{
		if (saveWave.first < 0)
			assert(false && "不正なウェーブナンバーです");

		sprintfDx(set, "data/stage/stageData/stage_%.2d/wave/wave_%d.csv", _stageNum, saveWave.first);
		filePath = set;

		// 上書き保存の時
		if (_override)
		{
			// ファイルに保存するデータが空の時
			if (saveWave.second.empty())
			{
				// ファイル内のデータを空として上書きするので、ファイルの削除をする
				DeleteWaveFile(filePath);
				continue;	// ファイルを削除したのでcontinue
			}

			// これでパスの通りにファイルを作成 or 開く // std::ios::truncは完全にデータを一から構築	※file.open()のデフォルトはstd::ios::trunc
			file.open(filePath, std::ios::trunc);
		}
		// 通常保存のとき
		else
		{
			// これでパスの通りにファイルを作成 or 開く // std::ios::appはデータの末尾に追加データを追加していくことを指定
			file.open(filePath, std::ios::app);
		}

		if (!file)
			assert(false && "ファイルを開けませんでした");

		for (const auto& waveData : saveWave.second)
		{
			// 召喚座標
			data.emplace_back(std::to_string(waveData.summonPosition.x));
			data.emplace_back(std::to_string(waveData.summonPosition.y));
			data.emplace_back(std::to_string(waveData.summonPosition.z));

			// 召喚する敵の種類
			data.emplace_back(std::to_string(waveData.enemyKind));

			// データをすべて回す
			for (auto dItr = data.begin();dItr != data.end();)
			{
				// データの書き出し
				file << *dItr;

				// 列の最後でなかったら
				if (++dItr != data.end())
				{
					// 切り分け（列）
					file << ',';
				}
			}
			// 改行（行）
			file << "\n";
			// データリセット
			data.clear();
		}
		// ファイルを閉じる
		file.close();		
	}

	// ウェーブ情報が変更されたので、ウェーブ情報の再設定をする
	SetWaveInfo();
}
*/

void WaveController::SaveWaveToJson(const std::unordered_map<int, std::list<WaveEnemyInfo::WaveEnemyData>>& _saveCon, bool _override)
{
	for (auto& waveData : _saveCon)
	{
		// 上書き保存の時
		if (_override)
		{
			// ファイルに保存するデータが空の時
			if (waveData.second.empty())
			{
				// ファイル内のデータを空として上書きするので、ファイルの削除をする
				WaveDataSerializer::DeleteWaveDataFile(waveData.first);
				continue;	// ファイルを削除したのでcontinue
			}
		}
		
		// Jsonにウェーブ情報を書き出し
		WaveDataSerializer::Save(waveData.first, WaveInfo::WaveData(waveData.second), _override);
	}

	// ウェーブ情報が変更されたので、ウェーブ情報の再設定をする
	SetWaveInfo();
}

std::unordered_multimap<int, VECTOR3I> WaveController::GetWaveData(const int& _stageNum, const int& _waveNum)
{
	std::unordered_multimap<int, VECTOR3I> waveDataContainer;	// 読み込んだデータを返すために、データを一時的に保持するコンテナ

#if 0
	char set[CHAR_MAX];
	sprintfDx(set, "data/stage/stageData/stage_%.2d/wave/wave_%d.csv", _stageNum, _waveNum);
	std::string file = set;

	std::ifstream dataFile(file);

	// ファイルパスが存在しないとき
	if (!dataFile)
		return std::unordered_multimap<int, VECTOR3I>{};

	CsvReader* csv = new CsvReader(file);

	

	int kind;		// 敵の種類
	VECTOR3I pos;	// 敵の生成座標

	for (int n = 0;n < csv->GetLines();n++)
	{
		for (int c = 0;c < csv->GetColumns(n);c++)
		{
			if (c / 3 == 0)		// 1 - 3 列目
			{
				// 座標の初期化
				SetVMem(pos, c % 3, csv->GetInt(n, c));
			}
			else				// 4 列目
			{
				// 種類の初期化
				kind = csv->GetInt(n, c);
			}
		}
		// ウェーブごとの敵の生成情報をpush
		returnContainer.insert(std::make_pair(kind, pos));
	}
	delete csv;
	return returnContainer;
#else
	WaveInfo::WaveData waveData = WaveDataSerializer::Load(_waveNum);

	for (const auto& wave : waveData.waveDataList)
	{
		// ウェーブごとの敵の生成情報をpush
		waveDataContainer.insert(std::make_pair(wave.enemyKind, wave.summonPosition));
	}

	return waveDataContainer;
#endif
}

bool WaveController::LoadWave(const int& _stageNum, const int& _waveNum)
{
	// コンテナのウェーブ情報をリセット //
	createEnemyContainer.clear();
	summonPositionContainer.clear();

	// 全てのスポナーの削除
	stageManager->DeleteAllSpawner();

	std::unordered_multimap<int, VECTOR3I> loadWaveCon = std::move(GetWaveData(_stageNum, _waveNum));

	if (loadWaveCon.empty())
		return false;	// ウェーブのデータが存在しなかったら

	for (auto& wave : loadWaveCon)
	{
		VECTOR3 summonPosition = VECTOR3(wave.second);	// 召喚座標

		// 地面との当たり判定をして埋もれないようにする
		stageManager->CheckRaycastStageObject(wave.second + VECTOR3(0, 200, 0), wave.second + VECTOR3(0, -1000, 0), std::set<int>{(int)StageObjectData::STAGE_OBJECT_KIND::GROUND_BLOCK}, &summonPosition);

		// 召喚する敵の情報をpush
		createEnemyContainer.emplace(wave.first, VECTOR3I(summonPosition));
		// スポナーの設置と座標のpush
		summonPositionContainer.emplace(stageManager->PutSpawner(summonPosition), VECTOR3I(summonPosition));
	}
	return true;
}

bool WaveController::DeleteWaveFile(const std::string _filePath)
{
	// ファイルの削除　削除できなかった場合は、falseを返す
	return std::filesystem::remove_all(_filePath);
}

void WaveController::Draw()
{
	// 描画フラグの設定
	UIDrawManager::GetUIDrawManagerInstance()->SetDrawFlag(uiFunctionIDNumber, gameController->InGame());

	if (!gameController->InGame())
		return;	// ゲーム中でなかったら

	//_ 敵の出現座標の描画 _//

	/*for (const auto& spawnPosition : summonPositionContainer)
	{
		VECTOR3 radius		= VOne * 50.0f;
		VECTOR3 position	= VGet((float)spawnPosition.x, (float)spawnPosition.y, (float)spawnPosition.z);

		DrawCube3D(position - radius, position + radius, 0x990000, 0xffffff, true);
	}*/

	//_ ウェーブ情報を設定するときの描画 _//

#ifdef _DEBUG

	int sphereColor;

	switch ((ENEMY_KIND)saveWaveData.enemyKind)
	{
	case ENEMY_KIND::WALKER:
		sphereColor = 0xffffff;
		break;
	case ENEMY_KIND::CLIMBER:
		sphereColor = 0xff0000;
		break;
	case ENEMY_KIND::RUSHER:
		sphereColor = 0x00eeee;
		break;
	case ENEMY_KIND::CRUSHER:
		sphereColor = 0x555555;
		break;
	default:
		assert(false);
		break;
	}
	// 現在設定しようとしている敵の出現座標の描画
	DrawSphere3D(VECTOR3(saveWaveData.summonPosition), 50.0f, 8, sphereColor, 0xffffff, false);
#endif
}

void WaveController::UIDraw()
{
	int drawWaveInfoY = 70;	// ウェーブ情報描画のY座標

	switch (waveState)
	{
	case WaveController::WAVE_STATE::STAY:
	case WaveController::WAVE_STATE::SUMMON:
	case WaveController::WAVE_STATE::FINISH:
	case WaveController::WAVE_STATE::END:

		if (waveNumber < waveMax)
		{
			// WAVE 文字の描画
			DrawRotaGraph(Screen::WIDTH_CENTER - 70, drawWaveInfoY, 1.0f, 0.0f, waveUIImage[W_WAVE], true);

			float waveNumScale		= 1.0f;							// ウェーブナンバーのスケール
			VECTOR2I waveNumSize	= VECTOR2I(794 / 10, 159 / 2);	// ウェーブナンバー一桁のサイズ

			// ウェーブナンバーの描画
			DrawRotaNum((float)(Screen::WIDTH_CENTER + 70), (float)drawWaveInfoY, waveNumber, waveNumSize.x, waveUIImage[W_NUMBER], waveNumSize.x, waveNumSize.y, 0, waveNumScale, 0.0f, 1);

			// / 文字の描画
			DrawRectRotaGraph(Screen::WIDTH_CENTER + 120, drawWaveInfoY, 0, waveNumSize.y, waveNumSize.x, waveNumSize.y, waveNumScale, 0.0f, waveUIImage[W_NUMBER], true);

			// 最大ウェーブ数の描画
			DrawRotaNum((float)(Screen::WIDTH_CENTER + 160), (float)drawWaveInfoY, waveMax, waveNumSize.x, waveUIImage[W_NUMBER], waveNumSize.x, waveNumSize.y, 0, waveNumScale, 0.0f, 1);
		}
		else
		{
			// WAVE_FINAL 文字の描画
			DrawRotaGraph(Screen::WIDTH_CENTER, drawWaveInfoY, 1.0f, 0.0f, waveUIImage[W_FINAL], true);
		}
		break;
	default:
		assert(false);
		break;
	}
}

