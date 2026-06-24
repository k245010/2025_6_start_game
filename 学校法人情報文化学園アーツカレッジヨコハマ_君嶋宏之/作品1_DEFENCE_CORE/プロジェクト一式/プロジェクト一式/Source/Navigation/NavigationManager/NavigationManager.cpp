#include "NavigationManager.h"
#include "../Node/NavPointBase.h"
#include "../Node/WayPoint/WayPoint.h"
#include "../Node/NavMesh/PortalPoint.h"
#include "../../Enemy/EnemyManager/EnemyManager.h"
#include "../../Stage/StageManager/StageManager.h"
#include "../../Player/Player.h"
#include "../CostInfo.h"
#include "../../Common/Function/function.h"
#include "../../Common/Debug/Debug.h"
#include "../../../ImGui/imgui.h"
#include <queue>
#include <set>
#include <list>
#include "../../../Library/DebugNew.h"

//#define NODE_USE_KIND_WALL_ERROR (1); // NavPointBaseクラスのNODE_USE_KIND::WALLを無視する　（経路探索で使わない）
#define VOXEL_USE (1)					// ボクセルを経路探索候補として使う

#ifdef _DEBUG

std::unordered_set<int> NavigationManager::debugDeleteNavPointNumberCheckList = {};	// 削除したナビポイントナンバーを記録する
#endif // _DEBUG


NavigationManager::NavigationManager()
{
	wayPointController	= new WayPointController();
	navMeshController	= new NavMeshController();
	navPointUpdater		= new NavPointUpdater();

	activeNavPointList.clear();
	inactiveNavPointList.clear();
	voxelNavList.clear();

	debugNavgationList.clear();
	debugNavPointCostList.clear();
	debugOpenedNavPointList.clear();
	debugVoxelList.clear();
	debugCostRateUpNavPointList.clear();
	debugNavCountList.clear();
	debugDeletedVoxelWayPointNumber.clear();
	debugTargetDataDeletedVoxelWayPointNumber.clear();
}

void NavigationManager::Init(bool _doNavSetting)
{
	stageManager	= FindGameObject<StageManager>();
	enemyManager	= FindGameObject<EnemyManager>();
	player			= FindGameObject<Player>();

	//__ NavMeshでのNavPointの初期化 __//

	// ポータルの生成
	navMeshController->Init(player,stageManager, _doNavSetting);
	
	// 初期化されたPortalPointコンテナを返す
	std::unordered_map<int, PortalPoint*>& portalContainer = navMeshController->GetInitCreatePortalPointContainer();

	for (auto& portal : portalContainer)
	{
		if (portal.second == nullptr)
			continue;

		PortalPoint& porTr	= *portal.second;
		NavPointBase* navP	= portal.second;

		for (const auto& pTr : porTr.GetInitTargetPortalPointContainer())
		{
			if (pTr == nullptr)
				continue;

			// 接続相手（NavPointBaseのTargetList）の初期化
			navP->PushTarget(pTr);
		}
	}

	for (auto& portal : portalContainer)
	{
		NavPointBase* navP = portal.second;

		// NavPointBaseのナンバーをnavPointCreateNumで変更し、NavigationManagerで接続相手がmap初期化時に変更される問題を防ぐ 　例）manager map 0 - 100 <- navMesh map 0 - 100 のようにpushするとき manager map は101となる
		navP->SetGlobalNumber(navPointCreateNum);

		//_ NavPointのコンテナの初期化 _//
		
		// 初期化はすべてアクティブなNavPointとする　この後の処理ですべてのNavPointとステージオブジェクトで当たり判定していきアクティブ、非アクティブを決める
		activeNavPointList.insert(std::make_pair(navPointCreateNum, navP));
		navPointUpdater->CreateCollision(activeNavPointList[navPointCreateNum]);
		// activeNavPointをpush
		navigationNavPointList.insert(std::make_pair(navPointCreateNum, navP));

#ifdef _DEBUG

		debugDeleteNavPointNumberCheckList.insert(navPointCreateNum);

#endif // _DEBUG


		navPointCreateNum++;
	}

	//__ ステージ初期化後のステージオブジェクトとNavPointの当たり判定 __//

	// ステージオブジェクトのコンテナを渡す　当たり判定をするため
	std::set<int> inactiveNavPoint = std::move(navPointUpdater->Init(stageManager->GetAllStageObject(),this));

	//_ 当たり判定結果からステージオブジェクトに当たっていたNavPointを非アクティブ化する (ステージが動的に変化する可能性があるので、削除はしないで非アクティブ化コンテナに保存) _//
	
	for (const auto& navNum : inactiveNavPoint)
	{
		// アクティブから非アクティブコンテナへ移動
		ActiveForInactivePushNavPoint(navNum);
	}
}

NavigationManager::~NavigationManager()
{
	if (wayPointController	!= nullptr)
	{
		wayPointController->DestroyMe();
		wayPointController	= nullptr;
	}

	if (navMeshController	!= nullptr)
	{
		navMeshController->DestroyMe();
		navMeshController	= nullptr;
	}

	if (navPointUpdater != nullptr)
	{
		navPointUpdater->DestroyMe();
		navPointUpdater = nullptr;
	}

	activeNavPointList.clear();
	inactiveNavPointList.clear();
	voxelNavList.clear();

	//saveRoot.clear();

	debugNavgationList.clear();
	debugOpenedNavPointList.clear();
	debugVoxelList.clear();
	debugCostRateUpNavPointList.clear();
	debugNavPointCostList.clear();
	debugNavCountList.clear();
	debugDeletedVoxelWayPointNumber.clear();
	debugTargetDataDeletedVoxelWayPointNumber.clear();

#ifdef _DEBUG

	// if (!debugDeleteNavPointNumberCheckList.empty())
	// 	assert(false && "削除されていないナビポイントがあります");

	debugDeleteNavPointNumberCheckList.clear();
#endif // _DEBUG


}

