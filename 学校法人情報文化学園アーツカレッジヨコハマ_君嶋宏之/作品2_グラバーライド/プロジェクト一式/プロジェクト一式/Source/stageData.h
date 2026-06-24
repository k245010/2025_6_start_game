#pragma once  
#include "../Library/gameObject.h"  
#include <vector>  
#include "CsvReader.h"  


namespace stage {  
   static const int MAX_STAGE = 30;  
 
}  
/// <summary>
/// csvから読み取ったステージに配置するオブジェクトのデータ
/// </summary>
struct StageObjectCsvData {  
   int type;  
   VECTOR pos;  
   VECTOR scale;  
   VECTOR rotation;  
   int gDirection;  
   int collisionId;  

   std::vector<int> slot;  
};  
typedef std::vector<StageObjectCsvData> StageCsvDataV;
/// <summary>
/// ステージに関する構成データ
/// </summary>
struct StageConfig {
	StageConfig() { bgmData = 0; backGroundData = 0; time = 0.0f; }
	int bgmData;//bgm番号
	int backGroundData;//背景の最初の番号
	float time;//目標タイム
};


struct StageObjectParrameta {  
   StageCsvDataV chara;  
   StageCsvDataV terra;  
};  
/// <summary>
/// ステージに関するデータを保持するクラス
/// </summary>
class StageData : public GameObject {  
public:  
   StageData(SceneBase* _scene);  
   ~StageData();  
   /// <summary>
   /// ステージに配置するオブジェクトのデータをロード
   /// </summary>
   /// <param name="_stage"></param>
   /// <returns></returns>
   bool LoadStageObjectCsvData(int _stage); 
   /// <summary>
   /// LoadStageObjectCsvDataとLoadStageConfigDataを纏めたもの
   /// </summary>
   /// <param name="_stage"></param>
   /// <returns></returns>
   bool LoadStageData(int _stage);
   /// <summary>
   /// csvから読み取ったデータを削除
   /// </summary>
   void AllClear();  
   /// <summary>
   /// ステージに関する構成情報をロードする
   /// </summary>
   /// <param name="_stage"></param>
   /// <returns></returns>
   bool LoadStageConfigData(int _stage);
   int GetCharaSize() { return (int)parrameta.chara.size(); }
   int GetTerraSize() { return (int)parrameta.terra.size(); }

   StageObjectCsvData GetCharaLines(int lins) { return parrameta.chara[lins]; }
   StageObjectCsvData GetTerraLines(int lins) { return parrameta.terra[lins]; }

   /// <summary>
   /// ステージに関する構成設定を取得
   /// </summary>
   /// <returns></returns>
   StageConfig GetStageConfigData()const { return config; }

   CsvReader csvReader;
private:  
   StageObjectParrameta parrameta;  
   StageConfig config;
   
};
