#pragma once
#include "../WaveInfo.h"

class WaveController;
class WaveDataSerializer
{
	friend WaveController;

public:

	/// <summary>
	///							ウェーブデータを書き込み
	/// </summary>
	/// <param name="_waveData">ウェーブナンバー				</param>
	/// <param name="_waveData">ウェーブデータ					</param>
	/// <param name="_override">上書き保存するかどうか			</param>
	static void Save(const int& _waveNum, const WaveInfo::WaveData& _waveData, bool _override = false);

	/// <summary>
	///							ウェーブデータを読み込み
	/// </summary>
	/// <param name="_waveData">ウェーブナンバー			</param>
	/// <returns>				ウェーブデータコンテナ		</returns>
	static WaveInfo::WaveData Load(const int& _waveNum);

private:

	/// <summary>
	///							ウェーブのファイルパスを返す
	/// </summary>
	/// <param name="_waveNum">	ウェーブナンバー			</param>
	/// <returns>				ウェーブのファイルパス		</returns>
	static std::string GetWaveDataFilePath(const int& _waveNum);

	/// <summary>
	///								ウェーブを削除する
	/// </summary>
	/// <param name="_filePath">	ウェーブナンバー	</param>
	/// <returns>					true:成功 / false:失敗	</returns>
	static bool DeleteWaveDataFile(const int& _waveNum);
};
