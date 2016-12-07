#ifndef Creature_h
#define Creature_h

#include <stdio.h>
#include "Rectangle.h"
#include "Projectile.h"

enum CreatureType{FIREGHOST, ICETOTEM, SWALLOW, MISSLE};//type and phase will decide which texture to use...

class Creature {
public:
    Creature(float x, float y, float size);
    Rect r;
    int type;
	CreatureType ctype;
	int phase;
	float changePhaseInterval;
	float changePhaseTimer;
    float hp; //hit point
	float hpLossRate;
    float maxHp;
    float speed; //movement speed
	float dx;
	float dy;
	int stunned;
	void hitByProjectile(Projectile p);

	bool forceMove;
	float forceMoveAngle;
	float forceMoveSpeed;
	float forceMoveTimer;

	bool alive;
	bool moving;

	float reloadInterval;
	float reloadTimer;
	Rect targetRect;
	float changeTargetInterval;
	float changeTargetTimer;
	float contactDamage;//damage per second

	float explosionDamage;

	Creature* targetCreature;

	float waitInterval;
	float waitTimer; //sometimes the monster will stay at a position and wait for a while...

	int nOfStates;
	int state;
	float stateDurations[10];
	float stateTimer;

private:
};

#endif
