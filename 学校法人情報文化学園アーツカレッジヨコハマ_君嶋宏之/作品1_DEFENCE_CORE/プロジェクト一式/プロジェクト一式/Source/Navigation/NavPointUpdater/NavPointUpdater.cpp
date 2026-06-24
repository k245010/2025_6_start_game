#include "NavPointUpdater.h"
#include "../Node/NavPointBase.h"
#include "../NavigationManager/NavigationManager.h"
#include "../../Stage/StageObjects/StageObjectBase.h"
#include "../../Collision/CollisionFunction.h"
#include "../../Transform/Transform.h"
#include "../CostInfo.h"
#include "../../Common/Function/Function.h"
#include "../../../Library/myDxLib.h"
#include "../../../Library/DebugNew.h"

#define DO_TEST_CONNECT_INFO (1)	// ナビポイント同士のブロック状態の改善をするためのテスト処理を実行するかどうか

namespace
{
	constexpr float NAV_POINT_COLL_RADIUS_SIZE = 0.5f;	// ナビポイントの当たり判定を行う半径
}

NavPointUpdater::NavPointUpdater()
{
	navPointList.clear();
	stageObjList.clear();
	stageObjectChangedArea.clear();
	hitNavPointList.clear();
	noHitNavPointList.clear();
	newPushNavPointContainer.clear();

	activatingNavPointList.clear();
	deactivatingNavPointList.clear();
}

NavPointUpdater::~NavPointUpdater()
{
	navPointList.clear();
	stageObjList.clear();
	stageObjectChangedArea.clear();
	hitNavPointList.clear();
	noHitNavPointList.clear();
	newPushNavPointContainer.clear();

	activatingNavPointList.clear();
	deactivatingNavPointList.clear();

	saveTr.clear();
}

std::set<int> NavPointUpdater::Init(const std::list<StageObjectBase*> _list, NavigationManager* _navManager)
{
	for (auto itr = _list.begin();itr != _list.end();itr++)
		stageObjList.emplace_back(*itr);

	assert(_navManager != nullptr);
	navigationManager = _navManager;

	std::set<int> hitNavPointNumList;	// ステージオブジェクトに当たったナビポイントを保存するコンテナ

	// ステージとの当たり判定を行う
	for (auto sItr = _list.begin();sItr != _list.end();sItr++)
	{
		for (auto nItr = navPointList.begin();nItr != navPointList.end();nItr++)
		{
			// 生成元モデルのインスタンスのポインタとStageObjectBaseのポインタが同じだったら
			if (nItr->second->GetOwnerModelData().pointer == *sItr)
				continue;	// 生成元モデルとの当たり判定は行わない

			StageObjectBase& sObj	= *(*sItr);						// ステージオブジェクトのトランスフォーム
			VECTOR3 nPos			= nItr->second->GetPosition();	// NavPointの座標 

			// ナビポイントに影響を及ぼさないステージオブジェクトだったら
			if (!StageObjectData::IsStageObjectToNavigationNodeChange(sObj.GetStageObjectKind()))
				continue;	// 当たり判定を行わない

#ifdef _DEBUG
			saveTr.emplace_back(sObj.GetTransform());
			Transform navTr;
			navTr.position = nPos;
			saveTr.emplace_back(navTr);
#endif // _DEBUG

			// NavPoint座標とステージオブジェクトが当たっていたら
			if (OBBToNavPointColl(sObj.GetTransform(), nPos))
			{
				if (sObj.GetStageObjectKind() == StageObjectData::STAGE_OBJECT_KIND::SWAMP_BLOCK)
				{
					// 泥ブロックはNavPointをつぶさないでコスト倍率を上げる
					nItr->second->SetCostRate(NodeCostRateInfo::GetAreaCostRate(StageObjectData::STAGE_OBJECT_KIND::SWAMP_BLOCK));
				}
				else
				{
					// オブジェクトにかぶさっているので当たったナビポイントコンテナに追加
					hitNavPointNumList.emplace(nItr->first);
				}
			}
		}
	}
	return hitNavPointNumList;
}

void NavPointUpdater::Update()
{
	//CollisionUpdate();
}

