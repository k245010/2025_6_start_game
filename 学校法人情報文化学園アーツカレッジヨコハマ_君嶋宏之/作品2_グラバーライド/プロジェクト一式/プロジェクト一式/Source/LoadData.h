#pragma once
#include "struct.h"
#include <vector>
#include "function.h"
#include "rgb.h"
#include <string>
/// <summary>
/// データを読み込むのに必要な情報を纏めておく場所
/// </summary>
namespace ResoData {
	//パスショートカット
	const std::string RESO_TE =	 "data/texture/";
	const std::string RESO_MO =	 "data/model/";
	const std::string RESO_SO =  "data/sound/";
	const std::string RESO_EFF = "data/effect/";
	const std::string RESO_ANI ="data/model/animation/";

	//sceneを選択してするようにする
	//読み込み用データの纏め場所
	struct LoadData {
		std::string key;//呼び出し用の鍵
		std::string path;//ファイルパス
		std::string scene;
	};


	struct EffectLoadData {
		LoadData data;
		float rate;

	};
	struct SoundData {
		std::string key;
		LoadData data;
		int volume;

	};
	const std::vector<EffectLoadData> EFFECT{
		{{{"CHARGE_START_EFFECT"},{"gravityReady.efkefc"},{"common"}},{100.0f}},
		{{{"SMOKE"},{"smoke.efk"},{"common"}},{10.0f} },
		{{{"ENEMY_HIT"},{"hit.efkefc"},{"common"}},{100.0f} },
		{{{"ENEMY_DESTROY_HIT_EFFECT"},{"hit2.efkefc"},{"common"}},{100.0f} },
		{{{"JUMP"},{"jumpSmoke.efkefc"},{"common"}},{120.0f} },
		{{{"ENERGYWAVE"},{"energyWave.efk"},{"common"}},{3.0f} },
		{{{"PLAYER_DIE_EFFECT"},{"death.efkefc"},{"common"}},{40.0f} },
		{{{"CLEAR"},{"goalEffect.efkefc"},{"common"}},{200.0f} },
		{{{"GOAL"},{"goal12.efkefc"},{"common"}},{40.0f} },
		//{{{"WIND"},{"gravityModeWInd4.efkefc"},{"common"}},{30.0f} },
		{{{"ENEMY_DEATH"},{"enemyDeath.efk"},{"common"}},40.0f},
		{{{"PLAYER_KICK_HIT"},{"kickHit.efkefc"},{"common"}},40.0f},
		{{{"PLAYER_RESPOWN"},{"reSpown.efkefc"},{"common"}},40.0f},
		{{{"PLAYER_HEAL_HP"},{"healHp.efkefc"},{"common"}},200.0f},
		{{{"PLAYER_HEAL_NO"},{"healNo.efkefc"},{"common"}},200.0f},
		{{{"PLAYER_HEAL_G"},{"healG.efkefc"},{"common"}},200.0f},
		{{{"GRAVITY_AREA0"},{"gravityArea1.efkefc"},{"common"}},100.0f },
		{{{"GRAVITY_AREA1"},{"gravityArea2.efkefc"},{"common"}},100.0f },
		//{{{"BREAK_BROCK"},{"brockBreak.efkefc"},{"common"}},120.0f },
		//{{{"NO_LAND"},{"noLand.efkefc"},{"common"}},40.0f },
		{{{"FLASH"},{"coin.efk"},{"common"}},100.0f },
		{{{"LAND"},{"landSmoke.efkefc"},{"common"}},100.0f },
		{{{"BUMPER_BOUND"},{"bound.efkefc"},{"common"}},100.0f },
		{{{"NO_GRAVITY_EFFECT"},{"gravityReady2.efkefc"},{"common"}},100.0f },


	};