const VECTOR3* NavigationManager::GetNavPointPosition(const int& _num)
{
	const std::unordered_map<int, NavPointBase*>::iterator itr = navigationNavPointList.find(_num);

	// キーが見つからなかったら
	if (itr == navigationNavPointList.end())
		return nullptr;

	if (itr->second == nullptr)
		assert(false);

	return &navigationNavPointList[_num]->GetPosition();
}

const NODE_USE_KIND* NavigationManager::GetNavPointUseKind(const int& _num)
{
	const std::unordered_map<int, NavPointBase*>::iterator itr = navigationNavPointList.find(_num);

	// キーが見つからなかったら
	if (itr == navigationNavPointList.end())
		return nullptr;

	if (itr->second == nullptr)
		assert(false);

	return &navigationNavPointList[_num]->GetUseKind();
}

const NODE_BLOCK_STATE* NavigationManager::GetNavPointBlockState(const int& _num)
{
	const std::unordered_map<int, NavPointBase*>::iterator itr = navigationNavPointList.find(_num);

	// キーが見つからなかったら
	if (itr == navigationNavPointList.end())
		return nullptr;

	if (itr->second == nullptr)
		assert(false);

	return &navigationNavPointList[_num]->GetBlockState();
}

const int NavigationManager::GetPositionForNearNavPointNumber(const VECTOR3& _pos, const int& _nodeKind, const int& _mode)
{
	float min		= FLT_MAX;
	int nearNumber	= -1;
	
	// 帰路探索に使えるnavPointを全て回す
	for (auto itr = navigationNavPointList.begin();itr != navigationNavPointList.end();itr++)
	{
		int navNumber			= itr->first;
		NavPointBase& navPoint	= *itr->second;

		// 経路探索で使うノードの種類なかったら
		if (!CanUseNavPoint(_nodeKind, navPoint.GetUseKind()))
			continue;

		// ナビポイントの状態をみたときに経路探索に使えなかったら
		if (!CanUseTypeNavPoint(_mode, navPoint.GetBlockState()))
			continue;

		// 距離を初期化
		float dis		= VSquareSize(navigationNavPointList[navNumber]->GetPosition() - _pos);

		// 最小値のほうが距離よりも値が大きかったら
		if (min > dis)
		{
			// 最小値を記録
			min		= dis;
			// navPointのナンバーを記録
			nearNumber = navNumber;
		}
	}
	assert(nearNumber >= 0 && "ナビゲーションに使えるNavPointが存在しませんでした  ※NavPointをうまく生成できなかった可能性があります");
	return nearNumber;
}