int NavPointUpdater::CollisionUpdate()
{
	int voxelOwnerKey = -1;

	// コンテナのリセット　前フレームでNavPointはNavigationManagerでコンテナ移動済み（アクティブ化or非アクティブ化）なので
	activatingNavPointList.clear();
	deactivatingNavPointList.clear();

	//__ NavPointとステージオブジェクトの当たり判定 __//

	// ステージの変化した範囲を記録したコンテナに要素がなかったら
	if (stageObjectChangedArea.empty())
		return -1;

	for (auto areaItr = stageObjectChangedArea.begin();areaItr != stageObjectChangedArea.end();)
	{
		STAGE_OBJECT_CHANGED_KIND changeKind				= (STAGE_OBJECT_CHANGED_KIND)areaItr->first;	// ステージオブジェクトがどう変化したかの種類
		Transform areaTr									= areaItr->second.transform;					// ステージオブジェクトのトランスフォーム
		std::unordered_map<int,std::list<int>> changedNormalNavList;										// ステージオブジェクトによってアクティブが操作された使用用途がノーマルのナビポイントを記録する
		std::unordered_map<NavPointBase*, std::list<NavPointBase*>> blockedConnectCon;						// ブロックするNavPointの接続関係記録コンテナ
		std::unordered_map<NavPointBase*, std::list<NavPointBase*>> freeConnectCon;							// ブロックから解放するNavPointの接続関係記録コンテナ

		for (auto nItr = navPointList.begin();nItr != navPointList.end();nItr++)
		{
			VECTOR3 nPos = nItr->second->GetPosition();	// NavPointの座標
			

			// NavPoint座標とステージオブジェクトが当たっていたら
			if (OBBToNavPointColl(areaTr, nPos))
			{
				//saveTr.emplace_back(areaTr);
				//saveVec.emplace_back(nPos);

				int navNum = nItr->first;

				switch (changeKind)
				{
				case NavPointUpdater::STAGE_OBJECT_CHANGED_KIND::PUT:

					// オブジェクトにかぶさっているのでディアクティベーティングコンテナに追加
					deactivatingNavPointList.emplace(navNum);

					// ナビポイントの使用用途がノーマルだったら
					if (nItr->second->GetUseKind() == NODE_USE_KIND::NORMAL)
					{
						// オブジェクトが設置されて新しくボクセルを作るときに、すでにボクセルで管理されているナビポイントでなかったら
						if (!nItr->second->IsSettingBlockedOwnerVoxelKey())
							changedNormalNavList.insert(std::make_pair(navNum, nItr->second->GetTargetNumberContainer()));
					}
					break;
				case NavPointUpdater::STAGE_OBJECT_CHANGED_KIND::CLEAR:

					// オブジェクトが除去されたのでアクティベーティングコンテナに追加
					activatingNavPointList.emplace(navNum);
					
					// ナビポイントの使用用途がノーマルだったら　潰された（BLOCKED）ボクセルは追加せず、別処理で管理している
					if (nItr->second->GetUseKind() == NODE_USE_KIND::NORMAL)
						changedNormalNavList.insert(std::make_pair(navNum, nItr->second->GetTargetNumberContainer()));
					break;
				default:

					assert(false);
					break;
				}
			}
			// 当たっていなかったら  接続の線分がオブジェクトに潰されているか判定する
			else
			{
				const std::unordered_map<NavPointBase*, TargetData>& targetCon = nItr->second->GetTargetContainer();	// NavPointの接続相手コンテナ

				for (const auto& target : targetCon)
				{
					NavPointBase& trNav = *(target.first);

					

					//int trNavNum		= trNav.GetGlobalNumber();

					// NavPointのエッジとステージオブジェクトが当たっていたら
					if (AABBToNavPointEdgeColl(areaTr, nPos, trNav.GetPosition()))
					{
						switch (changeKind)
						{
						case NavPointUpdater::STAGE_OBJECT_CHANGED_KIND::PUT:

							// オブジェクトにかぶさっているので潰されている状態に設定
							//nItr->second->SetTargetBlockState(target.first, NODE_BLOCK_STATE::BLOCKED);

							if (trNav.GetUseKind() == NODE_USE_KIND::WALL)
								int a = 0;

							blockedConnectCon[nItr->second].emplace_back(target.first);
							break;
						case NavPointUpdater::STAGE_OBJECT_CHANGED_KIND::CLEAR:

							// オブジェクトが除去されたので潰されていない状態に設定
							//nItr->second->SetTargetBlockState(target.first, NODE_BLOCK_STATE::FREE);

							//pushTargetCon.emplace(target.first);

							freeConnectCon[nItr->second].emplace_back(target.first);

							
							break;
						default:

							assert(false);
							break;
						}

						// ToDo changedNormalNavListの代入　エッジがとても長くてナビポイントは潰されないけど、エッジが潰されたときのボクセル構築、管理を考える 
					}
				}

				//// 相手との接続を削除
				//for (auto del = deleteTargetCon.begin();del != deleteTargetCon.end();)
				//{
				//	NavPointBase* deleteNav = *del;

				//	nItr->second->DeleteTarget(deleteNav);
				//	deleteNav->DeleteTarget(nItr->second);

				//	del = deleteTargetCon.erase(del);
				//}

				//// 相手との接続を追加
				//for (auto push = pushTargetCon.begin();push != pushTargetCon.end();)
				//{
				//	NavPointBase* pushNav = *push;

				//	nItr->second->PushTarget(pushNav);
				//	pushNav->PushTarget(nItr->second);

				//	push = pushTargetCon.erase(push);
				//}
			}
		}

		if (changeKind == STAGE_OBJECT_CHANGED_KIND::PUT)
		{
			// アクティブが操作されたナビポイントボクセルをコンテナにpushしコンテナのkeyを得る
			voxelOwnerKey = navigationManager->PushVoxelNavNode(changedNormalNavList, areaItr->second);
			
			//_ ナビポイントにオーナーのボクセルキーを記録 _//

			for (const auto& saveNum : changedNormalNavList)
			{
				// ボクセルコンテナのkeyを記録　削除時の処理を最適化
				navPointList[saveNum.first]->SetBlockedOwnerVoxelKey(voxelOwnerKey);
			}

			//_ ナビポイントの接続関係にオーナーのボクセルキーを記録 _//

			for (auto blockedConnectItr = blockedConnectCon.begin();blockedConnectItr != blockedConnectCon.end();)
			{
				NavPointBase& navMe = *blockedConnectItr->first;	// 自身のナビポイント

				// 接続関係が潰されている状態なので接続相手の情報を変更 //
				for (auto targetItr = blockedConnectItr->second.begin();targetItr != blockedConnectItr->second.end();targetItr++)
				{
					NavPointBase& navTr = **targetItr;

					// 接続相手の情報をBLOCKEDに変更
					navMe.SetTargetDataBlock(*targetItr, voxelOwnerKey);
#if DO_TEST_CONNECT_INFO
					navTr.SetTargetDataBlock(&navMe, voxelOwnerKey);
#endif
				}
				blockedConnectItr = blockedConnectCon.erase(blockedConnectItr);
			}
		}
		else if(changeKind == STAGE_OBJECT_CHANGED_KIND::CLEAR)
		{
			std::set<int> voxelOwnerKeyTargetVoxelOwnerKeyList;			// ボクセルkeyのボクセル同士の接続相手keyコンテナ

			std::unordered_map<int,NavPointBase*> allNavPointList = navigationManager->GetNavigationPointContainer();	// 全てのナビポイントを取得
				
			bool isGetVoxelOwnerKey							= false;	// ボクセルkeyが取得できたかどうか
			
			VECTOR3 VoxelOwnerNavPointPos					= VZero;	// オーナーのボクセルナビポイントの座標
			NavPointBase* nearVoxelPosToDeleteNavPoint		= nullptr;	// オーナーのボクセルナビポイントの座標に一番近いボクセルから削除されるナビポイント
			float minDistance								= FLT_MAX;	// VoxelOwnerNavPointPosとnearVoxelPosTofreeConnectPosの最小距離

			for (const auto& deleteNum : changedNormalNavList)
			{
				allNavPointList = navigationManager->GetNavigationPointContainer(); // 全てのナビポイントを取得(更新)

				// ボクセルkeyが取得できてなかったら
				if (!isGetVoxelOwnerKey)
				{
					// ボクセルコンテナのkeyの取得
					voxelOwnerKey = navPointList[changedNormalNavList.begin()->first]->GetBlockedOwnerVoxelKey();
					
					if (voxelOwnerKey < 0)
						continue;	// ボクセルが生成されたナビポイントでなかったら continue
					
					// navPointListにvoxelOwnerKeyが存在したら
					if (HasUnorderedMapContainerKey(allNavPointList, voxelOwnerKey))
					{
						const auto& voxelTargetList = allNavPointList[voxelOwnerKey]->GetTargetContainer();	// ボクセルkeyのボクセル同士の接続相手keyを取得

						for (const auto& voxelTar : voxelTargetList)
						{
							// 接続相手がボクセルだったら
							if (voxelTar.first->GetKind() == NAV_POINT_KIND::VOXEL_WAY_POINT)
							{
								// 接続相手のボクセルキーをpush
								voxelOwnerKeyTargetVoxelOwnerKeyList.emplace(voxelTar.first->GetGlobalNumber());
							}
						}
						VoxelOwnerNavPointPos = allNavPointList[voxelOwnerKey]->GetPosition();			// ボクセルの座標を設定
					}
					isGetVoxelOwnerKey = true;
				}

				//_ ボクセルキーを接続関係から削除 _//

				for (auto freeConnectItr = freeConnectCon.begin();freeConnectItr != freeConnectCon.end();)
				{
					NavPointBase& navMe = *freeConnectItr->first;	// 自身のナビポイント

					// 接続関係が潰されていない状態になったので接続相手の情報を変更 //
					for (auto targetItr = freeConnectItr->second.begin();targetItr != freeConnectItr->second.end();targetItr++)
					{
						// 接続相手が持っているボクセルキーと一致しなかたっら
						if (!navMe.HasTargetDataVoxelOwnerKey(*targetItr, voxelOwnerKey))
							continue;

						NavPointBase& navTr = **targetItr;

						// 接続関係を潰されている状態のため、保存したボクセルキーを削除する　ボクセルコンテナが空になったら、NODE_BLOCK_STATEをFREEにする
						navMe.DeleteTargetDataVoxelKey(*targetItr, voxelOwnerKey);
#if DO_TEST_CONNECT_INFO
						navTr.DeleteTargetDataVoxelKey(&navMe, voxelOwnerKey);
#endif

#ifdef _DEBUG

						//saveVec.emplace_back(navMe.GetPosition());
#endif // _DEBUG
					}
					freeConnectItr = freeConnectCon.erase(freeConnectItr);
				}
				// ボクセルからナビポイント情報を削除
				navigationManager->DeleteVoxelInNavNode(voxelOwnerKey, deleteNum.first);
			}

			//allNavPointList = navigationManager->GetNavigationPointContainer();

			for (const auto& deleteNum : changedNormalNavList)
			{
				if (HasUnorderedMapContainerKey(allNavPointList, deleteNum.first))
				{
					NavPointBase& deleteNavPoint = *allNavPointList[deleteNum.first];	// ボクセルから削除されるナビポイント

					VECTOR3 navMePos		= deleteNavPoint.GetPosition();
					float squareDistance	= VSquareSize(navMePos - VoxelOwnerNavPointPos);	// お互いの距離の二乗

					// minDistanceよりも、距離が短かったら
					if (minDistance > squareDistance)
					{
						minDistance = squareDistance;
						nearVoxelPosToDeleteNavPoint = &deleteNavPoint;	// よりボクセルナビポイント座標に近いナビポイントを記録
					}
				}
			}

			// ボクセルkeyのボクセル同士の接続相手が存在するとき
			if (voxelOwnerKeyTargetVoxelOwnerKeyList.size() > 0)
			{
				// オーナーのボクセルナビポイントの座標に一番近いブロックから解放されるナビポイントが設定されていたら
				if (nearVoxelPosToDeleteNavPoint != nullptr)
				{
					for (const auto& voxelTargetVoxelKey : voxelOwnerKeyTargetVoxelOwnerKeyList)
					{
						HasUnorderedMapContainerKey(allNavPointList, voxelTargetVoxelKey, true);

						NavPointBase& voxelNavPoint = *allNavPointList[voxelTargetVoxelKey];

						// 接続相手としてお互いをpush //  隣接した罠のブロックを削除した際に、ボクセルWayPointと接続が行われないバグを防ぐ
						nearVoxelPosToDeleteNavPoint->PushTarget(&voxelNavPoint);
						voxelNavPoint.PushTarget(nearVoxelPosToDeleteNavPoint);

#ifdef _DEBUG

						// saveVecPair.emplace_back(std::make_pair(nearVoxelPosToDeleteNavPoint->GetPosition(), voxelNavPoint.GetPosition()));
#endif // _DEBUG
					}
				}
			}
		}
		changedNormalNavList.clear();

		// 当たり判定を終えたのでコンテナから削除
		areaItr = stageObjectChangedArea.erase(areaItr);
	}

	/*const std::unordered_map<int, NavigationManager::VoxelNavNodeData> voxelNavList = navigationManager->GetVoxelNavList();

	for (auto voxel : voxelNavList)
	{
		Transform voxelTrans = voxel.second.voxelTransfrom;


	}*/

	return voxelOwnerKey;
}

