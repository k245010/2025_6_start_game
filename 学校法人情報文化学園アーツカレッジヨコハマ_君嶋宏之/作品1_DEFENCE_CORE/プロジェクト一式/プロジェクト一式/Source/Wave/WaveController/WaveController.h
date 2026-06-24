#pragma once
#include "../../../Library/GameObject.h"
#include "../WaveInfo.h"
#include <unordered_map>
#include <unordered_set>
#include <list>

class StageManager;
class StageObjectBase;
class EnemyManager;
class GameController;
class Player;
class Camera;
class WaveDataSerializer;
/// <summary>
/// 敵が攻めてくるウェーブを管理する
/// </summary>
class WaveController : public GameObject
{
	friend WaveDataSerializer;

public:

	WaveController();
	~WaveController();

	void Init();

	void Update()	override;
	void Draw()		override;

	/// <summary>
	/// ウェーブの状態
	/// </summary>
	enum class WAVE_STATE
	{
		STAY	= 0,	// 待機
		SUMMON,			// 敵の召喚
		FINISH,			// 召喚終了
		END,			// ウェーブの完全終了
	};

	/// <summary>
	///				ウェーブが終了したか返す
	/// </summary>
	/// <returns>	true:ウェーブが終了している / false:ウェーブが終了していない	</returns>
	bool IsWaveEnd() { return waveState == WAVE_STATE::STAY; }

	/// <summary>
	/// ウェーブを開始する
	/// </summary>
	void WaveStart();

	/// <summary>
	///				ウェーブに完全勝利したか返す
	/// </summary>
	/// <returns>	true:勝利した / false:勝利していない </returns>
	bool IsWaveAllWin() { return isWaveAllWin; }

private:

	/// <summary>
	/// UIの描画
	/// </summary>
	void UIDraw();

	/// <summary>
	/// 書き出しするウェーブデータ
	/// </summary>
	//struct SaveWaveData
	//{
	//	int enemyKind;				// 書き出し用の敵種類
	//	VECTOR3I summonPosition;	// 書き出し用の敵の召喚座標

	//	SaveWaveData()										: enemyKind(0),summonPosition(VZero)	{}
	//	SaveWaveData(const int& _kind,const VECTOR3I& _pos)	: enemyKind(_kind),summonPosition(_pos) {}
	//};

	/// <summary>
	/// ウェーブ情報を設定
	/// </summary>
	void SetWaveInfo();

	/// <summary>
	///	敵の召喚をエネミーマネージャーにリクエストする
	/// </summary>
	void SummonUpdate();

	/// <summary>
	/// ウェーブの設定をする
	/// </summary>
	void WaveSettingUpdate();

	/// <summary>
	///									ウェーブ情報の書き出し
	/// </summary>
	/// <param name="_saveCon">			保存するデータコンテナ			</param>
	/// <param name="_stageNum">		ステージナンバー				</param>
	/// <param name="_override">		上書き保存するかどうか			</param>
	//void SaveWave(const std::unordered_map<int, std::list<WaveEnemyInfo::WaveEnemyData>>& _saveCon, const int& _stageNum, bool _override = false);

	/// <summary>
	///									ウェーブ情報の書き出し
	/// </summary>
	/// <param name="_saveCon">			保存するデータコンテナ			</param>
	/// <param name="_override">		上書き保存するかどうか			</param>
	void SaveWaveToJson(const std::unordered_map<int, std::list<WaveEnemyInfo::WaveEnemyData>>& _saveCon, bool _override = false);

	/// <summary>
	///								ウェーブ情報を読み込み、コンテナに初期化する
	/// </summary>
	/// <param name="_stageNum">	ステージナンバー		</param>
	/// <param name="_waveNum">		現在のウェーブナンバー	</param>
	/// <returns>					成功：失敗				</returns>
	bool LoadWave(const int& _stageNum,const int& _waveNum);

	/// <summary>
	///								ウェーブ情報を読み込み、コンテナで返す
	/// </summary>
	/// <param name="_stageNum">	ステージナンバー														</param>
	/// <param name="_waveNum">		現在のウェーブナンバー													</param>
	/// <returns>					const std::unordered_multimap<int,VECTOR3I>&  ウェーブデータコンテナ	</returns>
	std::unordered_multimap<int, VECTOR3I> GetWaveData(const int& _stageNum, const int& _waveNum);

	/// <summary>
	///								ウェーブを削除する
	/// </summary>
	/// <param name="_filePath">	削除するファイルパス	</param>
	/// <returns>					true:成功 / false:失敗	</returns>
	bool DeleteWaveFile(const std::string _filePath);

	/// <summary>
	///								ウェーブファイルの最大数を返す
	/// </summary>
	/// <param name="_stageNum">	ステージナンバー </param>
	/// <returns>					ウェーブファイルの最大数 </returns>
	int GetWaveFileMax(const int& _stageNum);

	/// <summary>
	///									第一引数の数値を第二引数の数値へ丸める
	/// </summary>
	/// <param name="_number">			丸める数値				</param>
	/// <param name="_roundToNearest">	丸めるレベル数値単位	</param>
	void RoundToNearestNumber(int& _number, const int& _roundToNearest);

	StageManager* stageManager			= nullptr;
	EnemyManager* enemyManager			= nullptr;
	GameController* gameController		= nullptr;
	Player* player						= nullptr;
	Camera* camera						= nullptr;

	int stageNumber;		// ステージナンバー
	int waveNumber;			// ウェーブナンバー
	int waveMax;			// 最大ウェーブ数
	bool isWaveAllWin;		// ウェーブに完全勝利したかどうか

	int uiFunctionIDNumber = -1;	// UIDraw関数に対するIDナンバー

	WAVE_STATE waveState;	// ウェーブの状態
	float summonCountdown;	// 敵の生成カウントダウン
	std::unordered_multimap<int,VECTOR3I> createEnemyContainer;							// ウェーブごとの敵を管理するコンテナ
	std::unordered_map<const StageObjectBase*, VECTOR3I> summonPositionContainer;		// 敵の召喚座標から出現場所の描画を行うための、座標管理コンテナ key:スポナーのポインタ,召喚座標

	int saveWaveNum = 1;																// 書き出しするウェーブナンバー
	WaveEnemyInfo::WaveEnemyData saveWaveData;											// 書き出しするウェーブデータ
	std::unordered_map<int, std::list<WaveEnemyInfo::WaveEnemyData>> saveWaveDataList;	// 書き出しするウェーブデータのコンテナ	 key:ウェーブナンバー / value:ウェーブデータのコンテナ

	enum UI_KIND
	{
		W_WAVE,		// 通常ウェーブ
		W_FINAL,	// 最終ウェーブ
		W_NUMBER,	// ウェーブナンバー

		W_MAX,
	};

	int waveUIImage[W_MAX];
};