	const std::vector<SoundData>SOUND{
		//試し手本
		//{{ "BGM" }, {{"TITLE"},{"titleBgm.wav"}},{100}},
		{{ "SE" }, {{"重力起動"},		   {"segravityBegin.wav"},{"PlayScene"}},{80}},
		{{ "SE" }, {{"重力着地"},		   {"segravityLand.wav"}, {"PlayScene"}},{80}},
		{{ "SE" }, {{"重力移動"},		   {"segravityMove.wav"}, {"PlayScene"}},{200}},
		{{ "SE" }, {{"ジャンプ"},		   {"sejump.wav"}		, {"PlayScene"}},{100}},
		{{ "SE" }, {{"キック開始"},		   {"sekickBegin.wav"},   {"PlayScene"}},{100}},
		{{ "SE" }, {{"キック当たる"},	   {"sekickHit.wav"},     {"PlayScene"}},{100}},
		{{ "SE" }, {{"着地"},			   {"saeland.wav"},		  {"PlayScene"}},{70}},
		{{ "SE" }, {{"歩く"},			   {"sewalk.wav"},		  {"PlayScene"}},{100}},//長い
		{{ "SE" }, {{"風"},				   {"sewind.wav"},		  {"PlayScene"}},{40}},//長い
		{{ "SE" }, {{"プレイヤーダメージ"},{"sePlayerDamage.wav"},{"PlayScene"}},{100}},//長い
		{{ "SE" }, {{"無敵時間"},		   {"senoDamageBlink.wav"},{"PlayScene"}},{100}},//長い
		{{ "SE" }, {{"中間ポイント"},	   {"sepoint.wav"},{"PlayScene"}},{100}},//長い
		{{ "SE" }, {{"ゴール"},	           {"segoal.wav"},{"PlayScene"}},{100}},//長い
		{{ "SE" }, {{"決定"},			   {"seselect.wav"},{"common"}},{100}},//長い
		{{ "SE" }, {{"セレクト"},		   {"seselectAllow.wav"},{"common"}},{100}},
		{{ "SE" }, {{"回復"},			   {"seguageheal.wav"},{"PlayScene"}},{100}},
		{{ "BGM"}, {{"STAGE1"},			   {"bgmstage1Bgm.wav"}},{5}},
		{{ "SE" }, {{"コイン取得"},		   {"secoin.wav"},{"PlayScene"}},{60}},
		{{ "SE" }, {{"ALLコイン取得"},	   {"se_coin_all_get.wav"},{"PlayScene"}},{150}},
		{{ "SE" }, {{"鉄扉始動"},		   {"seirondoormove.wav"},{"PlayScene"}},{100}},
		{{ "SE" }, {{"鉄扉着地"},		   {"seirondoorland.wav"},{"PlayScene"}},{100}},
		{{ "SE" }, {{"跳ね返り"},		   {"sebanper.wav"},{"PlayScene"}},{100}},
		{{ "SE" }, {{"ブロック破壊"},	   {"sebrock_broken.wav"},{"PlayScene"}},{100}},
		{{ "SE" }, {{"足音1"},			   {"sefootstep1.wav"},{"PlayScene"}},{100}},
		{{ "SE" }, {{"足音2"},			   {"sefootstep2.wav"},{"PlayScene"}},{100}},
		{{ "SE" }, {{"足音3"},			   {"sefootstep3.wav"},{"PlayScene"}},{100}},
		{{ "SE" }, {{"ファンファーレ"},	   {"sefanfare.wav"},{"PlayScene"}},{50}},
		{{ "SE" }, {{"カメラ回転"},			{"engin.wav"},{"PlayScene"}},{230}},
		{{ "VC" }, {{"よし"},				{"/vc/vc_good.wav"},{"PlayScene"}},{100}},
		{{ "VC" }, {{"やられ0"},			{"/vc/vc_dead.wav"},{"PlayScene"}},{200}},
		{{ "VC" }, {{"高掛け声"},			{"/vc/vc_hiAttack.wav"},{"PlayScene"}},{100}},
		{{ "VC" }, {{"やられ1"},			{"/vc/vc_hiDamage.wav"},{"PlayScene"}},{120}},
		{{ "VC" }, {{"やられ2"},			{"/vc/vc_miDamage.wav"},{"PlayScene"}},{120}},
		{{ "VC" }, {{"ジャンプ"},			{"/vc/vc_jump.wav"},{"PlayScene"}},{100}},
		{{ "SE" }, {{"テッテレー"},			{"tettere.wav"},{"PlayScene"}},{50}},
		{{ "SE" }, {{"星が光る"},		   {"blight.wav"},{"PlayScene"}},{100}},
		{{ "BGM"}, {{"タイトル音楽"},	   {"bgmTitle.wav"} ,{ "TitleScene" }},{20}},
		{{ "BGM"}, {{"セレクト音楽"},	   {"bgmSelect.wav"} ,{ "SelectScene" }},{20}},
		{{ "BGM"}, {{"リザルト音楽"},	   {"bgmResult.wav"} ,{ "PlayScene" }},{20}},
		{{ "SE" }, {{"フェードアウト"},			{"sefadeOut.wav"},{"common"}},{40}},
		{{ "SE" }, {{"フェードイン"},		   {"sefadeIn.wav"},{"common"}},{40}},
		{{ "SE" }, {{"NO_GRAVITY"},		   {"nogravity.wav"},{"common"}},{80}},
	};


