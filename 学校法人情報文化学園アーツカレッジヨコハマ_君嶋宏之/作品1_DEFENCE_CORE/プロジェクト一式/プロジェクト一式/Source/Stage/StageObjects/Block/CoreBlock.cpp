#include "CoreBlock.h"
#include "../../../Collision/Collisions/SphereCollision.h"
#include "../../../Gauge/Gauge.h"
#include "../../../Screen.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../Common/Sound/SoundID.h"
#include "../../../Common/Effekseer/EffekseerObjectManager.h"
#include "../../../Common/UI/UIDrawManager/UIDrawManager.h"
#include "../../../../Library/DebugNew.h"

namespace
{
    const float GAUGE_WIDTH             = 650.0f;           // HPゲージの横幅
    const float GAUGE_HEIGHT            = 20.0f;            // HPゲージの縦幅
    const VECTOR2 GAUGE_POSITION        = VECTOR2(Screen::WIDTH_CENTER - GAUGE_WIDTH / 2 - 450.0f, 20); // HPゲージの表示座標

    constexpr float ROTATION_SPEED      = 0.05f;            // 回転速度
    constexpr float UP_SPEED            = 0.8f;             // 上昇速度
    constexpr float MAX_UP_LEN          = 60.0f;            // 上昇する最大の長さ

    constexpr float MAX_SHAKE_TIME      = 0.2f;             // 最大振動時間
    constexpr int MAX_SHAKE_DISTANCE    = 4;                // 最大振幅距離
}

CoreBlock::CoreBlock(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : BlockBase(_trans, _modelData, StageObjectData::STAGE_OBJECT_KIND::CORE_BLOCK,_hp, _putPlaceKind)
{
    coreUIImage = LoadGraph("data/texture/gameVictoryCoreImage.png");

    sphereColl  = new SphereCollision(&transform, COLLISION_OBJECT_KIND::CORE_BLOCK, "CORE_BLOCK_SPHERE", [this](const CollisionHitInfoData& tr) {return HitBlock(tr);});

#if GAME_FIRST_PERSON
    sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY_ATTACK);
#else
    sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY);
#endif


    shakePosition   = VECTOR2I(0,0);
    initPosition    = transform.position;

    // モデルを発光させる
    MV1SetMaterialEmiColor(hModel, 0, GetColorF(1, 1, 1, 1));

    hpGauge         = new Gauge(GAUGE_POSITION, GAUGE_WIDTH, GAUGE_HEIGHT, 0.0f, hp);
    // HPゲージの色の設定
    hpGauge->SetDefaultFillColor(0, 150, 255);

    // UIDraw関数を登録
    uiFunctionIDNumber = UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this] {UIDraw();});
    // 描画順を後に回す
    UIDrawManager::GetUIDrawManagerInstance()->SetDrawOrder(uiFunctionIDNumber, -1);
}

CoreBlock::~CoreBlock()
{
    if (sphereColl != nullptr)
    {
        delete sphereColl;
        sphereColl = nullptr;
    }

    if (hpGauge != nullptr)
    {
        delete hpGauge;
        hpGauge = nullptr;
    }
    
    // UIDraw関数を解放
    UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(uiFunctionIDNumber);
}

void CoreBlock::Update()
{
    //_ コアの回転 & 上昇の設定 _//

    angle += Time::GameDeltaTime() * ROTATION_SPEED;

    // 角度値が一周していたら
    if (angle >= DegToRad * 360.0f)
        angle -= DegToRad * 360.0f;

    // upCountの値から上昇する長さを設定
    upLen = fabsf(sinf(upCount += (Time::GameDeltaTime() * UP_SPEED))) * MAX_UP_LEN;

    //_ UIコアの振幅座標設定 _//

    if (isDamage)
    {
        // 振動時間を設定
        shakeTime   = MAX_SHAKE_TIME;
        isDamage    = false;
    }

    // 振動時間が0を超えているとき
    if ((shakeTime -= Time::GameDeltaTime()) > 0.0f)
    {
        // 振幅座標をランダムに設定 //
        shakePosition.x = GetRand(MAX_SHAKE_DISTANCE * 2) - MAX_SHAKE_DISTANCE;
        shakePosition.y = GetRand(MAX_SHAKE_DISTANCE * 2) - MAX_SHAKE_DISTANCE;
    }
}

void CoreBlock::Draw()
{
    if (isBroken)
        return; // コアが壊れていたら return

    // 描画前にトランスフォームを設定 //
    transform.rotation.y = angle;
    transform.position.y = initPosition.y + upLen;

    BlockBase::Draw();

    // トランスフォームの座標を初期値に戻す
    transform.position = initPosition;
    
    hpGauge->SetDefaultFillColor(0, 190, 255);
}

void CoreBlock::UIDraw()
{
    if (isBroken)
        return; // コアが壊れていたら return

    // HPゲージの描画
    hpGauge->Draw(VECTOR2(110.0f, 5.0f), hp);

    // コアUIの描画
    DrawRotaGraphF(-10.0f + shakePosition.x, -14.0f + shakePosition.y, 0.7f, 0.0f, coreUIImage, true);
}

bool CoreBlock::HitBlock(const CollisionHitInfoData& _targetData)
{
    const int DAMAGE = (int)_targetData.GetDamage(sphereColl->GetTagMe());

    VECTOR3 effectPos = transform.position;

    // 当たったときのダメージ量が設定されていたら
    if (DAMAGE > 0.0f)
    {
        hp          -= DAMAGE;
        isDamage    = true;

        //_ 当たった座標情報があったらeffectPosに座標を代入 _//

        if (_targetData.hitPointData)
        {
            if (!_targetData.hitPointData.value().hitPosition.empty())
                effectPos = *_targetData.hitPointData.value().hitPosition.begin();
        }
    
        // Y座標は見やすいように、コアの真ん中に固定   
        effectPos.y = transform.position.y + transform.GetLenY() / 2;
        // ヒットエフェクトの再生
        EffekseerObjectManager::SetEffect(effectPos, EffekseerObjectManager::EF_CORE_HIT);

        SoundManager::PlaySE(Sound_ID::CORE_DAMAGE_SE);

        if(hp <= 0)
        {
            SoundManager::PlaySE(Sound_ID::CORE_BROKEN_SE);

            isBroken    = true;
            hp          = 0;
            return true;
        }
    }
    return false;
}
