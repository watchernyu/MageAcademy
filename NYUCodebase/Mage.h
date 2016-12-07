#ifndef Mage_h
#define Mage_h

#include <stdio.h>
#include <vector>
#include "Rectangle.h"
#include "Orb.h"

enum Direction { d_left, d_right, d_up, d_down, d_leftup, d_leftdown, d_rightup, d_rightdown };//let's try with enum...
enum Color{white,red,blue,green,yellow,black};

class Mage {
public:
    Mage(float x,float y,float size);
	void takeDamage(float damage);
    Rect r;
    //int direction;//direction the mage is facing
    //8 directions in total
    float hp; //hit point
	float maxHp;
	float mp;
	float maxMp;
    //float mana; //mana point
    //float speed; //movement speed
    //float rot; //current rotation
    int orb; //current number of arcane orbs that are generated and ready
    int level;
	Direction direction;
	Color color;
	float speed;
	bool isslowed;//true means slowed
	float slowtimer;//0 means normal speed

	float forceMoveAngle;
	float forceMoveSpeed;
	float forceMoveTimer;

	std::vector<Orb> fireOrbs;
	std::vector<Orb> iceOrbs;
	std::vector<Orb> windOrbs;

	float windBlastCoolDownTime; //the CD of wind blast
	float windBlastTimer; // <= 0 means can use
};

#endif