	const std::vector<LoadData> MOTION{
		{{"PLAYER_WALK"},{"p_walk.mv1"},{"PlayScene"}},
		{{"PLAYER_STAY"},{"p_wait.mv1"},{"PlayScene"}},
		{{"PLAYER_JUMP"},{"p_jump.mv1"},{"PlayScene"}},
		{{"PLAYER_DOWN"},{"p_down.mv1"},{"PlayScene"}},
		//{{"PLAYER_WAIT"},{"p_wait.mv1"}},
		{{"PLAYER_DAMAGE"},{"p_damage.mv1"},{"PlayScene"}},
		{{"PLAYER_DIE"},{"p_die.mv1"},{"PlayScene"}},
		{{"PLAYER_LAND"},{"p_land.mv1"},{"PlayScene"}},
		{{"PLAYER_ATTACK"},{"p_attack.mv1"},{"PlayScene"}},
		{{"PLAYER_ZEROGRAVITY"},{"p_zeroGravity.mv1"},{"PlayScene"}},
		{{"PLAYER_GRAVITYMOVE"},{"p_gravityMove.mv1"},{"PlayScene"}},
		{{"PLAYER_GOAL"},{"p_goal.mv1"},{"PlayScene"}},
		{{"PLAYER_RAIL"},{"p_rail.mv1"},{"PlayScene"}},
		//{{"PLAYER_START"},{"p_start.mv1"}},

		{{"ENEMY_NOMAL_WALK"},{"e_nomal_walk.mv1"},{"PlayScene"}},
		{{"ENEMY_NOMAL_TURN"},{"e_nomal_turn.mv1"},{"PlayScene"}},
		{{"ENEMY_FLY_WALK"},{"e_fly_walk.mv1"},{"PlayScene"}},
		{{"SWITCH"},{"s_animation.mv1"},{"PlayScene"}},

	};

	const std::vector<LoadData>TEXTURE{
		{{"NUMBER_TEXT"},{"number.png"},{"common"}},
		{{"NUMBER_TEXT_1"},{"number_white.png"},{"common"}},
		{{"OBJ_BACK"},{"flare_0_0.png"},{"common"}},
		{{"SELECT_BACKGROUND"},{"select_background.jpg"},{"common"}},
		{{"SELECT_PICTURE_FRAME"},{"select_frame.png" },{ "SelectScene" }},
		{{"SELECT_PICTURE"},{"select_picture.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_1"},{"select_picture_1.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_2"},{"select_picture_2.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_3"},{"select_picture_3.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_4"},{"select_picture_4.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_5"},{"select_picture_5.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_6"},{"select_picture_6.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_7"},{"select_picture_7.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_8"},{"select_picture_8.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_9"},{"select_picture_9.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_10"},{"select_picture_10.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_11"},{"select_picture_11.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_12"},{"select_picture_12.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_13"},{"select_picture_13.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_14"},{"select_picture_14.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_15"},{"select_picture_15.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_16"},{"select_picture_16.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_17"},{"select_picture_17.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_18"},{"select_picture_18.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_19"},{"select_picture_19.png"},{"SelectScene"}},
		{{"SELECT_PICTURE_20"},{"select_picture_20.png"},{"SelectScene"}},
		{{"PAUSE_1"},{"poze.png"},{"PlayScene"}},
		{{"PAUSE_2"},{"CheckPointBack.png"},{"PlayScene"}},
		{{"PAUSE_3"},{"Stertback.png"},{"PlayScene"}},
		{{"PAUSE_4"},{"SerectBack.png"},{"PlayScene"}},
		{{"PAUSE_5"},{"senntak.png"},{"PlayScene"}},
		{{"PAUSE_6"},{"stert.png"},{"PlayScene"}},
		{{"PAUSE_7"},{"Back.png"},{"PlayScene"}},
		{{"VISION"},{"visionEffect.png"},{"PlayScene"}},
		{{"STAGE_FONT"},{"stage_font.png"},{"common"}},
		{{"TIME_NUM"},{"timeNum.png"},{"common"}},
		{{"COIN"},{"coin.png"},{"PlayScene"}},
		{{"STAR"},{"star2.png"},{"common"}},
		{{"CROSS"},{"cross.png"},{"PlayScene"}},
		{{"RESULT_1"},{"アセット 11.png"},{"PlayScene"}},
		{{"RESULT_2"},{"アセット 12.png"},{"common"}},
		{{"RESULT_3"},{"アセット 14.png"},{"common"}},
		{{"RESULT_4"},{"アセット 13.png"},{"common"}},
		{{"RESULT_5"},{"result_goselect.png"},{"common"}},
	};

}