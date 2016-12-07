#include "Creature.h"

const float CREATURE_SPEED = 0.15;

Creature::Creature(float x, float y, float size) :r({x,y,size ,size }),type(type),hp(20),maxHp(20),
speed(CREATURE_SPEED),dx(0),dy(0),stunned(false),phase(0), changePhaseInterval(1.2), changePhaseTimer(0),
alive(true), moving(false), state(0), stateTimer(0), changeTargetInterval(3), changeTargetTimer(0), targetRect({ 0,0,0,0 }),
hpLossRate(0),forceMove(false),forceMoveAngle(0),forceMoveSpeed(0),forceMoveTimer(0){
}

//currently many things are messy...
//maybe we should draw a creature according to its type?

void Creature::hitByProjectile(Projectile p) {
	//maybe add here certain creatures are immune to certain projectiles?
	hp -= p.damage;
	if (hp<0)
	{
		hp = 0;
	}
}