void NavPointUpdater::Draw()
{
#ifdef _DEBUG

	/*if (CheckHitKey(KEY_INPUT_G))
	{
		saveTr.clear();
		saveVec.clear();
	}

	for (auto& vec : saveVec)
	{
		DrawSphere3D(vec, 2.0f, 16, 0xff00ff, 0xffffff, false);
	}

	for (auto itr = saveTr.begin(); itr != saveTr.end(); itr++)
	{
		VECTOR3 halfLen = VDivI(VMult(itr->size, itr->scale),2);
		DrawCube3D(itr->position + halfLen, itr->position - halfLen, 0xff0000, 0xffffff, false);
	}*/
#endif
}

void NavPointUpdater::ReloadCollision()
{
	inContainer = false;
}

void NavPointUpdater::ReloadCollision(const int& _num, bool findChecked)
{
	inContainer = false;
}

bool NavPointUpdater::CreateCollision(NavPointBase* _ptr)
{
	int number = _ptr->GetGlobalNumber();

	std::pair<std::unordered_map<int, NavPointBase*>::iterator, bool> pair = navPointList.insert(std::make_pair(number, _ptr));

	// キーが登録できなかったら
	if (!pair.second)
	{
		assert(false);
		return false;
	}
	noHitNavPointList.emplace(number);
	return true;
}

