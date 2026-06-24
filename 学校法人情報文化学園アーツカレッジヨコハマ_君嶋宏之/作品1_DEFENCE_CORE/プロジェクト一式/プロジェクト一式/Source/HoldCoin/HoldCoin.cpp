#include "HoldCoin.h"
#include "../Screen.h"
#include "../Scene/Scenes/PlayScene.h"
#include "../Common/Sound/SoundManager.h"
#include "../Common/Function/Function.h"
#include "../Common/JsonSerializer/JsonSerializer.h"
#include "../Common/FileSystemUtils/FileSystemUtils.h"
#include "../Common/Debug/Debug.h"
#include "../../ImGui/imgui.h"
#include <fstream>		// ofstream,ifstreamを使うため
#include <cmath>

namespace
{
    constexpr float MAX_TIME_INSUFFICIENT_COIN_EVENT    = 1.5f;    // コイン不足の演出イベント時間　/ 秒
    constexpr float ALPHA_SET_INTERVAL                  = 0.5f;    // コイン不足の演出の不透明度を設定する間隔
    constexpr int SET_ALPHA                             = 150;     // コイン不足の演出の不透明度
}

HoldCoin::HoldCoin()
{
    drawCoin.position   = VECTOR2(110, Screen::HEIGHT - 115);

    PlayScene::startStageNumber;

    std::string filePath = GetHoldCoinDataFilePath();
    HoldCoinInfo::HoldCoinData holdCoinData;
   
    std::ifstream ifs(filePath);
    
    // ファイルが開けなかったら
    if (!ifs)
    {
        // エントリの作成
        FileSystemUtils::CreateDirectoryAndFile(filePath);
        // 初期は１００コインとする
        holdCoinData.holdCoinNum    = 100;  
        // holdCoinDataを書き出し
        SaveHoldCoinData(holdCoinData);
    }

    // Jsonを読み込み
    LoadHoldCoinData(holdCoinData);

    holdCoin                = holdCoinData.holdCoinNum;
    drawCoin.holdCoin       = holdCoinData.holdCoinNum;

    setStartHoldCoinData    = holdCoinData;

    coinUIImage[C_COIN]     = LoadGraph("data/texture/letter/coin.png");
    coinUIImage[C_NUMBER]   = LoadGraph("data/texture/letter/num_white.png");
}

HoldCoin::~HoldCoin()
{
    for (int i = 0;i < C_MAX;i++)
    {
        DeleteGraph(coinUIImage[i]);
    }
}

//-- ToDo 少しずつ減るようにして演出をつける ----

void HoldCoin::Update()
{
    //isInsufficientCoin = true;
    if (isInsufficientCoin)
    {
        time += Time::GameDeltaTime();

        // イベント時間を超えていたら
        if (time >= MAX_TIME_INSUFFICIENT_COIN_EVENT)
        {
            isInsufficientCoin  = false;
            time                = 0.0f;
        }
    }
   
    alpha -= (int)((SET_ALPHA / ALPHA_SET_INTERVAL) * Time::GameDeltaTime());

    // 浮動小数点の誤差を考慮してtimeが0.5秒ごとだったら
    if (std::fmodf(time, ALPHA_SET_INTERVAL) <= 0.1f)
    {
        // 不透明度の設定
        alpha = SET_ALPHA;
    }

    int errorCoin = drawCoin.holdCoin - holdCoin;  //  drawCoin.holdCoin と holdCoin の差の値
    int changeNum = abs((int)errorCoin / 10);      // 適応するコイン数
        
    // 差の値がるとき
    if (errorCoin != 0)
    {
        // 適応するコイン数が0の時
        if (changeNum == 0)
        {
            // コイン数が変更されるように値を設定
            changeNum = 1;
        }

        if (errorCoin > 0)
            drawCoin.holdCoin -= changeNum;
        else
            drawCoin.holdCoin += changeNum;
    }

    // デバッグモードがオンの時
    if (Debug::GetDebugMode())
    {
        // 保持コインデータを変更する
        ChangeHoldCoinDataUpdate();
    }
}

void HoldCoin::CoinDraw()
{
    //SetFontSize(40);
    //DrawFormatStringF(drawCoin.position.x, drawCoin.position.y, 0xffff00, "Coin");
    //SetFontSize(50);
    //// 一旦holdCoinを描画 ToDo　drawCoinに変更
    //DrawFormatStringF(drawCoin.position.x + 160.0f, drawCoin.position.y - 10.0f, 0xffffdd, "%000d", holdCoin);
    //SetFontSize(DEFAULT_FONT_SIZE);

    // COINを描画
    DrawRotaGraphF(drawCoin.position.x, drawCoin.position.y, 1.0f, 0.0f, coinUIImage[C_COIN], true);

    // コイン数を描画
    DrawRotaNum(drawCoin.position.x + 270.0f, drawCoin.position.y, drawCoin.holdCoin, 794 / 18, coinUIImage[C_NUMBER], 794 / 10, 159 / 2, 0, 1.0f, 0.0f, 1);

    // 消費量に対しコインが不足していた時
    if (isInsufficientCoin)
    {
        // 半透明描画に設定
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
        // 赤いボックスを描画
        DrawBoxAA(drawCoin.position.x - 180.0f, drawCoin.position.y - 45.0f, drawCoin.position.x + 390.0f, drawCoin.position.y + 45.0f, 0xff0000, true);
        // デフォルト描画に設定
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}

//------------------------------------

void HoldCoin::AddCoin(int _add)
{
    holdCoin            += _add;

    SoundManager::PlaySE(Sound_ID::COIN_ADD_SE);
}

bool HoldCoin::SubCoin(int _sub)
{
    holdCoin -= _sub;

    // 引かれたことによって、保持しているコイン数が０になるとわかったら
    if (holdCoin <= 0)
    {
        holdCoin = 0;
        return true;
    }
    return false;
}

void HoldCoin::ChangeHoldCoinDataUpdate()
{
    ImGui::Begin("HoldCoinData");

    if (ImGui::Button("Save"))
    {
        // holdCoinDataを書き出し
        SaveHoldCoinData(setStartHoldCoinData);
    }

    if (ImGui::Button("Load"))
    {
        // Jsonを読み込み
        LoadHoldCoinData(setStartHoldCoinData);
    }

    ImGui::SliderInt("StartHoldCoinNum", &setStartHoldCoinData.holdCoinNum, 100, 1000);

    ImGui::End();
}

std::string HoldCoin::GetHoldCoinDataFilePath()
{
    char setFilePath[CHAR_MAX];
    // holdCoinパスの初期化
    sprintfDx(setFilePath, "data/stage/stageData/stage_%.2d/holdCoin.json", PlayScene::startStageNumber);

    return std::string(setFilePath);
}

void HoldCoin::SaveHoldCoinData(const HoldCoinInfo::HoldCoinData& _holdCoinData)
{
    nlohmann::json json;

    json = _holdCoinData;
    // Jsonへ書き出し
    JsonSerializer::Save(GetHoldCoinDataFilePath(), json);
}

void HoldCoin::LoadHoldCoinData(HoldCoinInfo::HoldCoinData& _holdCoinData)
{
    // Jsonデータを読み込み
    nlohmann::json json = JsonSerializer::Load(GetHoldCoinDataFilePath());
    _holdCoinData       = json;
}
