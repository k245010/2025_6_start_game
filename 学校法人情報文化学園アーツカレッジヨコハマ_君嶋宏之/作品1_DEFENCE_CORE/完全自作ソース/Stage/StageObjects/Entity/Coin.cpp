#include "Coin.h"
#include "../../EntityController/EntityController.h"
#include "../../../Collision/Collisions/OBBCollision.h"
#include "../../../Collision/Collisions/SphereCollision.h"
#include "../../../../ImGui/imgui.h"
#include <set>

namespace
{
    constexpr float ROTATION_SPEED      = 1.2f;     // 回転速度
    constexpr float UP_SPEED            = 1.97f;    // 上昇速度
    constexpr float MAX_UP_LEN          = 27.0f;    // 上昇する最大の長さ

    constexpr float DROP_START_SCALE    = 0.5f;     // コインのドロップ直後のスケール
    constexpr float BIGGER_TIME         = 0.25f;    // 拡大しきるまでの時間(秒)
}

Coin::Coin(const Transform& _trans, const int& _hModel, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : EntityBase(_trans, _hModel, StageObjectData::STAGE_OBJECT_KIND::COIN, _hp, _putPlaceKind)
{
    angle = 1.0f / (GetRand(49) + 1);	// 0～1の範囲で回転値をランダムで設定
    upCount = 1.0f / (GetRand(49) + 1); // 0～1の範囲で上昇カウントをランダムで設定

    // 地面との当たり判定をして埋もれないようにする
    EntityController::GetEntityController()->CheckRaycastStageObject(transform.position + VECTOR3(0, 100, 0), transform.position + VECTOR3(0, -100, 0), std::set<int>{(int)StageObjectData::STAGE_OBJECT_KIND::GROUND_BLOCK}, & transform.position);

    // ランダムに座標をずらす //
    transform.position.x += GetRand(200) - 100;
    transform.position.z += GetRand(200) - 100;

    // 座標の原点がモデルの真ん中なので、座標を半径分上げて調整
    transform.position.y += transform.GetLenY() / 2;

    // 初期座標から変更したので、再度初期座標を設定
    initPosition = transform.position;

    finishScale = _trans.scale;
    transform.scale = VOne * DROP_START_SCALE;

    obbColl->SetTagMe(COLLISION_OBJECT_KIND::COIN);
    obbColl->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);

    sphereColl = new SphereCollision(&transform, COLLISION_OBJECT_KIND::COIN, "COIN_SPHERE_COLLISION", [this](const CollisionHitInfoData& tr) {return HitEntity(tr);});
    sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::COIN);
}

Coin::~Coin()
{
    if (sphereColl != nullptr)
    {
        delete sphereColl;
        sphereColl = nullptr;
    }
}

void Coin::Update()
{
    EntityBase::Update();
    /* 
    ImGui::Begin("Coin");
    ImGui::SliderFloat("rotSpeed", &ROTATION_SPEED, 0.0f, 5.0f);
    ImGui::SliderFloat("upSpeed", &UP_SPEED, 0.0f, 5.0f);
    ImGui::SliderFloat("upMax", &MAX_UP_LEN, 0.0f, 60.0f);

    ImGui::End();*/

    //_ BIGGER_TIMEの時間をかけて拡大 _//

    if (time <= BIGGER_TIME)
    {
        time            += Time::GameDeltaTime();
        transform.scale = finishScale * (time / BIGGER_TIME);
    }

    //_ 回転 _//

    angle   += Time::GameDeltaTime() * ROTATION_SPEED;

    // 角度値が一周していたら
    if (angle >= DegToRad * 360.0f)
        angle -= DegToRad * 360.0f;

    transform.rotation.y = angle;

    //_ 上昇 _//
  
    // upCountの値から上昇する長さを設定
    upLen                   = fabsf(sinf(upCount += (Time::GameDeltaTime() * UP_SPEED))) * MAX_UP_LEN;
    transform.position.y    = initPosition.y + upLen;
}

bool Coin::HitEntity(const CollisionHitInfoData& _targetData)
{
    if (_targetData.tag == COLLISION_OBJECT_KIND::PLAYER)
    {
        active = false;
        return true;
    }
    else if(_targetData.tag == COLLISION_OBJECT_KIND::COIN)
    {
        //VECTOR3 vec     = transform.position - _targetData.transform.position;
        //float radius    = sphereColl->GetRadius();
        //float moveLen   = vec.Size() - radius * 2;

        //if (moveLen < 0)
        //    return false;   // 移動値が0未満の場合 return

        //// 押し出し
        //transform.position += vec.Normalize() * (moveLen / 5);
    }
    return false;
}