bool NavPointUpdater::OBBToNavPointColl(const Transform& _block,const VECTOR3& _navP) const
{
	VECTOR3 halfLen			= VDivI(VMult(_block.size, _block.scale), 2);
	VECTOR3 blockPos		= _block.position;
	VECTOR3 blockRot		= _block.rotation;
	VECTOR3 navPos			= _navP;
	VECTOR3 returnVec		= VZero;
	bool isCalculation		= false;

	const VECTOR DISTANCE	= VSub(navPos, blockPos);

	// VECTORの座標のメンバ一つ一つを見る
	for (int m = 0;m < 3;m++)
	{
		// 辺の半分の長さが０以下だったら計算しない
		if (GetVMem(halfLen, m) <= 0.0f)
			continue;

		isCalculation = true;

		// 内積を使って四角形に対して現在座標の位置を割り出す
		float s = VDot(DISTANCE, GetVDirection(m, blockRot)) / GetVMem(halfLen, m);
		s		= fabsf(s);

		// 四角形の辺からはみ出ている
		if (s > 1)
		{
			// どれだけはみ出したかの算出
			returnVec += (GetVDirection(m, blockRot) * (s - 1)) * GetVMem(halfLen, m);
		}
	}
	// NavPointの座標よりも近かったらtrue遠かったらfalse　
	return VSize(returnVec) <= 0.0f && isCalculation;
}

