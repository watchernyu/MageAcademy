#include "Projectile.h"

Projectile::Projectile(float x, float y, float size):
r({x,y,size,size}){
    //init a projectile
}

//when shield is activated... the area near the wizard should bounce
//back projectile. So Then wizard has a bigger collision ...area?
//I should try create a shield unit on the position of the player
//and reset the shield to the player's position each time update the game
//logic
