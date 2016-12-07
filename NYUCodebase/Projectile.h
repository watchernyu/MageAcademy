#ifndef Projectile_h
#define Projectile_h

#include <stdio.h>
#include "Rectangle.h"

enum ProjectileType{ FIREBALL, ICESHARD, ARCANE, SHADOWDAGGER, ICEBLADE};

class Projectile {
private:
public:
    Projectile(float x, float y, float size);
	ProjectileType ptype;
    Rect r;
    float speed;
	float angle;//angle decides where the projectile is heading.
    float dx;
    float dy;
	float damage;//damage on impact
	float aoeDamage;//area damage.
};

#endif