std::vector<int> NavigationManager::GetTargetMoveNavPointContainer(const VECTOR3& _mePos, const VECTOR3& _goalPos, const int& _agentKind, const float& _moveSpeed, const int& _nodeKind, const int& _mode, const float& _dps)
{
#ifdef _DEBUG
	debugCostRateUpNavPointList.clear();
	debugNavPointCostList.clear();
#endif // _DEBUG

	// 経路探索するモードが潰されたノードも含めた経路探索のとき
	if (_mode == (int)NAVIGATION_MODE::IN_BLOCKED)
	{
		// DPSが0以下だったら
		if (_dps <= 0.0f)
			assert(false && "DPSの値が不正です");	// 潰されたノードも含めた経路探索のときDPSが0以下だと、遮断物を排除するコスト計算ができないので、assertを実行
	}

	std::vector<int> saveRoot;	// 進むべきルート
	// ルートの初期化
	saveRoot.clear();

	int nowNum = GetPositionForNearNavPointNumber(_mePos, _nodeKind, _mode);		// スタートのナンバー

	// キーが存在しなかったらassertを実行
	HasUnorderedMapContainerKey(navigationNavPointList, nowNum, true);

	//__ ゴール __//
	
	int goalNumber = GetPositionForNearNavPointNumber(_goalPos, _nodeKind, _mode);	// ゴールのナンバー
	// ゴールしたら
	if (goalNumber == nowNum)
		return saveRoot;		// ゴールしているのでreturn

	//__ 経路探索 __//

	//_ Stateとコストの初期化 _//

	for (auto itr = navigationNavPointList.begin();itr != navigationNavPointList.end();itr++)
	{
		NavPointBase& nav	= *itr->second;
		int number			= nav.GetGlobalNumber();

		nav.SetState(NODE_STATE::NONE);
		nav.SetCost({ FLT_MAX, FLT_MAX });
		
		// ノードの種類に応じた、コスト倍率を設定
		nav.SetCostRate(NodeCostRateInfo::GetNodeUseKindCostRate(nav.GetUseKind()));

		// DPSが0以下だったら
		if (_dps <= 0.0f)
			continue;	// 潰されたナビポイントのコスト処理を行わない

		// 潰されたナビポイントだったら
		if (nav.GetBlockState() == NODE_BLOCK_STATE::BLOCKED)
		{
			// keyが存在しなかったらassertを実行
			HasUnorderedMapContainerKey(voxelNavList, number, true);

			// コスト倍率を設定
			nav.SetCostRate(NodeCostRateInfo::GetBlockedCostRate(_moveSpeed,_dps,voxelNavList[number].hp));
		}
	}

	//_ 現在地点のポイントの状態をOpenにし、コストを初期化設定 _//

	std::priority_queue<OpenNode> openList;	// 大きい順で値をpushするコンテナ　今回はscoreが小さい順がいいので、OpenNodeで＜演算子をオーバーロードして逆にしている
	// 状態をOpen
	navigationNavPointList[nowNum]->SetState(NODE_STATE::OPEN);	
	// コストを設定
	navigationNavPointList[nowNum]->SetCost({ 0.0f, VSize(navigationNavPointList[nowNum]->GetPosition() - navigationNavPointList[goalNumber]->GetPosition()) });
	// openListにpush (先頭)
	openList.push({ nowNum, 0.0f });

	// 経路の記録
	std::unordered_map<int, int> cameFrom;

	// Openになってるポータルポイント分回す
	while (!openList.empty())
	{
		// 一番古い（先頭）の要素をゲットし、OpenLsitから削除
		int current			= openList.top().num;
		openList.pop();

		// キーが存在しなかったらassertを実行
		HasUnorderedMapContainerKey(navigationNavPointList, current, true);

		// ゴールまでたどり着いたら
		if (goalNumber == current)
			break;	// 経路探索の終了

		NavPointBase& navMe	= *navigationNavPointList[current];

		if (!CanUseNavPoint(_nodeKind, navMe.GetUseKind()))
			continue;	// 経路探索で使うノードの種類なかったら

		if (!CanUseTypeNavPoint(_mode, navMe.GetBlockState()))
			continue;	// ナビポイントの状態をみたときに経路探索に使えなかったら

		// CLOSEにし確認した判定
		navMe.SetState(NODE_STATE::CLOSE);

		//_ 自身が持ってる相手NavPointをみて次の行先を決める _//

		const std::unordered_map<NavPointBase*, TargetData>& trCon = navMe.GetTargetContainer();

		for (auto itr = trCon.begin();itr != trCon.end();itr++)
		{
#if VOXEL_USE
			// 接続相手がオブジェクトに潰されて使用できない状態だったら
			/*if (itr->second.blockState == NODE_BLOCK_STATE::BLOCKED)
			{
				itr->first->GetBlockedOwnerVoxelKey();
			}*/
#else
			// 接続相手がオブジェクトに潰されて使用できない状態だったら
			if (itr->second.blockState == NODE_BLOCK_STATE::BLOCKED)
				continue;
#endif

			NavPointBase& navTr		= *(itr->first);
			int trNum				= navTr.GetGlobalNumber();

			if (navTr.GetState() == NODE_STATE::CLOSE)
				continue;	// 確認済みは処理を飛ばす

			if (!CanUseNavPoint(_nodeKind, navTr.GetUseKind()))
				continue;	// 経路探索で使うノードの種類でなかったら
			
			if (!CanUseTypeNavPoint(_mode, navTr.GetBlockState()))
				continue;	// ナビポイントの状態をみたときに経路探索に使えなかったら

			if (!HasUnorderedMapContainerKey(navigationNavPointList, trNum))
				continue;	// ポータルポイントがコンテナから移動していたら

#ifdef _DEBUG

			debugNavCountList[navMe.GetGlobalNumber()].emplace_back(navTr.GetGlobalNumber());
#endif // _DEBUG


			//_ 接続関係の移動コスト倍率の設定　接続の線上に遮断物があった時 / VoxelWayPoint（BLOCKED）の場合、倍率を高くする _//
			
			float stepCostRate		= 0.0f;			// 移動コスト倍率
			bool isSetStepCostRate	= false;			// 移動コスト倍率が設定できたかどうか

			// ノードの種類に応じた、コスト倍率を設定
			//stepCostRate			+= NodeCostRateInfo::GetNodeUseKindCostRate(navTr.GetUseKind());

			// 遮蔽物を壊す攻撃力が設定されていたら
			if (_dps >= 0.0f)
			{
				int voxelOwnerKey						= navTr.GetBlockedOwnerVoxelKey();			// navTrがVoxelWayPointだったときのボクセルキー　潰されていなかったら-1
				const std::set<int>& voxelOwnerKeyCon	= itr->second.GetVoxelOwnerKeyContainer();	// 接続関係の線上に遮断物があった時の、遮断しているボクセルキーコンテナ

				for (const auto& voxKey : voxelOwnerKeyCon)
				{
					if (voxKey == voxelOwnerKey)
						continue;	// navTrがVoxelWayPointで、voxKeyがそのオーナーのボクセルキーだった場合、上記でコスト倍率を設定しているのでcontinue
					
					if (voxelNavList.find(voxKey) == voxelNavList.end())
						continue;	// キーが見つからない場合はcontinue

					// 移動コスト倍率の設定
					stepCostRate += NodeCostRateInfo::GetBlockedCostRate(_moveSpeed, _dps, voxelNavList[voxKey].hp);

					// 移動コストの設定ができたので、フラグをtrue
					isSetStepCostRate = true;

#if 0
					// 自身と接続相手がブロック状態のときこのノードのコスト倍率を上げる
					if (navMe.GetBlockState() == NODE_BLOCK_STATE::BLOCKED)
					{
						// 移動コスト倍率の設定
						stepCostRate += NodeCostRateInfo::GetBlockedCostRate(_agentKind, voxelNavList[voxKey].hp);

						// 移動コストの設定ができたので、フラグをtrue
						isSetStepCostRate = true;
					}
#else
					//if (itr->second.blockState == NODE_BLOCK_STATE::BLOCKED)
					//{
					//	// 移動コスト倍率の設定
					//	stepCostRate += NodeCostRateInfo::GetBlockedCostRate(_agentKind, voxelNavList[voxKey].hp);

					//	// 移動コストの設定ができたので、フラグをtrue
					//	isSetStepCostRate = true;
					//}
#endif
				}
			}

			// 移動コスト倍率の設定がされていなかったら
			if (!isSetStepCostRate)
			{
				stepCostRate = navTr.GetCostRate().costRate;
			}
			float stepCost	= VSize(navTr.GetPosition() - navMe.GetPosition()) * stepCostRate;	// 相手までの移動コスト
			float costG		= navMe.GetCost().g + stepCost;										// Gコスト 今までの実コスト
			float costH		= VSize(navTr.GetPosition() - _goalPos);							// Hコスト これからの推定コスト

#ifdef _DEBUG
			//debugNavPointCostList[navTr.GetGlobalNumber()].emplace_back(Cost(costG, costH));
			debugCostRateUpNavPointList[&navMe].emplace(&navTr);

#endif // _DEBUG

			//			   	   追加					 or			更新
			if (navTr.GetState() == NODE_STATE::NONE || costG < navTr.GetCost().g)/*costG < navTr.GetCost().g*/
			{
				// 経路の記録
				cameFrom[trNum] = current;

				// コストの設定 //
				Cost c	= Cost();
				c.g		= costG;
				c.h		= costH;	// VSize(tr.GetPosition() - wayPointList[goalWayPointNumber]->GetPosition());
				navTr.SetCost(c);

#ifdef _DEBUG
				debugNavPointCostList[navTr.GetGlobalNumber()].clear();
				debugNavPointCostList[navTr.GetGlobalNumber()].emplace_back(c);
#endif // _DEBUG

				// OpenListに追加
				if (navTr.GetState() == NODE_STATE::NONE)
				{
#ifdef _DEBUG
					// デバッグでオープンされたナビポイントナンバーを記録
					debugOpenedNavPointList.insert(trNum);
#endif // _DEBUG
					// 確認した状態にする
					navTr.SetState(NODE_STATE::OPEN);
				}
				// 確認したナビポイントのナンバーとコストをpush
				openList.push({ trNum, c.g + c.h });
			}
		}
	}

	int currentNum = goalNumber;	// 現在のルート値　ゴール値で初期化

	// mapのゴール値からたどっていく、個々の要素が次のルート値を持っているでたどる
	while (cameFrom.find(currentNum) != cameFrom.end()) // GOAL　->　START  
	{
		saveRoot.emplace_back(currentNum);
		// 1つ前のノードに戻る
		currentNum = cameFrom[currentNum];
	}
	// スタート地点を最後に入れて
	saveRoot.emplace_back(nowNum);
	// 順番をSTART -> GOALにする
	std::reverse(saveRoot.begin(), saveRoot.end());

	return saveRoot;
}

