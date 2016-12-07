#include "Mage.h"

Mage::Mage(float x, float y, float size) : r({x,y,size,size}),
hp(1000),maxHp(1000),level(1),mp(1000),maxMp(1000),
windBlastCoolDownTime(4), windBlastTimer(0),isslowed(false),slowtimer(0),
forceMoveAngle(0),forceMoveSpeed(0), forceMoveTimer(0){
    //init with x, y and size
}

void Mage::takeDamage(float damage) {
	hp -= damage;
	if (hp<0)
	{
		hp = 0;
	}
}

