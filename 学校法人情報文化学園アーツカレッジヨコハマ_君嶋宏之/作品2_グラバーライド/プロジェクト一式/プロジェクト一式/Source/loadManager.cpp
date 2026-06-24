#include "loadManager.h"
#include <mutex>
#include <EffekseerForDXLib.h>
#include "LoadData.h"
#include "stageObjectManager.h"
#include "fader.h"
#include "../Library/sceneManager.h"

#include "fader.h"
#include "pathReader.h"
#include "soundManager.h"
#include "configData.h"
#include "commonFind.h"
LoadManager::LoadManager(SceneBase* _scene):GameObject(_scene)
{
	
	configData= SceneManager::CommonScene()->FindGameObject<ConfigData>();
	PathReader* path = new PathReader();
	path->ReadPath("data/model");
	auto vf= path->GetFolder();
	std::string count;
	for (int i = 0; i < vf.size(); i++) {
		auto str = vf[i].substr(0,5);
		if (str == "model") {
			count = vf[i].substr(5, 2);
			auto path = str+count + ".mv1";
			int handle = MV1LoadModel(path.c_str());

			AddLoadData(count, path, "PlayScene", LoadDataType::MODEL);
		}
	}
	for (auto& s : ResoData::SOUND) {
		AddLoadData(s.data.key, s.data.path.c_str(), s.data.scene, LoadDataType::SOUND,s.volume);
	}

	for (auto& m : ResoData::MOTION) {
		AddLoadData(m.key, m.path.c_str(),m.scene, LoadDataType::ANIME);
	}
	for (auto& e : ResoData::EFFECT) {
		AddLoadData(e.data.key, e.data.path.c_str(),e.data.scene, LoadDataType::EFFECT,e.rate);
	}
	for (auto& t : ResoData::TEXTURE) {
		AddLoadData(t.key, t.path.c_str(),t.scene, LoadDataType::TEXTURE);
	}

	nowLoadPlace = "NO";
	LoadHandleData("common");
}

LoadManager::~LoadManager()
{
	
	//全部の追加したハンドルを削除
	for (auto itr = handleMap.begin(); itr != handleMap.end(); ++itr) {

		int handle = itr->second.handle;
		//auto it=std::find(loadDataList.begin(),loadDataList.end(),itr->first);
		switch (itr->second.loadDataType)
		{
		case LoadDataType::MODEL:
			MV1DeleteModel(handle);
			break;
		case LoadDataType::EFFECT:
			DeleteEffekseerEffect(handle);
			break;
		case LoadDataType::TEXTURE:
			DeleteGraph(handle);
			break;
		case LoadDataType::ANIME:
			MV1DeleteModel(handle);
			break;
		case LoadDataType::SOUND:
			DeleteSoundMem(handle);
			break;
		default:
			break;
		}

	}
	handleMap.clear();
	// 未ロードのデータリストもクリア
	loadDataList.clear();
}

void LoadManager::Draw()
{
	
}

void LoadManager::Update()
{
	
}





int LoadManager::LoadHandleData(const std::string& loadPlace)  
{  
   loadCount = 0;  
   nowLoadPlace = loadPlace;
  

       for (auto& data : loadDataList) {  
           if (data.loadPlace != loadPlace) continue;  

           int handle = -1;  
           switch (data.loadDataType)  
           {  
           case LoadDataType::MODEL:  
               handle = MV1LoadModel((ResoData::RESO_MO + data.filePath).c_str());  
               break;  
           case LoadDataType::TEXTURE:  
               handle = LoadGraph((ResoData::RESO_TE + data.filePath).c_str());  
               break;  
           case LoadDataType::ANIME:  
               handle = MV1LoadModel((ResoData::RESO_ANI + data.filePath).c_str());  
               break;  
           case LoadDataType::SOUND:  
               {  
                   handle = LoadSoundMem((ResoData::RESO_SO + data.filePath).c_str());  

                   auto cd = configData->GetConfigData();  
                   if (data.filePath.find("bgm") != std::string::npos) {  
                       ChangeVolumeSoundMem(cd.baseVolume + cd.bgmBaseVolume + data.volume, handle);  
                   } else {  
                       ChangeVolumeSoundMem(cd.baseVolume + cd.seBaseVolume + data.volume, handle);  
                   }  
               }  
               break;  
           case LoadDataType::EFFECT:  
               handle = LoadEffekseerEffect((ResoData::RESO_EFF + data.filePath).c_str(), data.rate);  
               break;  
           default:  
               break;  
           }  
           if (handle < 0)  
               CreateMessageBox(data.filePath, "ロード失敗");  

           handleMap.emplace(data.key, HandleData{ handle, data.loadDataType });  
           loadCount++;  
       }  
  

   return 0;  
}