void NavigationManager::Update()
{
	//__ デバッグ __//

	if (Debug::GetDebugMode())
	{
		ImGui::Begin("NavPointDraw");

		ImGui::Text("AllNavPoint         %d", (int)activeNavPointList.size() + (int)inactiveNavPointList.size());

		ImGui::Text("ActiveNavPoint	%d", (int)activeNavPointList.size());
		if (ImGui::TreeNode("ActiveNavKind"))
		{
			// NODE_USE_KINDに属する種類ごとの総数を描画
			ImGuiDrawInContainerNodeUseKindNum(GetInContainerNodeUseKindNum(activeNavPointList));

			ImGui::TreePop();
		}

		ImGui::Text("InactiveNavPoint %d", (int)inactiveNavPointList.size());
		if (ImGui::TreeNode("InactiveNavKind"))
		{
			// NODE_USE_KINDに属する種類ごとの総数を描画
			ImGuiDrawInContainerNodeUseKindNum(GetInContainerNodeUseKindNum(inactiveNavPointList));

			ImGui::TreePop();
		}
		if (ImGui::Button("NavMeshDraw"))
			isNavPointRootDraw = !isNavPointRootDraw;

		ImGui::Text("NODE_USE_DRAW");

		if (ImGui::RadioButton("NORMAL", drawRootKind == 0))
			drawRootKind = 0;
		ImGui::SameLine();

		if (ImGui::RadioButton("WALL", drawRootKind == 1))
			drawRootKind = 1;
		ImGui::SameLine();

		if (ImGui::RadioButton("ALL", drawRootKind == 2))
			drawRootKind = 2;

		if (ImGui::Button("NavPointDraw"))
			isNavPointDraw = !isNavPointDraw;

		ImGui::End();
	}

	// ステージが変化しナビゲーションの当たり判定が再設定されたら　NavPointが追加削除されたら
	if (IsChangedNavPointContainer())
	{
		// 敵のナビゲーション情報をリロード
		enemyManager->EnemyNavigationReload();
		isChangedContainer = false;
	}
}

