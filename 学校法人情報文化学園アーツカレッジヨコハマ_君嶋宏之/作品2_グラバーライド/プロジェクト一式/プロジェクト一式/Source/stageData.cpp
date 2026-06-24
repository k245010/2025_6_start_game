#include "stageData.h"
#include "function.h"
#include "../Library/sceneManager.h"
#include "commonData.h"
#include "commonFind.h"
#include <cassert>


StageData::StageData(SceneBase* _scene):GameObject(_scene)
{
	//csvReader = _scene->CreateGameObject<CsvReader>();
}

StageData::~StageData()
{
	AllClear();
}

bool StageData::LoadStageObjectCsvData(int _stage)  
{  
   FILE* tFp = nullptr;  
   FILE* cFp = nullptr;  
   std::string path;  
   path += "data\\stage\\";  
   path += std::to_string(_stage) + "tera.csv";  
   errno_t err;  
   err = fopen_s(&tFp, path.c_str(), "rt");  
   if (err != 0 || tFp == nullptr) {  
#if _DEBUG
       printfDx("ロード失敗: terraファイルが開けません");  
	   assert(false && "terraファイル読み込みに失敗");  
#endif
     
       return false;  
   }  

   path.clear();  
   path += "data\\stage\\";  
   path += std::to_string(_stage) + "chara.csv";  
   err = fopen_s(&cFp, path.c_str(), "rt");  
   if (err != 0 || cFp == nullptr) {  
       //fclose(tFp); // tFpを閉じる  
#if _DEBUG
	   printfDx("ロード失敗: charaファイルが開けません");
	   assert(false && "charaファイル読み込みに失敗");
#endif // _DEBUG

     
	   return false;
   }  

   StageObjectCsvData ts{};  
   StageObjectCsvData cs{};

   AllClear();  

   for (int i = 0; i < 4; i++) {
	   ts.slot.emplace_back(0);
	   cs.slot.emplace_back(0);
   }
   while (fscanf_s(tFp, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d", &ts.type, &ts.pos.x, &ts.pos.y, &ts.pos.z, &ts.scale.x, &ts.scale.y, &ts.scale.z, &ts.rotation.x, &ts.rotation.y, &ts.rotation.z, &ts.collisionId, &ts.slot[0], &ts.slot[1], &ts.slot[2], &ts.slot[3]) != EOF) {

	   parrameta.terra.emplace_back(ts);
   }
   while (fscanf_s(cFp, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d", &cs.type, &cs.pos.x, &cs.pos.y, &cs.pos.z, &cs.scale.x, &cs.scale.y, &cs.scale.z, &cs.rotation.x, &cs.rotation.y, &cs.rotation.z, &cs.collisionId, &cs.slot[0], &cs.slot[1], &cs.slot[2]) != EOF) {

	   parrameta.chara.emplace_back(cs);
   }

   fclose(tFp);  
   fclose(cFp);  
#if _DEBUG
   printfDx("ロード成功"); 
#endif // _DEBUG
   return true;  
}

bool StageData::LoadStageData(int _stage)
{
	LoadStageConfigData(_stage);
	if (LoadStageObjectCsvData(_stage)) {
		return true;
	}
	return false;
}

void StageData::AllClear()
{
	parrameta.chara.clear();
	parrameta.terra.clear();
}

bool StageData::LoadStageConfigData(int _stage)
{

	FILE* fp = 0;
	std::string path;
	path += "data\\stage\\";
	path += std::to_string(_stage) + "Config.csv";
	errno_t err;
	err = fopen_s(&fp, path.c_str(), "rt");
	if (err != 0) {
#if _DEBUG
		/*assert(err == 0 && "ファイル読み込みに失敗");
		printfDx("セーブ失敗");*/
#endif // _DEBUG
		return false;//ファイルに問題があるので処理をやめる

	}
	
	fscanf_s(fp, "%d,%d,%f",&config.backGroundData, &config.bgmData, &config.time);

	fclose(fp);
	return true;
}






