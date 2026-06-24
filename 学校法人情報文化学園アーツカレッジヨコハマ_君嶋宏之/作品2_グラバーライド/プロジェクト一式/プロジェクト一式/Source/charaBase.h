#pragma once
#include "Object.h"
#include "anime.h"

typedef Anime<PlayerAnimesionKind> playerAnime;
typedef Anime<EnemyNomalAnimationKind> enemyNomalAnime;
typedef Anime<EnemyFlyAnimationKind> enemyFlyAnime;

class SphereCollision;

class CharaBase : public Object
{
public:

	enum Direction
	{
		RIGHT,
		LEFT,
	};

	enum CharaState
	{
		NORMAL,
		ATTACK_HIT,
		ENEMY_DAMAGE,
		SPICK_DAMAGE,
	};

	CharaBase(SceneBase* _scene);
	virtual ~CharaBase();
	void Update()override;
	void Draw() override;

	MATRIX GetMatrix()override;

	void GravityMove(GravityState _state)override;

	void DirectionChange(float _rotate, Direction _dire);

	bool Damage(float _damage, int _invencible);

	int GetHp() { return hp; }
	bool noHp() { return (hp <= 0); }//Hpの判定

	VECTOR MoveTransform(VECTOR _velo, VECTOR _addPos);

	virtual MATRIX MoveRotateMatrix();

	/// <summary>
	/// この関数を呼んで移動すると移動周りの処理が全部詰まっているのでこれを使うことをおすすめする。
	/// velocityに代入する処理、centerPositionに代入する処理、移動用の角度に対するVtransformの機能が入っているので
	/// これを使うとこれらの処理が自動で行われる。
	/// 移動用の角度のセットを忘れると変な方向に移動してしまうので、しっかり移動用の角度に値を入れることを忘れずに
	/// </summary>
	/// <param name="_addPos">移動する値</param>

	void PositionMove(VECTOR _addPos);

	VECTOR GetAttackPosition() { return attackPosition; }

	void CollisionForSetCenterPosition(VECTOR set) { 
		centerPosition = set;
		PositionSetDraw();
	}
	void PositionSetDraw();
	VECTOR GetCenterPosition() { return centerPosition; }//当たり判定用
	VECTOR GetHeadPosition() { return headPosition; }

	void CharaRotationSet();
	//VECTOR GetMoveRotate() { return moveRotate; }

	virtual void AnimeInit();

	void CollisionForSetScale(VECTOR _vector) { scale = _vector; }
	VECTOR GetMoveRotate() { return moveRotate; }
	Direction GetDirection() { return direction; }
	VECTOR GetLandPosition() { return landPosition; }

protected:

	Direction direction;
	int noDamageTime;
	float damageNum;

	int hp;

	bool jump;
	VECTOR centerPosition;
	VECTOR headPosition;
	CharaState charaState;
	VECTOR moveRotate;

	VECTOR attackPosition;
	SphereCollision* attackSphereCollision;

	float centerPositionAmong;
	bool groundCollision;
	bool headCollision;

	

	bool debugNoDamage;
	VECTOR landPosition;
	float landPositionAdd;
};