void NavigationManager::Draw()
{
	VECTOR3 addPos = VGet(0.0f, 0.0f, 0.0f);	// 描画用加算座標  モデルにラインの描画などが埋もれてしまうのを防ぐ

	//for (const auto& nav : debugNavgationList)
	//{
	//	for (const auto& tar : nav.second)
	//	{
	//		VECTOR3 pPos	= navigationNavPointList[nav.first]->GetPosition();
	//		VECTOR3 pTrPos	= navigationNavPointList[tar]->GetPosition();

	//		DrawLine3D(pPos + addPos, pTrPos + addPos, 0xff00ff);
	//	}
	//}

	//int navCostCn = 0;
	//int navCostMax = (int)debugNavPointCostList.size();
	//for (const auto& navCost : debugNavPointCostList)
	//{
	//	navCostCn++;
	//	if (navigationNavPointList.find(navCost.first) == navigationNavPointList.end())
	//		continue;

	//	int colorNum = (int)(((float)navCostCn / navCostMax) * 255);
	//	int color = GetColor(colorNum, colorNum, colorNum);

	//	for (const auto& cost : navCost.second)
	//	{
	//		VECTOR3 bottomPos = navigationNavPointList[navCost.first]->GetPosition();
	//		VECTOR3 topPos = bottomPos + VECTOR3(0, 1, 0) * (cost.g / 10);

	//		//DrawLine3D(bottomPos, topPos, 0xff00ff);
	//		DrawCube3D(topPos + VOne * 5.0f, topPos - VOne * 5.0f, color, 0xffffff, true);
	//	}
	//}

	//for (const auto& nav : navCountList)
	//{
	//	if (navigationNavPointList.find(nav.first) == navigationNavPointList.end())
	//		continue;

	//	float dis = 250.0f;
	//	int cn = 0;

	//	if (nav.second.size() > 1)
	//		int s = 0;

	//	for (const auto& count : nav.second)
	//	{
	//		if (navigationNavPointList.find(count) == navigationNavPointList.end())
	//			continue;

	//		float addPosY = dis * cn++;

	//		VECTOR3 pos = navigationNavPointList[count]->GetPosition();
	//		pos.y += addPosY;
	//		DrawSphere3D(pos, 20.0f, 2, 0x5555ff, 0xffffff, false);
	//	}
	//}

	//for (const auto& nav : debugCostRateUpNavPointList)
	//{
	//	for (const auto& tar : nav.second)
	//	{
	//		VECTOR3 debugAddPos = VECTOR3(0.0f, 100.0f, 0.0f);
	//		VECTOR3 bottom		= nav.first->GetPosition() + debugAddPos;
	//		VECTOR3 top			= tar->GetPosition() + debugAddPos;

	//		//DrawLine3D(bottom, top, 0xff00ff);

	//		DrawCone3D(top, bottom, 30.0f, 16, 0xff00ff, 0xffffff, false);
	//	}
	//}

	
	/*for (const auto& opened : debugOpenedNavPointList)
	{
		if (navigationNavPointList.find(opened) == navigationNavPointList.end())
			continue;

		VECTOR3 pos = navigationNavPointList[opened]->GetPosition();
		DrawLine3D(pos, pos + VECTOR3(0, 1000, 0), 0xff00ff);
	}*/
	
	for (const auto& nItr : navigationNavPointList)
	{
		NavPointBase& nMe	= *nItr.second;
		VECTOR3 mePos		= nMe.GetPosition();

		//_ コストの描画 _//

		//if (nMe.GetState() != NODE_STATE::NONE)
		//{
		//	// Yプラス方向にコスト分長い線を描画
		//	DrawLine3D(mePos, mePos + VECTOR3(0, nMe.GetCost().g, 0), 0xff00ff);
		//}

		//_ ポータルポイントの描画 _//
		
		if (isNavPointDraw)
		{
			if (nMe.GetBlockState() == NODE_BLOCK_STATE::BLOCKED)
			{
				if (voxelNavList.find(nMe.GetGlobalNumber()) == voxelNavList.end())
					assert(false);

				// コスト倍率を設定
				nMe.SetCostRate(NodeCostRateInfo::GetBlockedCostRate(100.0f,10.0f,voxelNavList[nMe.GetGlobalNumber()].hp));
			}

			float costRate			= nMe.GetCostRate().costRate;

			int navColor	= 0x00ff00;
			if (costRate > 1.0f)
				navColor	= 0x0000ff;

			// ポータルポイントを球で描画
			DrawSphere3D(mePos, 20.0f, 2, navColor, 0xffffff, false);
		}

		//_ ポータルポイント同士を結んだ線の描画 _//

		if (!isNavPointRootDraw)
			continue;

		const std::unordered_map<NavPointBase*, TargetData>& nTrCon = nMe.GetTargetContainer();

		int rootColor = GetColor(255, 255, 255);

		// 接続相手ポータル分回す
		for (const auto& nTrItr : nTrCon)
		{
			NavPointBase& nTr		= *nTrItr.first;
			int pTrNum				= nTr.GetGlobalNumber();						// 接続相手のナンバー

			if (!HasUnorderedMapContainerKey(navigationNavPointList, pTrNum))
				continue;	// キーが見つからなかったので、continue

			int voxelOwnerKey = nTr.GetBlockedOwnerVoxelKey();									// navTrがVoxelWayPointだったときのボクセルキー　潰されていなかったら-1
			const std::set<int>& voxelOwnerKeyCon = nTrItr.second.GetVoxelOwnerKeyContainer();	// 接続関係の線上に遮断物があった時の、遮断しているボクセルキーコンテナ

			bool isSetStepCostRate = false;			// 移動コスト倍率が設定できたかどうか


			for (const auto& voxKey : voxelOwnerKeyCon)
			{
				// navTrがVoxelWayPointで、voxKeyがそのオーナーのボクセルキーだった場合、上記でコスト倍率を設定しているのでcontinue
				//if (voxKey == voxelOwnerKey)
				//	continue;

				if (voxelNavList.find(voxKey) == voxelNavList.end())
					continue;	// キーが見つからない場合はcontinue

				if (nTrItr.second.blockState == NODE_BLOCK_STATE::BLOCKED)
				{
					isSetStepCostRate = true;
				}
#ifdef _DEBUG
				//debugCostRateUpNavPointList[&navMe].emplace(&navTr);
#endif // _DEBUG
			}

			VECTOR3 trPos			= navigationNavPointList[pTrNum]->GetPosition();// 接続相手の座標
			NODE_USE_KIND trUseKind	= nTr.GetUseKind();								// 接続相手のノード使用用途種類

			// すべて描画する場合でないとき、描画するルートでなかったら
			if (drawRootKind != (int)NODE_USE_KIND::ALL && drawRootKind != (int)nTr.GetUseKind())
				continue;

			// ノード使用用途種類が同じでなかったら
			//if (nMe.GetUseKind() != trUseKind)
			//	continue;

			switch (trUseKind)
			{
			case NODE_USE_KIND::NORMAL:

				rootColor = 0x00ffff;
				break;
			case NODE_USE_KIND::WALL:

				rootColor = 0x0066ff;
				break;
			default:

				assert(false);
				break;
			}

			VECTOR3 drawLineMePos = mePos + addPos;	// ナビメッシュ関連の描画
			VECTOR3 drawLineTrPos = trPos + addPos;	// ナビメッシュ関連の描画

			// 自身がブロック状態だったら
			if (nMe.GetBlockState() == NODE_BLOCK_STATE::BLOCKED)
			{
				//continue;
				rootColor = 0xff0000;

				// Yプラス方向に線を描画
				DrawLine3D(drawLineMePos, drawLineMePos + VECTOR3(0.0f, 150.0f, 0.0f), 0xff00ff);
			}

			// 接続相手のエッジ線状に遮断物がある状態だったら
			if (nTrItr.second.blockState == NODE_BLOCK_STATE::BLOCKED)
			{
				rootColor = 0xff0000;

				// Yプラス方向に線を描画
				//DrawLine3D(drawLineTrPos, drawLineTrPos + VECTOR3(0.0f,150.0f,0.0f), 0xff00ff);
			}

			if (isSetStepCostRate)
				rootColor = 0xff0000;

			//_ ルートの描画 _//

			if (rootColor == 0xff0000)
			{
				DrawLine3D(drawLineMePos + VECTOR3(0.0f, 10.0f, 0.0f), drawLineTrPos + VECTOR3(0.0f, 10.0f, 0.0f), rootColor);
			}
			else
			{
				DrawLine3D(drawLineMePos, drawLineTrPos, rootColor);
			}
		}
	}
}