bool LoadManager::RemoveLoadData(const std::string& key)
{
	// データリストから削除
	auto it = std::remove_if(loadDataList.begin(), loadDataList.end(),[&](const LoadData& d) { return d.key == key; });
	bool removed = (it != loadDataList.end());
	loadDataList.erase(it, loadDataList.end());

	// ロード済みハンドルがあれば解放＆マップから削除
	auto hit = handleMap.find(key);
	if (hit != handleMap.end()) {
		int handle = hit->second.handle;
		switch (hit->second.loadDataType) {
		case LoadDataType::MODEL:        MV1DeleteModel(handle); break;
		case LoadDataType::EFFECT:       DeleteEffekseerEffect(handle); break;
		case LoadDataType::TEXTURE:      DeleteGraph(handle); break;
		case LoadDataType::ANIME:        MV1DeleteModel(handle); break;
		case LoadDataType::SOUND:        StopSoundMem(handle); DeleteSoundMem(handle); break;
		default:                         break;
		}
		handleMap.erase(hit);
	}
	return removed;
}

int LoadManager::ClearLoadData(const  std::string& loadPlace)
{
	int count = 0;
	//std::vector<std::string> keys;
	for (const auto& d : loadDataList) {
		if (d.loadPlace == loadPlace) {
			if (RemoveLoadData(d.key)) {
				++count;
			}
		}
	}
	return count;
}

int LoadManager::ClearLoadMapData(const std::string& loadPlace)
{
	int count = 0;
	std::vector<std::string> keysToRemove;

	for (const auto& [key, handleData] : handleMap) {
		
		auto it = std::find_if(loadDataList.begin(), loadDataList.end(), [&](const LoadData& d) {			
			return d.key == key && d.loadPlace == loadPlace;
		});

		if (it != loadDataList.end()) {
			int handle = handleData.handle;
			switch (handleData.loadDataType) {
			case LoadDataType::MODEL:
			case LoadDataType::ANIME:
				MV1DeleteModel(handle);
				break;
			case LoadDataType::EFFECT:
				DeleteEffekseerEffect(handle);
				break;
			case LoadDataType::TEXTURE:
				DeleteGraph(handle);
				break;
			case LoadDataType::SOUND:
				StopSoundMem(handle);
				DeleteSoundMem(handle);
				break;
			default:
				break;
			}
			keysToRemove.push_back(key);
			++count;
		}
	}

	for (const auto& key : keysToRemove) {
		handleMap.erase(key);
	}
	loadCount = 0.0f;
	return count;
}

int LoadManager::GetHandle(std::string key)
{
	auto it = handleMap.find(key);//指定キーを取得
	if (handleMap.end() == it) {//無かったら
		return -1;
	}
	else {
		return it->second.handle;//あったら値を返す
	}
}

std::vector<int> LoadManager::GetLoadTypeHandle(const LoadDataType& loadDataType)
{
	std::vector<int>handle;
	for (const auto& h : handleMap) {
		
		if (h.second.loadDataType == loadDataType) {
			handle.emplace_back(h.second.handle);
		}
	}
	return handle;
}

int LoadManager::SetAsync(bool flg)
{
	return SetUseASyncLoadFlag(flg);
}

bool LoadManager::IsLoading()
{
	int load = GetASyncLoadNum();
	return (load>0);
}

int LoadManager::NowloadNum()
{
	return GetASyncLoadNum();
}

float LoadManager::GetProgressRate() const
{
	int total = 0;
	for (const auto& d : loadDataList)
	{
		if (d.loadPlace == nowLoadPlace) total++;
	}

	int remaining = GetASyncLoadNum();  // 読み込み中の数
	int loaded = total - remaining;

	if (total == 0) return 1.0f; // 読み込み対象が無いときは100%
	return static_cast<float>(loaded) / static_cast<float>(total);
}




bool LoadManager::AddLoadData(const std::string& key, const std::string& filePath, const std::string& loadPlace, const LoadDataType& loadDataType)
{

	LoadData loadData = { filePath,loadPlace,loadDataType, key };
	RemoveLoadData(key);
	loadDataList.emplace_back(loadData);
	return true;
}

bool LoadManager::AddLoadData(const std::string& key, const std::string& filePath, const std::string& loadPlace, const LoadDataType& loadDataType, float rate)
{
	LoadData loadData = { filePath,loadPlace,loadDataType, key,rate };
	RemoveLoadData(key);
	loadDataList.emplace_back(loadData);
	return true;
}

bool LoadManager::AddLoadData(const std::string& key, const std::string& filePath, const std::string& loadPlace, const LoadDataType& loadDataType, int volume)
{
	LoadData loadData = { filePath,loadPlace,loadDataType, key,0,volume };
	RemoveLoadData(key);
	loadDataList.emplace_back(loadData);
	return true;
}
