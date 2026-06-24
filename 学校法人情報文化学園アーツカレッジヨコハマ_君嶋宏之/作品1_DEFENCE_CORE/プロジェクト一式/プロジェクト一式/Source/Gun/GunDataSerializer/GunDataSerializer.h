#pragma once
#include "../GunInfo.h"

class GunDataSerializer
{
public:
	
	/// <summary>
	///				GunDataが存在するフォルダパスを返す
	/// </summary>
	/// <returns>	GunDataが存在するフォルダパス		</returns>
	static const std::string GetGunDataDirectoryPath();

	/// <summary>
	///								GunDataをロードしてデータを返す
	/// </summary>
	/// <param name="_saveFileNum">	ロードするファイルパス	</param>
	/// <returns>					GunData					</returns>
	static GunInfo::GunData LoadGunData(std::string_view _filepath);

	/// <summary>
	///								gunsのGunDataをセーブする
	/// </summary>
	/// <param name="_saveFileNum">	セーブするファイルパス	</param>
	/// <param name="_gunData">		セーブするGunData			</param>
	static void SaveGunData(std::string_view _filepath, const GunInfo::GunData& _gunData);

private:


};