std::unordered_map<int, NavPointBase*>::iterator NavigationManager::ActiveForInactivePushNavPoint(const int& _num)
{
	std::unordered_map<int, NavPointBase*>::iterator navItr = navigationNavPointList.find(_num);

	// キーが見つからなかったら
	if (navItr == navigationNavPointList.end())
		return navItr;

	NavPointBase* por = navItr->second;

	// 使えない状態のポータルポイントとしてinactiveコンテナに追加
	std::pair<std::unordered_map<int, NavPointBase*>::iterator, bool> pair = 
		inactiveNavPointList.insert(std::make_pair(_num, por));

	navItr = navigationNavPointList.erase(navItr);
	
	if (!pair.second)
		assert(false && "すでに同じキーが登録されています");

	isChangedContainer = true;

	return navItr;
}

std::unordered_map<int, NavPointBase*>::iterator NavigationManager::InactiveForActivePushNavPoint(const int& _num)
{
	auto acItr		= navigationNavPointList.find(_num);

	// キーが見つかったら
	if (acItr != navigationNavPointList.end())
		return inactiveNavPointList.end();

	auto inacItr	= inactiveNavPointList.find(_num);

	// キーが見つからなかったら
	if (inacItr == inactiveNavPointList.end())
		assert(false);

	// 使える状態のポータルポイントとしてactiveコンテナに追加
	navigationNavPointList.insert(std::make_pair(inacItr->first, inacItr->second));

	// inactiveコンテナから削除
	inacItr				= inactiveNavPointList.erase(inacItr);

	isChangedContainer	= true;

	return inacItr;
}

int NavigationManager::StageObjectPutAreaBox(const VECTOR3& _startPos, const VECTOR3& _endPos, const int& _hp)
{
	return navPointUpdater->StageObjectPutAreaBox(_startPos, _endPos,_hp);
}

void NavigationManager::StageObjectClearAreaBox(const VECTOR3& _startPos, const VECTOR3& _endPos)
{
	navPointUpdater->StageObjectClearAreaBox(_startPos, _endPos);
}

