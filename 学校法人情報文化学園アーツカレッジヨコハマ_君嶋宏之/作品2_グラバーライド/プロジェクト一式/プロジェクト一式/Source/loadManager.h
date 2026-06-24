#pragma once

#include <thread>
#include <atomic>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
#include "../Library/gameObject.h"
enum class LoadDataType{
	MODEL,
	EFFECT,
	TEXTURE,
	ANIME,
	SOUND,
};

/// <summary>
/// リソースデータを読み込むクラス
/// </summary>
class ConfigData;
class LoadManager :public GameObject {
public:
	LoadManager(SceneBase* _scene);
	~LoadManager();
	void Draw()override;
	void Update()override;

	
	//void Finalize();
	//bool IsLoadFinished()const { return loading; };
	/// <summary>
	/// ハンドルを読み込むのに必要情報を登録
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="key"></param>
	/// <param name="filePath"></param>
	/// <param name="loadPlace"></param>
	/// <param name="loadDataType"></param>
	/// <param name="...args"></param>

	bool AddLoadData(const std::string& key, const std::string& filePath, const std::string& loadPlace, const LoadDataType& loadDataType);
	bool AddLoadData(const std::string& key, const std::string& filePath, const std::string& loadPlace, const LoadDataType& loadDataType,float rate);
	bool AddLoadData(const std::string& key, const std::string& filePath, const std::string& loadPlace, const LoadDataType& loadDataType,int volume);
	/// <summary>
	/// AddLoadDataで追加した情報を使いハンドルを読み込む
	/// </summary>
	/// <param name="loadPlace"></param>
	/// <returns></returns>
	int LoadHandleData(const  std::string& loadPlace);

	//int LoadSingleHandleData(std::string key);

	/// <summary>
	/// 特定のハンドルを消去
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	bool RemoveLoadData(const std::string& key);
	/// <summary>
	/// 場所を指定してデータの削除
	/// </summary>
	/// <param name="loadPlace"></param>
	/// <returns></returns>
	int ClearLoadData(const  std::string& loadPlace);
	/// <summary>
	/// 読み込み済みデータを場所指定で削除する
	/// </summary>
	/// <param name="loadPlace"></param>
	/// <returns></returns>
	int ClearLoadMapData(const  std::string& loadPlace);

	/// <summary>
	/// ハンドル取得
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	int GetHandle(std::string key);


	/// <summary>
	/// ロードのタイプに応じてhandleをvectorで返却
	/// </summary>
	/// <param name="loadDataType"></param>
	/// <returns></returns>
	std::vector<int>GetLoadTypeHandle(const LoadDataType& loadDataType);


	int SetAsync(bool flg);
	bool IsLoading();
	int NowloadNum();
	
	// 進捗率を返す
	float GetProgressRate() const;

	std::string GetNowScene() { return nowLoadPlace; }
private:
	ConfigData* configData;
		
	struct LoadData {
		std::string filePath;
		std::string loadPlace;
		LoadDataType loadDataType;
		std::string key;
		//effect用
		float rate;

		int volume;
	};
	struct HandleData {
		int handle;
		LoadDataType loadDataType;
	};
	
	std::list<LoadData> loadDataList;
	std::map<std::string, HandleData>handleMap;
	std::atomic<int> loadCount;         // 読み込み完了カウンタ
	std::atomic<int> totalCount;                       // 全ロード数

	std::string nowLoadPlace;
	std::string nextScene;
};

