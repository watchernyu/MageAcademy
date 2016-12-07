#ifndef Orb_h
#define Orb_h

#include <stdio.h>
#include "Rectangle.h"

class Orb {
public:
	Orb(float x, float y, float size);
	int type;
	Rect r;
	float dx, dy;
	float targetX, targetY; //the orb will try to move to this position
	bool isUsed; //being isUsed meaning the orb is now flying to the "spell generating point where spell will soon be launched"
};

#endif