int NavigationManager::PushVoxelNavNode(std::unordered_map<int, std::list<int>>& _navCon, const NavPointUpdater::AreaData& _voxel)
{
	int voxelCreateNum				= navPointCreateNum;
	VoxelNavNodeData voxelNavNode	= VoxelNavNodeData(_navCon,_voxel.transform,_voxel.stageObjectHp);

	//--デバッグ--
	debugVoxelList.emplace(voxelCreateNum);
	//------------

	// ボクセル
	voxelNavList.insert(std::make_pair(voxelCreateNum, voxelNavNode));
	navPointCreateNum++;

	VoxelNavNodeData& nowNewVoxel	= voxelNavList[voxelCreateNum];	// 今生成されたボクセル

	//_ ボクセル同士の接続相手の設定 _// ToDo エリア分けの導入ができたら重いfor文をやめてエリアインデクスで対応
	
	if (voxelNavList.size() > 1)
	{
		// 存在する接続相手分回す
		for (auto& targetVoxel : voxelNavList)
		{
			if (voxelCreateNum == targetVoxel.first)
				continue;	// 自身だったらcontinue

			// お互いの接続をリクエストする
			RequestConnectVoxelNavNode(nowNewVoxel, targetVoxel.second);
		}
	}

	//_ ボクセルを一つのナビポイントとしてnewしnavigationNavPointListにpush _//

	std::unordered_map<NavPointBase*, int> navTargetList;	// 接続相手情報コンテナ　key:接続相手ナビポイントポインタ / value:接続相手の接続の種類(TARGET_CONNECT_KIND)int値
	VECTOR3 blockedNavSumPos;								// 潰されたナビポイントの平均座標を算出するための、潰されたナビポイント座標の合計

	for (const auto& blockedNav : nowNewVoxel.blockedNavPointList)
	{
		if (!HasUnorderedMapContainerKey(navigationNavPointList, blockedNav.first))
			continue;	// すでに潰されてコンテナ内に存在しなかったら continue

		NavPointBase* navblocked = navigationNavPointList[blockedNav.first];	// 潰されたナビポイント
		
		if (navblocked == nullptr)
		{
			assert(false);
			continue;
		}

		// 座標の合計を記録
		blockedNavSumPos += navblocked->GetPosition();

		for (const auto& targetNum : blockedNav.second)
		{
			if (!HasUnorderedMapContainerKey(navigationNavPointList, targetNum))
				continue;	// すでに潰されていたらcontinue

			NavPointBase* navTarget			= navigationNavPointList[targetNum];// 潰されたナビポイントの接続相手

			assert(navTarget != nullptr);

			const TargetData* targetData	= navTarget->GetTargetData(navblocked);

			assert(targetData != nullptr);

			int targetConnectKind			= (int)targetData->kind;			// 接続相手の接続の種類(TARGET_CONNECT_KIND)

			// コンテナにpush
			navTargetList.insert(std::make_pair(navTarget, targetConnectKind));
		}
	}

	// 平均座標としてボクセルの底面座標を代入
	nowNewVoxel.voxelBottomPosition = VDivI(blockedNavSumPos, (int)nowNewVoxel.blockedNavPointList.size());

	// VoxelWayPointをウェイポイントコントローラーでnewする
	NavPointBase* wayPoint = wayPointController->PushWayPoint(voxelCreateNum, navTargetList, nowNewVoxel.voxelBottomPosition,NAV_POINT_KIND::VOXEL_WAY_POINT);
	
	// 自身VoxelWayPointが潰されているということを設定
	wayPoint->SetBlockState(NODE_BLOCK_STATE::BLOCKED);
	
	// コスト倍率を設定
	//wayPoint->SetCostRateNum(NodeCostRateInfo::GetBlockedCostRate(_voxel.stageObjectHp));

	assert(wayPoint != nullptr);

	//_ VoxelWayPointの接続相手のナビポイントの接続設定 _//

	for (const auto& wayPointTarget : wayPoint->GetTargetContainer())
	{
		int wayPointTargetNum = wayPointTarget.first->GetGlobalNumber();

		if (navigationNavPointList.find(wayPointTargetNum) == navigationNavPointList.end())
			assert(false);

		// 今生成したVoxelWayPointを接続相手としてナビポイントに設定する
		navigationNavPointList[wayPointTargetNum]->PushTarget(wayPoint, wayPointTarget.second.kind);
	}

	// navigationNavPointListにVoxelWayPointをpush
	navigationNavPointList.insert(std::make_pair(voxelCreateNum, wayPoint));

	for (const auto& blockedNav : _navCon)
	{
		// 潰されたナビポイントをactiveからinactiveに移動
		ActiveForInactivePushNavPoint(blockedNav.first);
	}

	return voxelCreateNum;
}

void NavigationManager::RequestConnectVoxelNavNode(VoxelNavNodeData& _me, VoxelNavNodeData& _target)
{
	for (const auto& meblocked : _me.blockedNavPointList)
	{
		int meBlockedNum = meblocked.first;		// 自身の潰されたナビポイントナンバー

		for (const auto& targetblocked : _target.blockedNavPointList)
		{
			for (const auto& targetBlockedTargetNum : targetblocked.second)
			{
				if (meBlockedNum != targetBlockedTargetNum)
					continue;	// 接続相手としてお互いのナビポイントが登録されていないので、continue

				// お互いにVoxelNavNodeDataの接続相手を追加 //
				_me.PushTarget(targetBlockedTargetNum);
				_target.PushTarget(meBlockedNum);

				return;	// 接続ができたので、return
			}
		}
	}
}