bool NavPointUpdater::AABBToNavPointEdgeColl(const Transform& _trans, const VECTOR3& _navP1, const VECTOR3& _navP2) const
{
	VECTOR3 rayDir = _navP2;

	if (VAllSameF(rayDir, 0.0f))
		return false;


	//__ レイの設定 __//

	// 光線を境界ボックスの空間へ移動
	MATRIX mat		= _trans.GetMatrix();

	// 始点
	VECTOR3 pRay	= _navP1 * mat;

	for (int i = 0;i < 3;i++)
	{
		// 座標をリセットする (0.0f
		mat.m[3][i] = 0.0f;
	}
	// 方向ベクトル
	VECTOR3 dirRay	= rayDir * mat;


	//__ 交差判定 __//

	VECTOR3 p		= pRay;
	VECTOR3 d		= dirRay;

	VECTOR3 halfLen = VDivI(VMult(_trans.size, _trans.scale), 2);

	VECTOR3 min		= VScale(halfLen,-1);
	VECTOR3 max		= halfLen;

	float t			= -FLT_MAX;
	float tMax		= FLT_MAX;

	for (int i = 0;i < 3;i++)
	{
		if (abs(GetVMem(d, i)) < EPSILON_NUM)
		{
			float t1 = 0.0f;
			float t2 = 0.0f;
			if (GetVMem(p, i) < GetVMem(min, i) || GetVMem(p, i) > GetVMem(max, i))
			{
				// 交差していない
				return false;
			}
			else
			{
				// スラブとの距離を算出
				// t1が近スラブ、t2が遠スラブとの距離

				if (GetVMem(d, i) == 0.0f)
					continue;

				float odd	= 1.0f / GetVMem(d, i);
				t1			= (GetVMem(min, i) - GetVMem(p, i)) * odd;
				t2			= (GetVMem(max, i) - GetVMem(p, i)) * odd;

				if (t1 > t2)
				{
					float tmp	= t1;
					t1			= t2;
					t2			= tmp;
				}
			}

			if (t1 > t)
				t		= t1;

			if (t2 < tMax)
				tMax	= t2;

			// スラブ交差チェック
			if (t >= tMax)
			{
				// 交差していない
				return false;
			}
		}
	}
	
	constexpr int COLL_VMEM_MAX = 3;

	VECTOR3 edgeCenterPos	= VDivI((_navP1 + _navP2), 2);
	VECTOR3 returnVec		= VZero;
	float radius			= fabsf(VSize(_navP1 - _navP2)) / 2;

	const VECTOR DISTANCE	= VSub(edgeCenterPos, _trans.position);

	// VECTORの座標のメンバ一つ一つを見る
	for (int m = 0;m < COLL_VMEM_MAX;m++)
	{
		// 辺の半分の長さが０以下だったら計算しない
		if (GetVMem(halfLen, m) <= 0)
			continue;

		// 内積を使って四角形に対して現在座標の位置を割り出す
		float s = VDot(DISTANCE, GetVDirection(m, _trans.rotation)) / GetVMem(halfLen, m);
		s		= fabsf(s);
		
		// 四角形の辺からはみ出ている
		if (s > 1)
		{
			// どれだけはみ出したかの算出
			returnVec = VAdd(returnVec, VScale(VScale(GetVDirection(m, _trans.rotation), (s - 1)), GetVMem(halfLen, m)));
		}
	}
	// レイを直径とした球がBOXに当たっているなら交差している
	return VSize(returnVec) < radius ? true : false;
}

