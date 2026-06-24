#pragma once
#include "../Library/gameObject.h"
#include "../Imgui/imgui.h"
#include "starData.h"
#include "stageData.h"

class StageScore;
class CommonData:public GameObject{
public:
	CommonData(SceneBase* _scene) :GameObject(_scene) { 
		data.stage = 0; 
		/*for (int i = 0; i < stage::MAX_STAGE; i++)
		{
			data.bestCoinNum[i] = -1;
			data.bestClearTime[i] = -1.0f;
			for (int j = 0; j < STAR_NUM; j++)
			{
				data.getStar[j][i] = false;
			}
		}*/
	};
	~CommonData() {};
	/*void Update() {
		#if _DEBUG
		ImGui::Begin("CommonData");
		ImGui::Text("Stage1：Time%f,Star%d %d %d", data.bestClearTime[0], data.getStar[0][0], data.getStar[1][0], data.getStar[2][0]);
		ImGui::Text("Stage2：Time%f,Star%d %d %d", data.bestClearTime[1], data.getStar[0][1], data.getStar[1][1], data.getStar[2][1]);
		ImGui::Text("Stage3：Time%f,Star%d %d %d", data.bestClearTime[2], data.getStar[0][2], data.getStar[1][2], data.getStar[2][2]);
		ImGui::End();
		#endif
	}*/

	struct Data{
		int stage;//現在選択されているステージ
		bool clear;//セレクトシーンに戻るときにステージをクリアして戻ったかどうか

		/*int bestCoinNum[stage::MAX_STAGE];//各ステージの最大コイン取得数　記録されていないなら-1が入っている
		float bestClearTime[stage::MAX_STAGE];//各ステージのベストクリアタイム　記録されていないなら-1が入っている
		bool getStar[STAR_NUM][stage::MAX_STAGE];//各ステージのスターを入手したかどうか　入手していたらtrue*/
		int maxCoinNum;//ステージのコイン総量
	};
	void SetStage(int _stage) { data.stage = _stage; }
	/*//ステージのベストコイン取得数を更新したならデータを上書きする
	void SetBestCoinNum(int _stage, int _newCoin) {
		if (data.bestCoinNum[_stage] == -1 || data.bestCoinNum[_stage] < _newCoin)
			data.bestCoinNum[_stage] = _newCoin;
	};
	//ステージのベストタイムを更新したならデータを上書きする
	void SetBestClearTime(int _stage,float _time) {
		if (data.bestClearTime[_stage] == -1.0f || data.bestClearTime[_stage] > _time)
			data.bestClearTime[_stage] = _time;
	};
	//ステージの☆取得条件を更新する　一度☆を取ったら次に☆を取り逃しても取った判定にする
	void SetGetStar(int _stage,StarType _starType, bool _getStar) {
		if(data.getStar[_starType][_stage]==false){
			data.getStar[_starType][_stage] = _getStar;
		}
	}*/
	//ステージのコインの総量をセット
	void SetMaxCoinNum(int coinNum) { data.maxCoinNum = coinNum; }
	void SetClear(bool _clear) { data.clear = _clear; }

	Data GetData()const { return data; }

	
private:
	Data data;
	
};