void NavigationManager::DeleteVoxelInNavNode(const int& _key, const int& _num)
{
	std::unordered_map<int, VoxelNavNodeData>::iterator voxelItr = voxelNavList.find(_key);

	if (voxelItr == voxelNavList.end())
		assert(false);

	VoxelNavNodeData& voxelData = voxelNavList[_key];

	//_ 潰された(非アクティブ)ナビポイントのアクティブ化 _//

	for (auto blockedNavItr = voxelData.blockedNavPointList.begin();blockedNavItr != voxelData.blockedNavPointList.end();)
	{
		if (_num == blockedNavItr->first)
		{
			// つぶされなくなったナビポイントなのでinactiveからactiveに移動 
			InactiveForActivePushNavPoint(blockedNavItr->first);

			// ボクセルのオーナーkeyをリセット
			navigationNavPointList[blockedNavItr->first]->ResetBlockOwnerVoxelKey();

			// 潰された状態をリセット
			navigationNavPointList[blockedNavItr->first]->SetBlockState(NODE_BLOCK_STATE::FREE);

			// ボクセルからナビポイントを削除する
			blockedNavItr = voxelData.blockedNavPointList.erase(blockedNavItr);
			break;	// 削除したので break
		}
		else
		{
			blockedNavItr++;
		}
	}

	if (!voxelData.blockedNavPointList.empty())
		return;	// ボクセルの潰されたナビポイント情報が空でなかったら return

#ifdef _DEBUG
	debugDeletedVoxelWayPointNumber.emplace(_key);

#endif // _DEBUG

	
	// ボクセルを削除
	voxelItr = voxelNavList.erase(voxelItr);

	//_ navigationNavPointListとウェイポイントコントローラーからボクセルのVoxelWayPointの削除 _//

	std::unordered_map<int, NavPointBase*>::iterator navItr = navigationNavPointList.find(_key);

	if (navItr == navigationNavPointList.end())
		assert(false);

	//_ VoxelWayPointの接続相手からVoxelWayPointとの接続をやめてからVoxelWayPoint自身の削除処理をする　nullptrを避ける _//
	
	// VoxelWayPointの接続相手からVoxelWayPointとの接続をやめる
	DeleteNavPointToTargetDisconnect(navigationNavPointList[_key]);
	// VoxelWayPoint自身の削除処理をする
	wayPointController->DeleteWayPoint(_key);

	// ボクセルのナビポイントを削除
	navItr = navigationNavPointList.erase(navItr);
}

void NavigationManager::VoxelNavNodeHPChange(const int& _key, const int& _hp)
{
	if (HasUnorderedMapContainerKey(voxelNavList,_key))
		return;	// キーが存在しなかったら return

	// ボクセルのHPの設定
	voxelNavList[_key].hp = _hp;
}

void NavigationManager::DeleteNavPointToTargetDisconnect(NavPointBase* _deleteNavPoint)
{
	//_ 削除される予定のナビポイントの接続相手だったナビポイントから引数の削除される予定のナビポイントとの接続をやめる _//

	for (const auto& deleteNavTargetNum : _deleteNavPoint->GetTargetNumberContainer())
	{
		// ToDo　潰されているかの状態を見てfindを削減する  ナビポイントにactive状態を付与した方がいいかも   inactive , navigationの判断が楽

		// navigationNavPointListになかったら
		if (navigationNavPointList.find(deleteNavTargetNum) == navigationNavPointList.end())
		{
			HasUnorderedMapContainerKey(inactiveNavPointList, deleteNavTargetNum, true);

			// 接続をやめる
			inactiveNavPointList[deleteNavTargetNum]->DeleteTarget(_deleteNavPoint);
			continue;	// 接続を解除できたので、continue
		}
		// 接続をやめる
		navigationNavPointList[deleteNavTargetNum]->DeleteTarget(_deleteNavPoint);
	}
}

// ToDo ビットシフトに対応した処理に変更　現在intの引数は一つの値しか対応できない
bool NavigationManager::CanUseNavPoint(const int& _useKind, const NODE_USE_KIND& _checkKind)
{
	// ノードの使用用途の種類がすべてだったら無条件でtrue
	if (_useKind == (int)NODE_USE_KIND::ALL)
		return true;

	// ノードの使用用途の種類が一致していたら
	if (_useKind == (int)_checkKind)
		return true;
	return false;
}

bool NavigationManager::CanUseTypeNavPoint(const int& _mode, const NODE_BLOCK_STATE& _checkMode)
{
	// 潰されたノードも含めた経路探索のモードだったら	このモードの場合無条件でtrueを返す
	if (_mode == (int)NAVIGATION_MODE::IN_BLOCKED)
		return true;

	// ブロック状態が一致していたら
	if (_mode == (int)_checkMode)
		return true;
	return false;
}

std::unordered_map<int, int> NavigationManager::GetInContainerNodeUseKindNum(const std::unordered_map<int, NavPointBase*>& _con)
{
	// NODE_USE_KINDの種類ごとのナビポイント総数を記録するコンテナ
	// key:NODE_USE_KIND / value:総数
	std::unordered_map<int, int> sumNumCon;	
	
	for (int i = 0;i < (int)NODE_USE_KIND::ALL;i++)
	{
		// キーの初期化
		sumNumCon[i];
	}

	for (auto itr = _con.begin();itr != _con.end();itr++)
	{
		int useKind = (int)itr->second->GetUseKind();

		sumNumCon[useKind]++;
	}
	return sumNumCon;
}

void NavigationManager::ImGuiDrawInContainerNodeUseKindNum(const std::unordered_map<int, int>& _con)
{
	std::string kindStr[] =
	{
		"NORMAL",
		"WALL",
		"ALL",
	};

	for (int i = 0;i < (int)NODE_USE_KIND::ALL;i++)
	{
		if (_con.find(i) == _con.end())
			continue;	// キー(特定のNODE_USE_KIND)が存在しなかったら continue

		ImGui::Text("%s %d", kindStr[i].c_str(), _con.at(i));
	}
}