Transform NavPointUpdater::GetPositionToTransform(const VECTOR3& _startPos, const VECTOR3& _endPos) const
{
	//_ 指定した範囲からトランスフォームを初期化　回転可のBOXのイメージ _//

	Transform transform;
	transform.position	= VDivI((_startPos + _endPos), 2);		// 中心座標
	transform.size		= GetVDistance(_startPos, _endPos);		// BOXの直径

	return transform;
}

int NavPointUpdater::StageObjectPutAreaBox(const Transform& _trans, const int& _hp)
{
	stageObjectChangedArea.insert(std::make_pair((int)STAGE_OBJECT_CHANGED_KIND::PUT, AreaData(_trans,_hp)));

	int voxelOwnerKey = CollisionUpdate();
	
	return voxelOwnerKey;
}

int NavPointUpdater::StageObjectPutAreaBox(const VECTOR3& _startPos, const VECTOR3& _endPos, const int& _hp)
{
	Transform transform = GetPositionToTransform(_startPos, _endPos);

	stageObjectChangedArea.insert(std::make_pair((int)STAGE_OBJECT_CHANGED_KIND::PUT, AreaData(transform,_hp)));

	int voxelOwnerKey = CollisionUpdate();

	return voxelOwnerKey;
}

void NavPointUpdater::StageObjectClearAreaBox(const Transform& _trans)
{
	stageObjectChangedArea.insert(std::make_pair((int)STAGE_OBJECT_CHANGED_KIND::CLEAR, AreaData(_trans,0)));
	
	int voxelOwnerKey = CollisionUpdate();

	//if (voxelOwnerKey == -1)
	//	return;

	//stageObjectChangedTransform.insert(std::make_pair(_trans, voxelOwnerKey));
}

void NavPointUpdater::StageObjectClearAreaBox(const VECTOR3& _startPos, const VECTOR3& _endPos)
{
	Transform transform = GetPositionToTransform(_startPos, _endPos);

	stageObjectChangedArea.insert(std::make_pair((int)STAGE_OBJECT_CHANGED_KIND::CLEAR, AreaData(transform,0)));
	
	int voxelOwnerKey = CollisionUpdate();

	//if (voxelOwnerKey == -1)
	//	return;

	//stageObjectChangedTransform.insert(std::make_pair(tr, voxelOwnerKey));
}

void NavPointUpdater::StageObjectPutAreaBoxHPChange(const int& _number, const int& _hp)
{
	//if (stageObjectChangedTransform.find(_trans) == stageObjectChangedTransform.end())
	//	assert(false);

	//int key = stageObjectChangedTransform[_trans];	// PushVoxelNavNodeで返ってきたコンテナのkey
	
	// ボクセルのHPを設定
	navigationManager->VoxelNavNodeHPChange(_number, _hp);
}

void NavPointUpdater::ResetCollisionHitContainer()
{
	hitNavPointList.clear();
}

