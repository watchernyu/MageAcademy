
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Mage.h"
#include "Creature.h"
#include "Projectile.h"
#include "Orb.h"
#include "Rectangle.h"

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>

////////////////////////////////////////////////////////
#define PLAYER1CHEAT 1
////////////////////////////////////////////////////////

using namespace std;
#define DEBUG 0
#define PI 3.14159265359f
#define EPS -1E7

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum EffectType{AOE, WINDBLAST};

class Effect {
public:
	Effect(float x, float y, float width);
	Rect r;
	EffectType type;
	bool used;
	float damage;
	float lifetime;
};
Effect::Effect(float x, float y, float width) :r({x,y,width,width}),type(AOE),used(false), lifetime(1){
}

class AOE {
	//let me for now set AOE to be able to kill eveything..... include yourself....
public:
	AOE(float x, float y, float width, int type,float time);
	//currently make it a circle
	//currently just make AOE affect all enemies in area then erase this AOE
	int type;
	float damage;//this damage will be inflicted on all the units in AOE area
	float time; //when time runs out, the aoe sprite will disappear
	bool effect;//true means can still deal damage...
	Rect r; //radius is r.w/2 
};
AOE::AOE(float x, float y, float width, int type, float time):r({x,y,width,width }),type(type),damage(6),time(time),effect(true){
}

//function prototype
void drawCreature(const Creature &c,int side);
void drawCreatures();
void drawLife(Rect rect, float maxHp, float hp);
void moveToPlayer(Creature &c, Mage player,float elapsed);
int checkCollision(Rect r1, Rect r2);
int checkCollisionCircle(Rect r1, Rect r2);
void setUp();
void background();
GLuint LoadTexture(const char *image_path, GLenum image_format = GL_RGBA);
void drawRect(Rect rect);
void drawSprite(Rect rect, GLuint texture);
void drawSpriteRot(Rect rect, GLuint texture, GLfloat rot);

float getRandomNumber(float a, float b);
float getReflect(Projectile proj, Rect shield);
float getDistance(Rect r1, Rect r2);
float getRadianToTarget(Rect r1, Rect r2);
void initEverything();
void tackleEvent(SDL_Event event);
void doNotLeaveWindow(Rect &r);
void doLogistics();
void drawEverything();
void drawMage(Mage mage);
void initPlayers();
void drawPlayers();
void updatePlayersDirections();
void playersMove();
void moveAccordingToDirection(Rect &r, float speed, Direction d);
void projectileMoveAll();

void createFireball(int playerNumber,Mage &player);
Rect findLaunchPoint(Rect r, float angle);
Rect findSummonPoint(Rect r, float angle);
void createIceshard(int playerNumber, Mage &player);
void drawOrbs(Mage player);
void orbMove(Mage player, vector<Orb> &orbs, float baseAngle, float extraRadius);
void orbMoveAll();
void drawProjectile(Projectile p);
void drawProjectiles();
void createShadowDagger(int playerNumber, Mage &player);
void moveAccordingToRadian(Rect &r, float speed, float angle);
float angleToRadian(float angle);
void drawSpritePhase(Rect rect, GLuint texture, int pose);

void createFireghost(int playerNumber, Mage &player);
void createIcetotem(int playerNumber, Mage &player);
void createSwallow(int playerNumber, Mage &player);
Effect createFireballAOEEffect(Rect fireball_r);

void clearCreatures();

int directionToAngle(Direction d);
Direction getPlayer1Direction();
Direction getPlayer2Direction();
void checkCollisionAll();
void testCode();

void creaturesMoveAll();
void aiAll();

//window
const int WIDTH = 800;
const int HEIGHT = 600;
const float ORTHO_HALF_WIDTH = 1.33f;
const float ORTHO_HALF_HEIGHT = 1.0f;

//player
const float PLAYER_SPEED = 1;
const float MAGE_SIZE = 0.1f;

GLfloat FIRECOLORS[] = { 1.0, 0, 0,
1.0, 0, 0,
1.0, 0, 0, 1.0, 0, 0 }; //red

GLfloat LIFECOLORS[] = { 0, 1.0, 0,
0, 1.0, 0,
0, 1.0, 0, 0, 0.1, 0 }; //green

GLfloat MANACOLORS[] = { 0.95, 0.4, 0.95,
0.95, 0.4, 0.95,
0.95, 0.4, 0.95, 0.95, 0.4, 0.95 }; //purple

GLuint mage1Texture;
GLuint mage2Texture;

GLuint iceShardTexture;
GLuint fireballTexture;
GLuint slimeTexture;
GLuint shieldTexture;
GLuint fireOrbTexture;
GLuint iceOrbTexture;
GLuint windOrbTexture;
GLuint arcaneBoltTexture;
GLuint shadowDaggerTexture;

GLuint redMarkTexture;
GLuint blueMarkTexture;

GLuint iceBladeTexture;
GLuint missleTexture;

GLuint aoeTexture;
GLuint bossHeadTexture;
GLuint bossBodyTexture;
GLuint bgTexture;
GLuint redwinTexture;
GLuint bluewinTexture;
GLuint forceFieldTexture;

GLuint fireghostTexture;
GLuint swallowTexture;
GLuint icetotemTexture;
GLuint windblastTexture;

//keyboard control movement related
bool player1MoveLeft = false;
bool player1MoveRight = false;
bool player1MoveUp = false;
bool player1MoveDown = false;
bool player2MoveLeft = false;
bool player2MoveRight = false;
bool player2MoveUp = false;
bool player2MoveDown = false;

float ORB_BASE_ANGLE = 0;

//spells
float FIREBALL_SIZE = 0.14;
float FIREBALL_SPEED = 1.8;
float FIREBALL_DAMAGE = 110;
float FIREBALL_AOE_DAMAGE = 50;
float FIREBALL_AOE_RADIUS = 0.4;
float ICESHARD_SIZE = 0.04;
float ICESHARD_SPEED = 2;
float ICESHARD_DAMAGE = 10;

float WINDBLAST_DAMAGE_MULTIPLY = 2;
float WINDBLAST_AOE_RADIUS = 0.35;
float WINDBLAST_SPEED_MULTIPLY = 1.6;

float WINDBLAST_FORCEMOVE_SPEED = 2;
float WINDBLAST_FORCEMOVE_TIME = 0.2;

float ARCANEBOLT_SIZE = 0.07;
float ARCANEBOLT_DAMAGE = 15;
float ARCANEBOLT_SPEED = 1.8;

float SHADOWDAGGER_SIZE = 0.11;
float SHADOWDAGGER_DAMAGE = 75;
float SHADOWDAGGER_SPEED = 2.2;
float SHADOWDAGGER_SLOWTIME = 1.3;
float SHADOWDAGGER_SLOWDIVIDER = 2.5; //the speed will be divided by this number..
float SHADOWDAGGER_GRADUAL_DAMAGE = 15;

float ICEBLADE_SIZE = 0.06;
float ICEBLADE_DAMAGE = 9;
float ICEBLADE_SPEED = 1.7;

//summoning
float FIREGHOST_SPEED = 0.35;
float FIREGHOST_SIZE = 0.11;
float FIREGHOST_CONTACT_DAMAGE = 17;

float ICETOTEM_SPEED = 0;
float ICETOTEM_SIZE = 0.12;

float SWALLOW_SPEED = 0.8;
float SWALLOW_SIZE = 0.1;

float MISSLE_SPEED = 0.5;
float MISSLE_DAMAGE = 35;
float MISSLE_SIZE = 0.04;

float MP_RESTORE_RATE = 120;//restore 100 points per second

float MP_BOLT = 50;
float MP_ORB = 75;
int MAX_NORB = 12;

//set frame rate
const int FRAMERATE = 60;
const float MSPERFRAME = (int)(1000 / FRAMERATE); //millisecond per frame
float ELAPSED = 0;

SDL_Window* displayWindow;

Mage player1(0,0,0.1);
Mage player2(0, 0, 0.1);
vector<Creature> creatures1;
vector<Creature> creatures2;
vector<Projectile> projectiles1;
vector<Projectile> projectiles2;
vector<Effect> neutralEffects; //this is the effect that can affact both players..

Rect bg;
bool DONE;


//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
Mix_Chunk *gRock = NULL;
Mix_Chunk *gBird = NULL;
Mix_Chunk *gFireball = NULL;
Mix_Chunk *gExplosion = NULL;
Mix_Chunk *gShieldBreak = NULL;
Mix_Chunk *gShield = NULL;
Mix_Chunk *gWolf = NULL;
Mix_Chunk *gIceBlade = NULL;
Mix_Chunk *gMagicball = NULL;
Mix_Chunk *gIcetotem = NULL;

Effect createFireballAOEEffect(Rect fireball_r) {
	Effect newEffect(fireball_r.x, fireball_r.y, FIREBALL_AOE_RADIUS);
	newEffect.type = AOE;
	newEffect.used = false;
	newEffect.lifetime = 1.5;
	newEffect.damage = FIREBALL_AOE_DAMAGE;
	return newEffect;
}

void checkCollisionHelper(Mage &ourplayer, Mage &enemyplayer, vector<Projectile> &ourproj, vector<Projectile> &enemyproj,
	vector<Creature> &ourcs, vector<Creature> &enemycs) {
	//this is becoming too messey I have to fix it...

	//check if things kill player...
	for (size_t i = 0; i < ourproj.size(); i++)
	{
		if (checkCollisionCircle(ourproj[i].r, enemyplayer.r))
		{
			enemyplayer.takeDamage(ourproj[i].damage); //maybe I can let Mage class deal with its shield logic...
			if (ourproj[i].ptype==FIREBALL)
			{
				Mix_PlayChannel(-1, gExplosion, 0);
				neutralEffects.push_back(createFireballAOEEffect(ourproj[i].r));
			}
			if (ourproj[i].ptype == SHADOWDAGGER)
			{
				enemyplayer.isslowed = true;
				enemyplayer.slowtimer = SHADOWDAGGER_SLOWTIME;
			}
			ourproj.erase(ourproj.begin() + i);
		}
	}

	//check if things kill creature...
	for (size_t i = 0; i < ourproj.size(); i++)
	{
		for (size_t j = 0; j < enemycs.size(); j++)
		{
			if (checkCollisionCircle(ourproj[i].r, enemycs[j].r))
			{
				enemycs[j].hitByProjectile(ourproj[i]);
				if (ourproj[i].ptype == FIREBALL)
				{
					Mix_PlayChannel(-1, gExplosion, 0);
					neutralEffects.push_back(createFireballAOEEffect(ourproj[i].r));
				}
				if (ourproj[i].ptype == SHADOWDAGGER)
				{
					enemycs[j].hpLossRate = SHADOWDAGGER_GRADUAL_DAMAGE;
				}
				ourproj.erase(ourproj.begin() + i);
				if (enemycs[j].hp <= 0)
				{
					enemycs.erase(enemycs.begin() + j);
				}

				break;
			}
		}
	}

	//creatures hit players..
	for (size_t i = 0; i < ourcs.size(); i++)
	{
		if (ourcs[i].ctype == FIREGHOST)
		{
			if (checkCollisionCircle(ourcs[i].r, enemyplayer.r))
			{
				enemyplayer.takeDamage(ourcs[i].contactDamage*ELAPSED);
			}
			for (size_t j = 0; j < enemycs.size(); j++)
			{
				if (checkCollisionCircle(ourcs[i].r, enemycs[j].r)) {
					enemycs[j].hp -= ourcs[i].contactDamage*ELAPSED;
					if (enemycs[j].hp <= 0)
					{
						enemycs.erase(enemycs.begin() + j);
					}
				}
			}
		}
		else if (ourcs[i].ctype == MISSLE) {
			bool hit = false;
			for (size_t j = 0; j < enemycs.size(); j++)
			{
				if (checkCollisionCircle(ourcs[i].r, enemycs[j].r)) {
					enemycs[j].hp -= ourcs[i].explosionDamage;
					if (enemycs[j].hp <= 0)
					{
						enemycs.erase(enemycs.begin() + j);
					}
					ourcs.erase(ourcs.begin() + i);
					hit = true;
					break;
				}
			}
			if (!hit&&checkCollisionCircle(ourcs[i].r, enemyplayer.r))
			{
				enemyplayer.takeDamage(ourcs[i].explosionDamage);
				ourcs.erase(ourcs.begin() + i);
			}
			else if (!hit) {
				ourcs[i].hp -= ELAPSED * 5;
				if (ourcs[i].hp <= 0)
				{
					ourcs.erase(ourcs.begin() + i);
				}
			}
		}
	}
}

void checkCollisionAll() {

	checkCollisionHelper(player1,player2, projectiles1, projectiles2,creatures1, creatures2);
	checkCollisionHelper(player2, player1, projectiles2, projectiles1, creatures2, creatures1);

	//neutral effects....
	for (size_t i = 0; i < neutralEffects.size(); i++)
	{
		if (neutralEffects[i].used)
		{
			continue;
		}
		switch (neutralEffects[i].type)
		{
		case AOE:
			for (size_t j = 0; j < creatures1.size(); j++)
			{
				if (checkCollisionCircle(neutralEffects[i].r, creatures1[j].r))
				{
					creatures1[j].hp -= neutralEffects[i].damage;
				}
			}
			for (size_t j = 0; j < creatures2.size(); j++)
			{
				if (checkCollisionCircle(neutralEffects[i].r, creatures2[j].r))
				{
					creatures2[j].hp -= neutralEffects[i].damage;
				}
			}
			if (checkCollisionCircle(neutralEffects[i].r, player1.r))
			{
				player1.hp -= neutralEffects[i].damage;
			}
			if (checkCollisionCircle(neutralEffects[i].r, player2.r))
			{
				player2.hp -= neutralEffects[i].damage;
			}
			neutralEffects[i].used = true;
			break;
		default:
			break;
		}
	}
	/*
		//check if things kill player...
	for (size_t i = 0; i < projectiles1.size(); i++)
	{
		if (checkCollisionCircle(projectiles1[i].r,player2.r))
		{
			player2.takeDamage(projectiles1[i].damage); //maybe I can let Mage class deal with its shield logic...
			projectiles1.erase(projectiles1.begin() + i);
		}
	}

	//check if things kill creature...
	for (size_t i = 0; i < projectiles1.size(); i++)
	{
		for (size_t j = 0; j < creatures2.size(); j++)
		{
			if (checkCollisionCircle(projectiles1[i].r,creatures2[j].r))
			{
				creatures2[j].hitByProjectile(projectiles1[i]);
				projectiles1.erase(projectiles1.begin() + i);
				if (creatures2[j].hp<=0)
				{
					creatures2.erase(creatures2.begin()+j);
				}
				break;
			}
		}
	}

	for (size_t i = 0; i < projectiles2.size(); i++)
	{
		if (checkCollisionCircle(projectiles2[i].r, player1.r))
		{
			player1.takeDamage(projectiles2[i].damage); //maybe I can let Mage class deal with its shield logic...
			projectiles2.erase(projectiles2.begin() + i);
		}
	}

	for (size_t i = 0; i < projectiles2.size(); i++)
	{
		for (size_t j = 0; j < creatures1.size(); j++)
		{
			if (checkCollisionCircle(projectiles2[i].r, creatures1[j].r))
			{
				creatures1[j].hitByProjectile(projectiles2[i]);
				projectiles2.erase(projectiles2.begin() + i);
				if (creatures1[j].hp <= 0)
				{
					creatures1.erase(creatures1.begin() + j);
				}
				break;
			}
		}
	}

	//creatures hit players..
	for (size_t i = 0; i < creatures1.size(); i++)
	{
		if (creatures1[i].ctype == FIREGHOST)
		{
			if (checkCollisionCircle(creatures1[i].r, player2.r))
			{
				player2.takeDamage(creatures1[i].contactDamage*ELAPSED);
			}
			for (size_t j = 0; j < creatures2.size(); j++)
			{
				if (checkCollisionCircle(creatures1[i].r, creatures2[j].r)) {
					creatures2[j].hp -= creatures1[i].contactDamage*ELAPSED;
					if (creatures2[j].hp <= 0)
					{
						creatures2.erase(creatures2.begin() + j);
					}
				}
			}
		}
		else if (creatures1[i].ctype == MISSLE) {
			bool hit = false;
			for (size_t j = 0; j < creatures2.size(); j++)
			{
				if (checkCollisionCircle(creatures1[i].r, creatures2[j].r)) {
					creatures2[j].hp -= creatures1[i].explosionDamage;
					if (creatures2[j].hp<=0)
					{
						creatures2.erase(creatures2.begin() + j);
					}
					creatures1.erase(creatures1.begin() + i);
					hit = true;
					break;
				}
			}
			if (!hit&&checkCollisionCircle(creatures1[i].r, player2.r))
			{
				player2.takeDamage(creatures1[i].explosionDamage);
				creatures1.erase(creatures1.begin() + i);
			}
			else if(!hit) {
				creatures1[i].hp -= ELAPSED * 5;
				if (creatures1[i].hp <= 0)
				{
					creatures1.erase(creatures1.begin() + i);
				}
			}
		}
	}

	for (size_t i = 0; i < creatures2.size(); i++)
	{
		if (creatures2[i].ctype == FIREGHOST)
		{
			if (checkCollisionCircle(creatures2[i].r, player1.r))
			{
				player1.takeDamage(creatures2[i].contactDamage*ELAPSED);
			}

			for (size_t j = 0; j < creatures1.size(); j++)
			{
				if (checkCollisionCircle(creatures2[i].r, creatures1[j].r)) {
					creatures1[j].hp -= creatures2[i].contactDamage*ELAPSED;
					if (creatures1[j].hp <= 0)
					{
						creatures1.erase(creatures1.begin() + j);
					}
				}
			}
		}
		else if (creatures2[i].ctype == MISSLE) {
			bool hit = false;
			for (size_t j = 0; j < creatures1.size(); j++)
			{
				if (checkCollisionCircle(creatures2[i].r, creatures1[j].r)) {
					creatures1[j].hp -= creatures2[i].explosionDamage;
					if (creatures1[j].hp <= 0)
					{
						creatures1.erase(creatures1.begin() + j);
					}
					creatures2.erase(creatures2.begin() + i);
					hit = true;
					break;
				}
			}
			if (!hit&&checkCollisionCircle(creatures2[i].r, player1.r))
			{
				player1.takeDamage(creatures2[i].explosionDamage);
				creatures2.erase(creatures2.begin() + i);
			}
			else if(!hit) {
				creatures2[i].hp -= ELAPSED * 5;
				if (creatures2[i].hp <= 0)
				{
					creatures2.erase(creatures2.begin() + i);
				}
			}
		}
	}
	*/

}

int checkEndOfGame() {
	if (player1.hp<=0)
	{
		return 2;
	}
	if (player2.hp<=0)
	{
		return 1;
	}
	return 0;
}

void updateCreatureShadowDamageHelper(vector<Creature> &cs) {
	for (size_t i = 0; i < cs.size(); i++)
	{
		cs[i].hp -= cs[i].hpLossRate*ELAPSED;
	}
}

void updateCreatureShadowDamage() {
	updateCreatureShadowDamageHelper(creatures1);
	updateCreatureShadowDamageHelper(creatures2);
}


int ENDOFGAME = 0;
int main(int argc, char *argv[]) {

	testCode();//this part is solely for testing purpose....

	setUp();
	initEverything();

	SDL_Event event;
	
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	float lastFrameTicks = 0.0f;
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	ELAPSED = ticks - lastFrameTicks;

	//play music!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//don't have original music though....
	if (Mix_PlayingMusic() == 0)
	{
		//Play the music
		Mix_PlayMusic(gMusic, -1);
	}
	
	//main game loop
	while (!DONE) {
		ticks = (float)SDL_GetTicks() / 1000.0f;
		ELAPSED = ticks - lastFrameTicks;//in seconds
		lastFrameTicks = ticks;

		//tackle all keyboard events...
		while (SDL_PollEvent(&event)) {
			tackleEvent(event);
		}
		if (ENDOFGAME==0)
		{
			ENDOFGAME = checkEndOfGame();//0 means blue wins, 1 means red wins
			
			if (ENDOFGAME>0)
			{
				if (creatures1.size()>0 || creatures2.size()>0)
				{
					clearCreatures();
				}
			}
		}

		ORB_BASE_ANGLE += ELAPSED;

		////game logic

		//summoned creature AI part
		aiAll();

		updateCreatureShadowDamage();//creatures that are affected by shadow dagger will gradually lose health

		//move logic
		updatePlayersDirections();
		playersMove();
		orbMoveAll();
		projectileMoveAll();
		creaturesMoveAll();

		//collision
		checkCollisionAll();
		
		//logistics, such as not allowing player go outside the world...
		//and erase monsters that are dead... and erase effects that are used...
		doLogistics();
		    
		//DRAWING!
		drawEverything();

		//I will fix the texture later......
		//maybe do some ..... errrrrrrr dynamic changed background + related boss behavior, creature generation

		//regulate frame rate
		SDL_GL_SwapWindow(displayWindow);
		int actualTimeInFrameMS = (int)(SDL_GetTicks() - ticks);
		if (actualTimeInFrameMS < MSPERFRAME) {
			SDL_Delay(int(MSPERFRAME - actualTimeInFrameMS));
		}
	}
	SDL_Quit();
	return 0;
}

Creature createRandomMonster() {
    float xr = ((float)rand() / RAND_MAX)*2.0f-1;
    float yr = ((float)rand() / RAND_MAX)*2.0f - 1;
    Creature newC(xr,-0.7+yr/4,0);
    return newC;
}

Projectile createMonsterFireball(Rect r, Rect target, bool randomized) {
	float x, y, size, speed, dx, dy;
	int type = 1;
	size = 0.12;
	speed = 2.5;
	//hardcode some numbers for now
	x = r.x;
	y = r.y;
	dx = 0;
	dy = 0;
	Rect finaltarget = target;
	if (randomized)
	{
		finaltarget.x += ((float)rand() / RAND_MAX / 10 - 0.05);
		finaltarget.y += ((float)rand() / RAND_MAX / 10 - 0.05);
	}
	float angle = getRadianToTarget(r, finaltarget);
	dx = speed * cosf(angle);
	dy = speed * sinf(angle);

	Projectile newProj(x, y, size);
	return newProj;
}

Projectile createMonsterProj(Creature c, Rect target,bool randomized) {
	Rect r = c.r;
	float x, y, size, speed, dx, dy;
	int type = 0;
	size = 0.05;
	speed = 2.5;
	//hardcode some numbers for now
	x = r.x;
	y = r.y;
	dx = 0;
	dy = 0;
	Rect finaltarget = target;
	if (randomized)
	{
		finaltarget.x += ((float)rand()/RAND_MAX/10-0.05);
		finaltarget.y += ((float)rand() / RAND_MAX / 10 - 0.05);
	}
	float angle = getRadianToTarget(c.r, finaltarget);
	dx = speed * cosf(angle);
	dy = speed * sinf(angle);

	Projectile newProj(x, y, size);
	return newProj;
}

Orb createOrb(Mage &player) {
	Rect r = player.r;
	player.mp -= MP_ORB;
	float x, y, size;
	int type = 0;
	size = 0.022;
	//hardcode some numbers for now
	x = r.x;
	y = r.y;
	Orb newOrb(x, y, size);
	return newOrb;
}

void moveToPlayer(Creature &c, Mage player,float elapsed) {
    float xx = player.r.x - c.r.x;
    float yy = player.r.y - c.r.y;
    float tan_v = atan2(yy, xx);
    float dx = c.speed *cosf(tan_v);
    float dy = c.speed * sinf(tan_v);
    c.r.x += elapsed * dx;
    c.r.y += elapsed * dy;
}

int checkCollision(Rect r1, Rect r2) {
    //check if two rectangles, r1 and r2 have collided.
	//1 for collided

	float r1left = r1.x - r1.w / 2;
	float r1right = r1.x + r1.w / 2;
	float r1up = r1.y + r1.h / 2;
	float r1down = r1.y - r1.h / 2;

	float r2left = r2.x - r2.w / 2;
	float r2right = r2.x + r2.w / 2;
	float r2up = r2.y + r2.h / 2;
	float r2down = r2.y - r2.h / 2;

	if (r1left<r2right && r1right>r2left && r1down<r2up && r1up>r2down) {
		return 1;
	}
	return 0;
}

int checkCollisionCircle(Rect r1, Rect r2) {
    float r1radius = r1.w / 2;
    float r2radius = r2.w / 2;
    float x_difference = r2.x - r1.x;
    float y_difference = r2.y - r1.y;
    if (sqrt(x_difference * x_difference + y_difference * y_difference) <= r1radius + r2radius) {
        return 1;
    }
    return 0;
}

// Initialize everything and give us a window
void setUp() {
	// Initializes event, file I/O and threading subsystems by default
	// and also initialize the video subsystem
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	// Set up the window with the specified title, centered,
	// dimensions 800 by 600, set to use OpenGL
	displayWindow =
		SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	// Context contains all the OpenGL information 
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	// And we have to say to associate this context as the current
	// context for the window, even though we just got it from the
	// window.
	SDL_GL_MakeCurrent(displayWindow, context);

	// glViewport specifies a pixel rectangle in the window into which
	// the final image is displayed.
	glViewport(0, 0, WIDTH, HEIGHT);     // Same as default
	glMatrixMode(GL_PROJECTION); // Changes will be applied to the projection matrix.
								 // Set up the boundaries for the projection matrix.
								 // left, right, top, bottom, near, far
	glOrtho(-ORTHO_HALF_WIDTH, ORTHO_HALF_WIDTH,   // left, right
		-ORTHO_HALF_HEIGHT, ORTHO_HALF_HEIGHT, // bottom, top
		-1.0, 1.0);	// near, far

	//init audio
	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}

	//Load music
	gMusic = Mix_LoadMUS("BGM.mp3");
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}

	//Load sound effects
	gRock = Mix_LoadWAV("rock.wav");
	if (gRock == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}

	gBird = Mix_LoadWAV("bird.wav");
	if (gBird == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}

	gShield = Mix_LoadWAV("shield.wav");
	if (gShield == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}

	gFireball = Mix_LoadWAV("fireballFlying.wav");
	if (gFireball == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}

	gExplosion = Mix_LoadWAV("explosion.wav");
	if (gExplosion == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}

	gShieldBreak = Mix_LoadWAV("shieldBroken.wav");
	if (gShieldBreak == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
	gWolf = Mix_LoadWAV("wolf.wav");
	if (gWolf == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
	gIceBlade = Mix_LoadWAV("iceBlade.wav");
	if (gIceBlade == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
	gMagicball = Mix_LoadWAV("magicball.wav");
	if (gMagicball == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
	gIcetotem = Mix_LoadWAV("icetotem.wav");
	if (gIcetotem == NULL)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
}



// Set up the background
void background() {
	//Initialize clear color
	// RGBA: Values from 0 to 1.
	// 0,0,0 is black. 1,1,1 is white.  alpha = 1 is opaque
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clear the color buffer`
	glClear(GL_COLOR_BUFFER_BIT);
}

//draw a rectangle in the window
void drawRect(Rect rect) {
	GLfloat x = rect.x;
	GLfloat y = rect.y;
	GLfloat w = rect.w;
	GLfloat h = rect.h;
	GLfloat quad[] = {-w/2,-h/2,w/2,-h/2,w/2,h/2,-w/2,h/2};
	glVertexPointer(2, GL_FLOAT, 0, quad);
	// Let us specify the vertes array to use
	glEnableClientState(GL_VERTEX_ARRAY);
	// Modifying the Model-view matrix
	glMatrixMode(GL_MODELVIEW);
	// to the identiy, i.e. we are not moving / transforming the figure
	glLoadIdentity();
	GLfloat triangleColors[] = { 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }; //white
	glColorPointer(3, GL_FLOAT, 0, triangleColors);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y,0);

	glDrawArrays(GL_QUADS, 0, 4);
	// If we dont disable the color array, it will effect other things drawn.
	glDisableClientState(GL_COLOR_ARRAY);
}

GLuint LoadTexture(const char *image_path, GLenum image_format) {
	// Load the image file
	SDL_Surface *surface = IMG_Load(image_path);
	// Always check that opening worked!
	if (!surface) {
		cerr << "Failed to open: " << image_path
			<< " " << IMG_GetError() << endl;
	}

	GLuint textureID;
	// Provides one or more texture IDs. Docs refer to them as "names"
	// The first arg says how many
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h,
		0, image_format, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);
	return textureID;
}

void drawMage(Mage mage) {
	drawSprite(mage.r, mage1Texture);
	drawLife(mage.r, mage.maxHp, mage.hp);
}

void drawCreature(const Creature &c,int side) {
	GLuint texture = fireghostTexture;
	switch (c.ctype)
	{
	case FIREGHOST:
		texture = fireghostTexture;
		break;
	case ICETOTEM:
		texture = icetotemTexture;
		break;
	case SWALLOW:
		texture = swallowTexture;
		break;
	case MISSLE:
		texture = missleTexture;
		break;
	default:
		break;
	}

	drawSpritePhase(c.r, texture, c.phase);
	if (side == 1)//if player1's creature
	{
		drawSprite(c.r, redMarkTexture);
	}
	else {
		drawSprite(c.r, blueMarkTexture);
	}
	if (c.ctype==MISSLE)
	{
		return;
	}

	drawLife(c.r,c.maxHp,c.hp);

}

void drawMana(Rect rect, float maxMp, float mp) {
	GLfloat y = rect.y + rect.h / 2 + 0.03;
	GLfloat w = rect.w * (mp / maxMp);
	GLfloat x = rect.x - (rect.w - w) / 2;
	GLfloat h = 0.02;
	GLfloat quad[] = { -w / 2,-h / 2,w / 2,-h / 2,w / 2,h / 2,-w / 2,h / 2 };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	// Let us specify the vertes array to use
	glEnableClientState(GL_VERTEX_ARRAY);
	// Modifying the Model-view matrix
	glMatrixMode(GL_MODELVIEW);
	// to the identiy, i.e. we are not moving / transforming the figure
	glLoadIdentity();
	GLfloat * triangleColors = MANACOLORS; //green
	glColorPointer(3, GL_FLOAT, 0, triangleColors);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0);

	glDrawArrays(GL_QUADS, 0, 4);
	// If we dont disable the color array, it will effect other things drawn.
	glDisableClientState(GL_COLOR_ARRAY);
}

//draw a rectangle in the window
void drawLife(Rect rect,float maxHp,float hp) {
	GLfloat y = rect.y+rect.h/2+0.01;
	GLfloat w = rect.w * (hp/maxHp);
	GLfloat x = rect.x-(rect.w-w)/2;
	GLfloat h = 0.02;
	GLfloat quad[] = { -w / 2,-h / 2,w / 2,-h / 2,w / 2,h / 2,-w / 2,h / 2 };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	// Let us specify the vertes array to use
	glEnableClientState(GL_VERTEX_ARRAY);
	// Modifying the Model-view matrix
	glMatrixMode(GL_MODELVIEW);
	// to the identiy, i.e. we are not moving / transforming the figure
	glLoadIdentity();
	GLfloat * triangleColors = LIFECOLORS; //green
	glColorPointer(3, GL_FLOAT, 0, triangleColors);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0);

	glDrawArrays(GL_QUADS, 0, 4);
	// If we dont disable the color array, it will effect other things drawn.
	glDisableClientState(GL_COLOR_ARRAY);
}


//draw a rectangle and add a texture to it
void drawSprite(Rect rect, GLuint texture) {
	GLfloat x = rect.x;
	GLfloat y = rect.y;
	GLfloat w = rect.w;
	GLfloat h = rect.h;
	// Turn on texturing so We can apply a texture to our shape
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(rect.x, rect.y, 0.0);

	GLfloat quad[] = { -w / 2,-h / 2,w / 2,-h / 2,w / 2,h / 2,-w / 2,h / 2 };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0 };

	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}


void drawSpriteRot(Rect rect, GLuint texture, GLfloat rot) {
	GLfloat x = rect.x;
	GLfloat y = rect.y;
	GLfloat w = rect.w;
	GLfloat h = rect.h;
	// Turn on texturing so We can apply a texture to our shape
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(rect.x, rect.y, 0.0);
	glRotatef(rot, 0, 0, 1);

	GLfloat quad[] = { -w / 2,-h / 2,w / 2,-h / 2,w / 2,h / 2,-w / 2,h / 2 };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	GLfloat quadUVs[] = { 0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0 };

	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}

void initPlayers() {
	player1 = Mage(0, 0, 0.1);
	player2 =  Mage(0, 0, 0.1);

	player1.r.x = -0.8;
	player1.r.y = 0.2;
	player1.r.w = MAGE_SIZE;
	player1.r.h = MAGE_SIZE;
	player1.direction = d_up;
	player1.color = red;
	player1.speed = 1;

	if (PLAYER1CHEAT)
	{
		player1.maxHp = 3000;
		player1.maxMp = 1500;
		player1.hp = player1.maxHp;
		player1.mp = player1.maxMp;
	}

	player2.r.x = 0.8;
	player2.r.y = -0.2;
	player2.r.w = MAGE_SIZE;
	player2.r.h = MAGE_SIZE;
	player2.direction = d_up;
	player2.color = blue;
	player2.speed = 1;
}

void clearCreatures() {
	creatures1.clear();
	creatures2.clear();
}

void initEverything() {
	//load textures
	try {
		mage1Texture = LoadTexture("mage_red.png");
		mage2Texture = LoadTexture("mage_blue.png");
		iceShardTexture = LoadTexture("iceShard.png");
		arcaneBoltTexture = LoadTexture("arcaneBolt.png");
		shadowDaggerTexture = LoadTexture("shadowDagger.png");
		redMarkTexture = LoadTexture("redMark.png");
		blueMarkTexture = LoadTexture("blueMark.png");

		iceBladeTexture = LoadTexture("iceBlade.png");
		missleTexture = LoadTexture("missle.png");
		fireballTexture = LoadTexture("fireball.png");
		slimeTexture = LoadTexture("slime.png");
		shieldTexture = LoadTexture("shield.png");

		fireOrbTexture = LoadTexture("fireOrb.png");
		iceOrbTexture = LoadTexture("iceOrb.png");
		windOrbTexture = LoadTexture("windOrb.png");

		fireghostTexture = LoadTexture("fireghost.png");
		swallowTexture = LoadTexture("swallow.png");
		icetotemTexture = LoadTexture("icetotem.png");

		windblastTexture = LoadTexture("windblast.png");

		aoeTexture = LoadTexture("lava.png");
		bossHeadTexture = LoadTexture("bossHead.png");
		bossBodyTexture = LoadTexture("bossBody.png");
		bgTexture = LoadTexture("bg.png");
		redwinTexture = LoadTexture("redwin.png");
		bluewinTexture = LoadTexture("bluewin.png");
		forceFieldTexture = LoadTexture("forceField.png");
	}
	catch (int e) {
		cout << "error in loading image\n";
	}

	//init keyboard controlled moves
	player1MoveLeft = false;
	player1MoveRight = false;
	player1MoveUp = false;
	player1MoveDown = false;
	player2MoveLeft = false;
	player2MoveRight = false;
	player2MoveUp = false;
	player2MoveDown = false;

	DONE = false;

	bg = { 0,0,ORTHO_HALF_WIDTH * 2,ORTHO_HALF_HEIGHT * 2 };

	//clear vectors
	creatures1.clear();
	creatures2.clear();

	initPlayers();
}

Rect findLaunchPoint(Rect r,float angle) {
	//given the rect of a creature/mage and angle it's firing, give the point 
	//where the projectile should be generated
	float margin = 0.02;
	cout << angle<<endl;
	Rect result = { r.x + (r.w/2+ margin)*cosf(angle*PI/180),r.y + (r.w/2+ margin)*sinf(angle*PI/180) ,0,0 };
	return result;
}

Rect findSummonPoint(Rect r, float angle) {
	//given the rect of a creature/mage and angle it's firing, give the point 
	//where the projectile should be generated
	float margin = 0.1;
	cout << angle << endl;
	Rect result = { r.x + (r.w / 2 + margin)*cosf(angle*PI / 180),r.y + (r.w / 2 + margin)*sinf(angle*PI / 180) ,0,0 };
	return result;
}

void createFireghost(int playerNumber, Mage &player) {
	int orbcost = 3;
	if (player.fireOrbs.size() >= orbcost)
	{
		for (size_t i = 0; i < orbcost; i++)
		{
			player.fireOrbs.pop_back();
		}

		Mix_PlayChannel(-1, gWolf, 0);

		Rect r_launch = findSummonPoint(player.r, directionToAngle(player.direction));
		Creature newCreat(r_launch.x, r_launch.y, FIREGHOST_SIZE);
		newCreat.ctype = FIREGHOST;
		newCreat.maxHp = 180;
		newCreat.hp = newCreat.maxHp;
		newCreat.speed = FIREGHOST_SPEED;
		newCreat.changeTargetInterval = 1;
		newCreat.contactDamage = FIREGHOST_CONTACT_DAMAGE;

		if (playerNumber == 1)
		{
			creatures1.push_back(newCreat);
		}
		else {
			creatures2.push_back(newCreat);
		}
	}
}

void createIcetotem(int playerNumber, Mage &player) {
	//icetotem is stationary, can fire long range ice shard to attack
	int orbcost = 4;
	if (player.iceOrbs.size() >= orbcost)
	{
		for (size_t i = 0; i < orbcost; i++)
		{
			player.iceOrbs.pop_back();
		}
		Mix_PlayChannel(-1, gIcetotem, 0);
		Rect r_launch = findSummonPoint(player.r, directionToAngle(player.direction));
		Creature newCreat(r_launch.x, r_launch.y, ICETOTEM_SIZE);
		newCreat.ctype = ICETOTEM;
		newCreat.maxHp = 275;
		newCreat.hp = newCreat.maxHp;
		newCreat.speed = ICETOTEM_SPEED;
		newCreat.reloadInterval = 2.6;
		newCreat.reloadTimer = newCreat.reloadInterval;

		if (playerNumber == 1)
		{
			creatures1.push_back(newCreat);
		}
		else {
			creatures2.push_back(newCreat);
		}
	}
}

void createSwallow(int playerNumber, Mage &player) {
	//icetotem is stationary, can fire long range ice shard to attack
	int fireorbcost = 2;
	int iceorbcost = 2;
	if (player.iceOrbs.size() >= iceorbcost && player.fireOrbs.size()>=fireorbcost)
	{
		for (size_t i = 0; i < iceorbcost; i++)
		{
			player.iceOrbs.pop_back();
		}
		for (size_t i = 0; i < fireorbcost; i++)
		{
			player.fireOrbs.pop_back();
		}

		Mix_PlayChannel(-1, gBird, 0);

		Rect r_launch = findSummonPoint(player.r, directionToAngle(player.direction));
		Creature newCreat(r_launch.x, r_launch.y, SWALLOW_SIZE);
		newCreat.ctype = SWALLOW;
		newCreat.maxHp = 185;
		newCreat.hp = newCreat.maxHp;
		newCreat.speed = SWALLOW_SPEED;
		newCreat.changeTargetInterval = 2.5;
		newCreat.waitInterval = 1.5;

		newCreat.state = 0;
		newCreat.moving = true;
		newCreat.targetRect = { getRandomNumber(-1,1),getRandomNumber(-1,1) ,0,0 };
		newCreat.stateDurations[0] = 1.5; //flying
		newCreat.stateDurations[1] = 0; //firing (no time)
		newCreat.stateDurations[2] = 2;//wait
		newCreat.nOfStates = 3;

		newCreat.stateTimer = newCreat.stateDurations[0];

		float radian = getRadianToTarget(newCreat.r, newCreat.targetRect);
		newCreat.dx = newCreat.speed * cosf(radian);
		newCreat.dy = newCreat.speed * sinf(radian);

		if (playerNumber == 1)
		{
			creatures1.push_back(newCreat);
		}
		else {
			creatures2.push_back(newCreat);
		}
	}
}


Effect createWindBlastEffect(Rect r) {
	//return a wind blast effect object
	Effect newEffect(r.x, r.y, 0.3);
	newEffect.type = WINDBLAST;
	newEffect.used = true;
	newEffect.lifetime = 0.2;
	newEffect.damage = 0;
	return newEffect;
}

void useWindBlast(Mage &player,Mage &enemyPlayer, vector<Projectile> &ourProjs,vector<Projectile> &enemyProjs, vector<Creature> &ourCreatures,vector<Creature> &enemyCreatures) {
	if (player.windBlastTimer>0)
	{
		//if cool down not finished
		return;
	}
	player.windBlastTimer = player.windBlastCoolDownTime;//reset timer
	Mix_PlayChannel(-1, gShield, 0);
	
	//write logic here to bounce back projectiles...
	float angle = directionToAngle(player.direction);
	float radian = angle/180*PI;
	float windRadius = WINDBLAST_AOE_RADIUS;
	float distanceFromPlayer = 0.25;
	Rect wind_r = { player.r.x + distanceFromPlayer*cosf(radian), player.r.y + distanceFromPlayer*sinf(radian),
		windRadius,windRadius };
	
	//here add bounce back projectiles logic
	for (size_t i = 0; i < enemyProjs.size(); i++)
	{
		if (checkCollisionCircle(wind_r, enemyProjs[i].r))
		{
			//convert from enemy proj to our proj
			enemyProjs[i].angle = getRadianToTarget(player.r, enemyProjs[i].r) /PI *180;
			enemyProjs[i].damage *= WINDBLAST_DAMAGE_MULTIPLY; //
			enemyProjs[i].speed *= WINDBLAST_SPEED_MULTIPLY;
			ourProjs.push_back(enemyProjs[i]);
			enemyProjs.erase(enemyProjs.begin()+i);
		}
	}

	for (size_t i = 0; i < ourCreatures.size(); i++)
	{
		if (checkCollisionCircle(wind_r, ourCreatures[i].r) && ourCreatures[i].ctype!=ICETOTEM) {
			ourCreatures[i].forceMoveTimer = WINDBLAST_FORCEMOVE_TIME;
			ourCreatures[i].forceMoveSpeed = WINDBLAST_FORCEMOVE_SPEED;
			ourCreatures[i].forceMoveAngle = getRadianToTarget(player.r, ourCreatures[i].r)*180/PI;
		}
	}

	for (size_t i = 0; i < enemyCreatures.size(); i++)
	{
		if (checkCollisionCircle(wind_r, enemyCreatures[i].r) && enemyCreatures[i].ctype != ICETOTEM) {
			enemyCreatures[i].forceMoveTimer = WINDBLAST_FORCEMOVE_TIME;
			enemyCreatures[i].forceMoveSpeed = WINDBLAST_FORCEMOVE_SPEED;
			enemyCreatures[i].forceMoveAngle = getRadianToTarget(player.r, enemyCreatures[i].r) * 180 / PI;
		}
	}

	if (checkCollisionCircle(wind_r, enemyPlayer.r)) {
		enemyPlayer.forceMoveTimer = WINDBLAST_FORCEMOVE_TIME/2; //mage only be affected half the time length
		enemyPlayer.forceMoveSpeed = WINDBLAST_FORCEMOVE_SPEED;
		enemyPlayer.forceMoveAngle = getRadianToTarget(player.r, enemyPlayer.r) * 180 / PI;
	}

	neutralEffects.push_back(createWindBlastEffect(wind_r));
}


Creature createMissle(Rect swallow_r) {
	//swallow will launch a missle that can track down the other player...
	//use this function to create a missle at the position of the swallow
	Mix_PlayChannel(-1, gMagicball, 0);
	Rect r_launch = swallow_r;
	Creature newCreat(r_launch.x, r_launch.y, MISSLE_SIZE);
	newCreat.ctype = MISSLE;
	newCreat.maxHp = 30;
	newCreat.hp = newCreat.maxHp;
	newCreat.speed = MISSLE_SPEED;
	newCreat.explosionDamage = MISSLE_DAMAGE;

	newCreat.moving = true;
	return newCreat;
}

void createFireball(int playerNumber, Mage &player) {//mage standard spell
	int orbcost = 2;
	if (player.fireOrbs.size()>= orbcost)
	{
		for (size_t i = 0; i < orbcost; i++)
		{
			player.fireOrbs.pop_back();
		}

		Mix_PlayChannel(-1, gFireball, 0);

		Rect r_launch = findLaunchPoint(player.r,directionToAngle(player.direction));
		Projectile fireball(r_launch.x, r_launch.y, FIREBALL_SIZE);
		fireball.angle = directionToAngle(player.direction);
		cout << fireball.angle << endl;
		fireball.speed = FIREBALL_SPEED;
		fireball.ptype = FIREBALL;
		fireball.damage = FIREBALL_DAMAGE;
		fireball.aoeDamage = FIREBALL_AOE_DAMAGE;
		if (playerNumber==1)
		{
			projectiles1.push_back(fireball);
		}
		else {
			projectiles2.push_back(fireball);
		}
	}
}

void createShadowDagger(int playerNumber, Mage &player) {//mage standard spell
	int fireorbcost = 1;
	int iceorbcost = 1;
	if (player.fireOrbs.size() >= fireorbcost && player.iceOrbs.size() >= iceorbcost)
	{
		for (size_t i = 0; i < fireorbcost; i++)
		{
			player.fireOrbs.pop_back();
		}
		for (size_t i = 0; i < iceorbcost; i++)
		{
			player.iceOrbs.pop_back();
		}
		Rect r_launch = findLaunchPoint(player.r, directionToAngle(player.direction));
		Projectile newproj(r_launch.x, r_launch.y, SHADOWDAGGER_SIZE);
		newproj.angle = directionToAngle(player.direction);
		cout << newproj.angle << endl;
		newproj.speed = SHADOWDAGGER_SPEED;
		newproj.ptype = SHADOWDAGGER;
		newproj.damage = SHADOWDAGGER_DAMAGE;
		if (playerNumber == 1)
		{
			projectiles1.push_back(newproj);
		}
		else {
			projectiles2.push_back(newproj);
		}
	}
}

void createArcanebolt(int playerNumber, Mage &player) {
	//arcane arrow is the most basic attack type 
	//only cost some mana...
	Rect r_launch = findLaunchPoint(player.r, directionToAngle(player.direction));
	if (player.mp<MP_BOLT)
	{
		return;
	}
	else {
		player.mp -= MP_BOLT;
	}

	Mix_PlayChannel(-1, gMagicball, 0);

	for (size_t i = 0; i < 3; i++)
	{
		//each time 3 shards will be generated 
		Projectile bolt(r_launch.x + getRandomNumber(-0.05, 0.05), r_launch.y + getRandomNumber(-0.05, 0.05), ARCANEBOLT_SIZE);
		bolt.angle = directionToAngle(player.direction);
		cout << bolt.angle << endl;
		bolt.speed = ARCANEBOLT_SPEED + getRandomNumber(-0.1, 0.1);
		bolt.ptype = ARCANE;
		bolt.damage = ARCANEBOLT_DAMAGE;
		if (playerNumber == 1)
		{
			projectiles1.push_back(bolt);
		}
		else {
			projectiles2.push_back(bolt);
		}
	}
}

Projectile createCreatureProjectile(Rect sourceR,Rect targetR, ProjectileType type) {
	float radian = getRadianToTarget(sourceR, targetR);
	Rect r_launch = { sourceR.x + sourceR.w/2*cosf(radian) ,sourceR.y + sourceR.w / 2 * sinf(radian),0,0 };
	Projectile proj(0, 0, 0);
	switch (type)
	{
	case ICEBLADE:

		Mix_PlayChannel(-1, gIceBlade, 0);

		//each time 3 shards will be generated 
		proj.r.x = r_launch.x + getRandomNumber(-0.05, 0.05);
		proj.r.y = r_launch.y + getRandomNumber(-0.05, 0.05);
		proj.r.w = ICEBLADE_SIZE;
		proj.r.h = ICEBLADE_SIZE;

		proj.angle = radian / PI * 180;
		proj.speed = ICEBLADE_SPEED + getRandomNumber(-0.1, 0.1);
		proj.ptype = ICEBLADE;
		proj.damage = ICEBLADE_DAMAGE;
		break;
	case MISSLE:
		break;
	default:
		break;
	}
	return proj;
}

void createIceshard(int playerNumber, Mage &player) {//mage standard spell
	//arcane arrow is the most basic attack type 
	//only cost some mana...

	int orbcost = 2;
	if (player.iceOrbs.size() >= orbcost)
	{
		cout << player.iceOrbs.size() << endl;
		for (size_t i = 0; i < orbcost; i++)
		{
			player.iceOrbs.pop_back();
		}
		Rect r_launch = findLaunchPoint(player.r, directionToAngle(player.direction));

		Mix_PlayChannel(-1, gIceBlade, 0);

		for (size_t i = 0; i < 20; i++)
		{
			//each time 3 shards will be generated 
			Projectile iceshard(r_launch.x + getRandomNumber(-0.05, 0.05), r_launch.y + getRandomNumber(-0.05, 0.05), ICESHARD_SIZE);
			iceshard.angle = directionToAngle(player.direction) + getRandomNumber(-20, 20);
			iceshard.speed = ICESHARD_SPEED + getRandomNumber(-0.1, 0.1);
			iceshard.ptype = ICESHARD;
			iceshard.damage = ICESHARD_DAMAGE;
			if (playerNumber == 1)
			{
				projectiles1.push_back(iceshard);
			}
			else {
				projectiles2.push_back(iceshard);
			}
		}
	}
}


bool canCreateFireOrb(Mage &player) {
	if (player.mp<MP_ORB || player.fireOrbs.size()>MAX_NORB)
	{
		return false;
	}
	return true;
}

bool canCreateIceOrb(Mage &player) {
	if (player.mp<MP_ORB || player.iceOrbs.size()>MAX_NORB)
	{
		return false;
	}
	return true;
}

void tackleEvent(SDL_Event event) {
	if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
		DONE = true;
	}
	//key board events
	Orb newOrb(player1.r.x, player1.r.y, 0.025);
	if (event.type == SDL_KEYDOWN) {
		//use booleans player up, player down to down
		//to decide if the player is pressing a key to move
		switch (event.key.keysym.sym) {
		case SDLK_w:
			player1MoveUp = true;
			break;
		case SDLK_s:
			player1MoveDown = true;
			break;
		case SDLK_a:
			player1MoveLeft = true;
			break;
		case SDLK_d:
			player1MoveRight = true;
			break;
		case SDLK_g:
			if (canCreateFireOrb(player1))
			{
				player1.fireOrbs.push_back(createOrb(player1));
			}
			break;
		case SDLK_h:
			if (canCreateIceOrb(player1))
			{
				player1.iceOrbs.push_back(createOrb(player1));
			}
			break;
		case SDLK_r:
			createArcanebolt(1, player1);
			break;
		case SDLK_f:
			useWindBlast(player1,player2, projectiles1,projectiles2,creatures1, creatures2);
			break;
		case SDLK_t:
			if (player1.fireOrbs.size()>player1.iceOrbs.size())
			{
				createFireball(1, player1);
			}
			else if(player1.fireOrbs.size()<player1.iceOrbs.size()) {
				createIceshard(1, player1);
			}
			else {
				createShadowDagger(1, player1);
			}
			break;
		case SDLK_y:
			if (player1.fireOrbs.size() > player1.iceOrbs.size())
			{
				createFireghost(1, player1);
			}
			else if (player1.fireOrbs.size() < player1.iceOrbs.size()) {
				createIcetotem(1, player1);
			}
			else {
				createSwallow(1, player1);
			}
			break;
		case SDLK_LEFT:
			player2MoveLeft = true;
			break;
		case SDLK_RIGHT:
			player2MoveRight = true;
			break;
		case SDLK_UP:
			player2MoveUp = true;
			break;
		case SDLK_DOWN:
			player2MoveDown = true;
			break;
		case SDLK_l:
			if (canCreateFireOrb(player2))
			{
				player2.fireOrbs.push_back(createOrb(player2));
			}
			break;
		case SDLK_SEMICOLON:
			if (canCreateIceOrb(player2))
			{
				player2.iceOrbs.push_back(createOrb(player2));
			}
			break;
		case SDLK_i:
			createArcanebolt(2, player2);
			break;
		case SDLK_o:
			if (player2.fireOrbs.size()>player2.iceOrbs.size())
			{
				createFireball(2, player2);
			}
			else if (player2.fireOrbs.size()<player2.iceOrbs.size()) {
				createIceshard(2, player2);
			}
			else {
				createShadowDagger(2, player2);
			}
			break;
		case SDLK_p:
			if (player2.fireOrbs.size() > player2.iceOrbs.size())
			{
				createFireghost(2, player2);
			}
			else if (player2.fireOrbs.size() < player2.iceOrbs.size()) {
				createIcetotem(2, player2);
			}
			else {
				createSwallow(2, player2);
			}
			break;
		case SDLK_k:
			useWindBlast(player2, player1,projectiles2, projectiles1, creatures2,creatures1);
			break;
		case SDLK_0:
			if (ENDOFGAME>0)
			{
				ENDOFGAME = 0;
				initEverything();
			}
			//Mix_PlayChannel(-1, gRock, 0); //the third argument is how many times to repeat it...
			//Mix_PlayChannel(-1, gBird, 0);
			break;
		case SDLK_9:
			//If there is no music playing
			if (Mix_PlayingMusic() == 0)
			{
				//Play the music
				Mix_PlayMusic(gMusic, -1);
			}
			//If music is being played
			else
			{
				//If the music is paused
				if (Mix_PausedMusic() == 1)
				{
					//Resume the music
					Mix_ResumeMusic();
				}
				//If the music is playing
				else
				{
					//Pause the music
					Mix_PauseMusic();
				}
			}
			break;
		default:
			break;
		}
	}
	else if (event.type == SDL_KEYUP) {
		switch (event.key.keysym.sym) {
		case SDLK_w:
			player1MoveUp = false;
			break;
		case SDLK_s:
			player1MoveDown = false;
			break;
		case SDLK_a:
			player1MoveLeft = false;
			break;
		case SDLK_d:
			player1MoveRight = false;
			break;
			//will fix direction etc later....
		case SDLK_LEFT:
			player2MoveLeft = false;
			break;
		case SDLK_RIGHT:
			player2MoveRight = false;
			break;
		case SDLK_UP:
			player2MoveUp = false;
			break;
		case SDLK_DOWN:
			player2MoveDown = false;
			break;
		default:
			break;
		}
	}
}

void doNotLeaveWindow(Rect &r) {
	if (r.x > ORTHO_HALF_WIDTH) {
		r.x = ORTHO_HALF_WIDTH;
	}
	if (r.x < -ORTHO_HALF_WIDTH) {
		r.x = -ORTHO_HALF_WIDTH;
	}
	if (r.y < -ORTHO_HALF_HEIGHT) {
		r.y = -ORTHO_HALF_HEIGHT;
	}
	if (r.y > ORTHO_HALF_HEIGHT) {
		r.y = ORTHO_HALF_HEIGHT;
	}
}

bool projLeftWindow(Projectile &p) {
	float margin = 0.1;
	if (p.r.x > ORTHO_HALF_WIDTH+ margin|| p.r.x < -ORTHO_HALF_WIDTH - margin||
		p.r.y >ORTHO_HALF_HEIGHT+margin|| p.r.y<-ORTHO_HALF_HEIGHT - margin) {
		return true;
	}
	return false;
}

void updateCreaturePhase(Creature &c) {
	if (c.changePhaseTimer<=0)
	{
		c.changePhaseTimer += c.changePhaseInterval;
		c.phase = (c.phase + 1) % 2;
	}
	c.changePhaseTimer -= ELAPSED;
}

void restoreMana(Mage &player,float rate) {
	//rate is how much mana restore per second
	player.mp += rate*ELAPSED;
	if (player.mp>player.maxMp)
	{
		player.mp = player.maxMp;
	}
}

void updateCoolDown(Mage &player) {
	player.windBlastTimer -= ELAPSED;
	if (player.windBlastTimer < 0)
	{
		player.windBlastTimer = 0;
	}
}

void checkSpeedRecover(Mage &player) {
	if (player.isslowed) {
		player.slowtimer -= ELAPSED;
		if (player.slowtimer<=0)
		{
			player.isslowed = false;
		}
	}
}

void keepAway(Rect &obj, Rect &obstacle) {
	//will keep a obj from a obstacle
	float radian = getRadianToTarget(obstacle, obj);
	float margin = 0.01;
	float distance = obj.w / 2 + obstacle.w / 2 + margin;
	float newX = obstacle.x + distance*cosf(radian);
	float newY = obstacle.y + distance*sinf(radian);
	obj.x = newX;
	obj.y = newY;
}

void dontMoveIntoTotem(Mage &player, vector<Creature> &cs1, vector<Creature> &cs2) {
	//will prevent a player from stepping onto a totem (or other obstacle) on the field.
	for (size_t i = 0; i < cs1.size(); i++)
	{
		if (cs1[i].ctype==ICETOTEM && checkCollisionCircle(player.r,cs1[i].r))
		{
			keepAway(player.r, cs1[i].r);
		}
	}

	for (size_t i = 0; i < cs2.size(); i++)
	{
		if (cs2[i].ctype == ICETOTEM && checkCollisionCircle(player.r, cs2[i].r))
		{
			keepAway(player.r, cs2[i].r);
		}
	}
}

void doLogistics() {
	//now I'm also putting some logistics about time her.e..
	//such as time elapsed for effects..
	
	doNotLeaveWindow(player1.r);
	doNotLeaveWindow(player2.r);
	updateCoolDown(player1);
	updateCoolDown(player2);

	dontMoveIntoTotem(player1, creatures1, creatures2);
	dontMoveIntoTotem(player2, creatures2, creatures1);

	//replenish mana
	restoreMana(player1, MP_RESTORE_RATE);
	restoreMana(player2, MP_RESTORE_RATE);

	checkSpeedRecover(player1);
	checkSpeedRecover(player2);

	for (size_t i = 0; i < creatures1.size(); i++)
	{
		doNotLeaveWindow(creatures1[i].r);
		updateCreaturePhase(creatures1[i]);
		if (creatures1[i].hp<=0)
		{
			creatures1.erase(creatures1.begin()+i);
		}
	}
	for (size_t i = 0; i < creatures2.size(); i++)
	{
		doNotLeaveWindow(creatures2[i].r);
		updateCreaturePhase(creatures2[i]);
		if (creatures2[i].hp <= 0)
		{
			creatures2.erase(creatures2.begin() + i);
		}
	}
	for (size_t i = 0; i < projectiles1.size(); i++)
	{
		if (projLeftWindow(projectiles1[i])) {
			projectiles1.erase(projectiles1.begin() + i);
		}
	}
	for (size_t i = 0; i < projectiles2.size(); i++)
	{
		if (projLeftWindow(projectiles2[i])) {
			projectiles2.erase(projectiles2.begin() + i);
		}
	}
	for (size_t i = 0; i < neutralEffects.size(); i++)
	{
		if (neutralEffects[i].lifetime<=0)//erase an effect if its lifetime is end... used effects may still stay on
			//the map for a while...
		{
			neutralEffects.erase(neutralEffects.begin() + i);
		}
		else {
			neutralEffects[i].lifetime -= ELAPSED;
		}
	}
}

void drawPlayers() {
	drawSpriteRot(player1.r, mage1Texture, directionToAngle(player1.direction));
	drawLife(player1.r, player1.maxHp, player1.hp);
	drawMana(player1.r, player1.maxMp, player1.mp);
	drawSpriteRot(player2.r, mage2Texture, directionToAngle(player2.direction));
	drawLife(player2.r, player2.maxHp, player2.hp);
	drawMana(player2.r, player2.maxMp, player2.mp);
}

void drawProjectile(Projectile p) {
	GLuint texture;
	switch (p.ptype)
	{
	case FIREBALL:
		texture = fireballTexture;
		break;
	case ICESHARD:
		texture = iceShardTexture;
		break;
	case ARCANE:
		texture = arcaneBoltTexture;
		break;
	case SHADOWDAGGER:
		texture = shadowDaggerTexture;
		break;
	case ICEBLADE:
		texture = iceBladeTexture;
		break;
	default:
		texture = fireballTexture;
		break;
	}
	drawSpriteRot(p.r, texture,p.angle);
}

void drawCreatures() {
	for (size_t i = 0; i < creatures1.size(); i++)
	{
		drawCreature(creatures1[i],1);
	}
	for (size_t i = 0; i < creatures2.size(); i++)
	{
		drawCreature(creatures2[i],2);
	}
}

void drawProjectiles() {
	for (size_t i = 0; i < projectiles1.size(); i++)
	{
		drawProjectile(projectiles1[i]);
	}
	for (size_t i = 0; i < projectiles2.size(); i++)
	{
		drawProjectile(projectiles2[i]);
	}
}

void drawOrbs(Mage player) {
	//this will draw all the 3 types of orbs of a player...
	int n = player.fireOrbs.size();
	if (n>0)
	{
		for (size_t i = 0; i < n; i++)
		{
			drawSprite(player.fireOrbs[i].r, fireOrbTexture);
		}
	}
	n = player.iceOrbs.size();
	if (n>0)
	{
		for (size_t i = 0; i < n; i++)
		{
			drawSprite(player.iceOrbs[i].r, iceOrbTexture);
		}
	}
	n = player.windOrbs.size();
	if (n>0)
	{
		for (size_t i = 0; i < n; i++)
		{
			drawSprite(player.windOrbs[i].r, windOrbTexture);
		}
	}
}

void drawEffect(Effect e) {
	GLuint texture;
	switch (e.type)
	{
	case WINDBLAST:
		texture = windblastTexture;
		break;
	default:
		texture = aoeTexture;
		break;
	}
	drawSprite(e.r, texture);
}

void drawEffects() {
	for (size_t i = 0; i < neutralEffects.size(); i++)
	{
		drawEffect(neutralEffects[i]);
	}
}

void drawEverything() {
	background();
	drawSprite(bg, bgTexture);

	drawPlayers();

	drawOrbs(player1);
	drawOrbs(player2);

	drawProjectiles();
	drawCreatures();

	drawEffects();

	if (ENDOFGAME==1)
	{
		drawSprite({ 0,0,ORTHO_HALF_WIDTH*2,ORTHO_HALF_HEIGHT* 2 }, redwinTexture);
	}
	else if (ENDOFGAME == 2) {
		drawSprite({ 0,0,ORTHO_HALF_WIDTH*2,ORTHO_HALF_HEIGHT * 2 }, bluewinTexture);
	}
}

float getRandomNumber(float a, float b) {
	//get a random number between a,b, a<b
	float scope = b - a;
	float rr = ((float)rand() / RAND_MAX)*scope + a;
	return rr;
}

float getReflect(Projectile proj, Rect shield) {
	//will return the angle of reflection
	float playerToProj = atan2(proj.r.y - shield.y, proj.r.x - shield.x);
	return playerToProj;
}

float getDistance(Rect r1, Rect r2) {
	return sqrtf((r1.x - r2.x)*(r1.x - r2.x) + (r1.y - r2.y)*(r1.y - r2.y));
}

float getRadianToTarget(Rect r1, Rect r2) {
	float angle = atan2(r2.y - r1.y, r2.x - r1.x);
	return angle;
}

void testCode() {
	enum Color{red, green, blue};
	Color r = red;
	if (r==red)
	{
		cout << "testing enum!\n";
	}
	Creature c1(44,55,66);
	vector<Creature> cc;
	cc.push_back(c1);
	Creature *cp = &c1;
	cout << "sss\n";
	cout << cp->r.x << endl;
	cc.pop_back();
	cout << "sss\n";
	cout << cp->r.x << endl;
}

Direction getPlayer1Direction() {//will return the new direction that player 1 is facing, if player1 moves..
	Direction d = player1.direction;
	if (player1MoveLeft&&player1MoveUp)
	{
		d = d_leftup;
	}
	else if (player1MoveLeft&&player1MoveDown) {
		d = d_leftdown;
	}
	else if(player1MoveLeft){
		d = d_left;
	}
	else if (player1MoveRight&&player1MoveUp) {
		d = d_rightup;
	}
	else if (player1MoveRight&&player1MoveDown) {
		d = d_rightdown;
	}
	else if (player1MoveRight) {
		d = d_right;
	}
	else if (player1MoveUp){
		d = d_up;
	}
	else if (player1MoveDown) {
		d = d_down;
	}
	return d;
}

Direction getPlayer2Direction() {//will return the new direction that player 2 is facing, if it moves..
	Direction d = player2.direction;
	if (player2MoveLeft&&player2MoveUp)
	{
		d = d_leftup;
	}
	else if (player2MoveLeft&&player2MoveDown) {
		d = d_leftdown;
	}
	else if (player2MoveLeft) {
		d = d_left;
	}
	else if (player2MoveRight&&player2MoveUp) {
		d = d_rightup;
	}
	else if (player2MoveRight&&player2MoveDown) {
		d = d_rightdown;
	}
	else if (player2MoveRight) {
		d = d_right;
	}
	else if (player2MoveUp) {
		d = d_up;
	}
	else if (player2MoveDown) {
		d = d_down;
	}
	return d;
}

int directionToAngle(Direction d) {
	int angle = 0;
	switch (d) {
	case d_up:
		angle = 0;
		break;
	case d_leftup:
		angle = 45;
		break;
	case d_left:
		angle = 90;
		break;
	case d_leftdown:
		angle = 135;
		break;
	case d_down:
		angle = 180;
		break;
	case d_rightdown:
		angle = 225;
		break;
	case d_right:
		angle = 270;
		break;
	case d_rightup:
		angle = 315;
		break;
	default:
		break;
	}
	angle = (angle+90)%360;//now angle start from positive x axis...
	
	return angle;
}

void updatePlayersDirections() {
	player1.direction = getPlayer1Direction();
	player2.direction = getPlayer2Direction();
}

void playersMove() {

	if (player1.forceMoveTimer>0)
	{
		player1.r.x += player1.forceMoveSpeed * ELAPSED * cosf(player1.forceMoveAngle*PI / 180);
		player1.r.y += player1.forceMoveSpeed * ELAPSED * sinf(player1.forceMoveAngle*PI / 180);
	}else //else no force move
	if (player1MoveUp || player1MoveDown || player1MoveLeft || player1MoveRight) {
		if (!player1.isslowed)
		{
			moveAccordingToDirection(player1.r, player1.speed, player1.direction);
		}
		else {
			//if player is slowed down 
			moveAccordingToDirection(player1.r, player1.speed/SHADOWDAGGER_SLOWDIVIDER, player1.direction);
		}
	}
	player1.forceMoveTimer -= ELAPSED;

	if (player2.forceMoveTimer>0)
	{
		player2.r.x += player2.forceMoveSpeed * ELAPSED * cosf(player2.forceMoveAngle*PI / 180);
		player2.r.y += player2.forceMoveSpeed * ELAPSED * sinf(player2.forceMoveAngle*PI / 180);
	}else if (player2MoveUp || player2MoveDown || player2MoveLeft || player2MoveRight) {
		if (!player2.isslowed)
		{
			moveAccordingToDirection(player2.r, player2.speed, player2.direction);
		}
		else {
			moveAccordingToDirection(player2.r, player2.speed/ SHADOWDAGGER_SLOWDIVIDER, player2.direction);
		}
	}

	player2.forceMoveTimer -= ELAPSED;
}

void moveAccordingToDirection(Rect &r, float speed, Direction d) {
	float moveDistance = speed * ELAPSED;
	float diagonalDistance = moveDistance*cosf(45);
	switch (d)
	{
	case d_left:
		r.x -= moveDistance;
		break;
	case d_right:
		r.x += moveDistance;
		break;
	case d_up:
		r.y += moveDistance;
		break;
	case d_down:
		r.y -= moveDistance;
		break;
	case d_leftup:
		r.x -= diagonalDistance;
		r.y += diagonalDistance;
		break;
	case d_leftdown:
		r.x -= diagonalDistance;
		r.y -= diagonalDistance;
		break;
	case d_rightup:
		r.x += diagonalDistance;
		r.y += diagonalDistance;
		break;
	case d_rightdown:
		r.x += diagonalDistance;
		r.y -= diagonalDistance;
		break;
	default:
		break;
	}
}


void moveAccordingToRadian(Rect &r, float speed, float angle) {
	float moveDistance = speed * ELAPSED;
	r.x += moveDistance * cosf(angle);
	r.y += moveDistance * sinf(angle);
}

void orbMove(Mage player,vector<Orb> &orbs, float baseAngle, float extraRadius) {
	int n = orbs.size();
	if (n<=0)
	{
		return;
	}
	float anglepiece = 360 / n;
	float orbSpeed = 1.5;
	float radius = 0.08 + 0.002*n+ extraRadius;

	for (size_t i = 0; i < n; i++)
	{
		float angle = anglepiece*i;
		float radian = baseAngle + angle*PI / 180;
		orbs[i].targetX = player.r.x + radius*cosf(radian);
		orbs[i].targetY = player.r.y + radius*sinf(radian);
	}

	for (size_t i = 0; i < n; i++)
	{
		Rect targetRect = { orbs[i].targetX ,orbs[i].targetY,0,0 };
		if (getDistance(orbs[i].r, targetRect)>0.007)
		{
			float moveRadian = getRadianToTarget(orbs[i].r, targetRect);
			moveAccordingToRadian(orbs[i].r, orbSpeed, moveRadian);
		}
		else {
			orbs[i].r.x = orbs[i].targetX;
			orbs[i].r.y = orbs[i].targetY;
		}
	}
}

void updateSwallowState(Creature &c) {
	if (c.stateTimer<=0)
	{
		int nextState = (c.state + 1) % c.nOfStates;
		c.stateTimer += c.stateDurations[nextState];
		c.state = nextState;
	}
	c.stateTimer -= ELAPSED;
}

Rect pickRandomEnemyCreatureRect(const vector<Creature> &enemyCs) {
	int n = enemyCs.size();
	int rr = rand() % n;
	Rect toReturn = enemyCs[rr].r;
	return toReturn;
}

void setAi(Mage &ourplayer, Mage &enemyPlayer, vector<Creature> &ourcs, const vector<Creature> &enemyCs, vector<Projectile> &ourprojs){
	float radian;
	for (size_t i = 0; i < ourcs.size(); i++)
	{
		switch (ourcs[i].ctype)
		{
		case ICETOTEM://icetotem: when it can attack, with some probability, attack player, otherwise attack nearest creature. (when there is enemy creature)
			ourcs[i].dx = 0;
			ourcs[i].dy = 0;
			if (ourcs[i].reloadTimer <= 0)
			{
				ourcs[i].reloadTimer += ourcs[i].reloadInterval;
				Rect target;
				if (enemyCs.size()>0 && getRandomNumber(0,1)>0.1)
				{
					//shoot enemy creature
					target = pickRandomEnemyCreatureRect(enemyCs);
				}
				else {
					//shoot player..
					target = enemyPlayer.r;
				}

				for (size_t j = 0; j < 3; j++)
				{
					ourprojs.push_back(createCreatureProjectile(ourcs[i].r, target, ICEBLADE));
				}
			}
			ourcs[i].reloadTimer -= ELAPSED;
			break;
		case FIREGHOST:
			if (ourcs[i].changeTargetTimer<=0)
			{
				//if change target...
				ourcs[i].changeTargetTimer += ourcs[i].changeTargetInterval;
				ourcs[i].targetRect = { enemyPlayer.r.x+getRandomNumber(-0.1,0.1),enemyPlayer.r.y + getRandomNumber(-0.1,0.1),0,0 };
			}

			ourcs[i].moving = true;
			radian = getRadianToTarget(ourcs[i].r, ourcs[i].targetRect);
			ourcs[i].dx = ourcs[i].speed * cosf(radian);
			ourcs[i].dy = ourcs[i].speed * sinf(radian);

			ourcs[i].changeTargetTimer -= ELAPSED;
			break;
		case SWALLOW:
			updateSwallowState(ourcs[i]);
			switch (ourcs[i].state)
			{
			case 0: //flying
				ourcs[i].moving = true;

				//cout << 0<<endl;
				break;
			case 1: //fire
				//fire proj and set next move target position
				ourcs[i].targetRect = { getRandomNumber(-1,1),getRandomNumber(-1,1) ,0,0 };
				radian = getRadianToTarget(ourcs[i].r, ourcs[i].targetRect);
				ourcs[i].dx = ourcs[i].speed * cosf(radian);
				ourcs[i].dy = ourcs[i].speed * sinf(radian);
				ourcs[i].moving = false;
				ourcs.push_back(createMissle(ourcs[i].r)); //swallow will create a missle, which is also a creature
				//ourprojs.push_back(createCreatureProjectile(ourcs[i].r, enemyPlayer.r, ICEBLADE));
				//cout << 0 << endl;
				break;
			case 2: //wait
				
				//cout << 2 << endl;
				break;
			default:
				break;
			}
			break;
		case MISSLE:
			ourcs[i].changeTargetTimer += ourcs[i].changeTargetInterval;
			ourcs[i].targetRect = { enemyPlayer.r.x + getRandomNumber(-0.1,0.1),enemyPlayer.r.y + getRandomNumber(-0.1,0.1),0,0 };

			ourcs[i].moving = true;
			radian = getRadianToTarget(ourcs[i].r, ourcs[i].targetRect);
			ourcs[i].dx = ourcs[i].speed * cosf(radian);
			ourcs[i].dy = ourcs[i].speed * sinf(radian);
			break;
		default:
			break;
		}
	}
}

void aiAll() {
	setAi(player1, player2, creatures1, creatures2, projectiles1);
	setAi(player2, player1, creatures2, creatures1, projectiles2);
	//this function will iterate through all the creatures...
	//add to projectiles if they fire sth...
	//and set up each creature's dx and dy 
	//so creaturesMoveAll can manage their movements...
}

void creaturesMoveAll() {
	//at the ai part, each creature needs to set up its dx and dy...
	//if creature is being force moved.. then the creature will not be able to move...
	for (size_t i = 0; i < creatures1.size(); i++)
	{
		if (creatures1[i].forceMoveTimer>0)
		{
			creatures1[i].r.x += creatures1[i].forceMoveSpeed * ELAPSED * cosf(creatures1[i].forceMoveAngle*PI/180);
			creatures1[i].r.y += creatures1[i].forceMoveSpeed * ELAPSED * sinf(creatures1[i].forceMoveAngle*PI / 180);
		}else if (creatures1[i].moving)
		{
			creatures1[i].r.x += creatures1[i].dx * ELAPSED;
			creatures1[i].r.y += creatures1[i].dy * ELAPSED;
		}

		creatures1[i].forceMoveTimer -= ELAPSED;
	}
	for (size_t i = 0; i < creatures2.size(); i++)
	{
		if (creatures2[i].forceMoveTimer>0)
		{
			creatures2[i].r.x += creatures2[i].forceMoveSpeed * ELAPSED * cosf(creatures2[i].forceMoveAngle*PI / 180);
			creatures2[i].r.y += creatures2[i].forceMoveSpeed * ELAPSED * sinf(creatures2[i].forceMoveAngle*PI / 180);
		}
		else if (creatures2[i].moving)
		{
			creatures2[i].r.x += creatures2[i].dx * ELAPSED;
			creatures2[i].r.y += creatures2[i].dy * ELAPSED;
		}
		creatures2[i].forceMoveTimer -= ELAPSED;
	}
}

void projectileMoveAll() {
	for (size_t i = 0; i < projectiles1.size(); i++)
	{
		moveAccordingToRadian(projectiles1[i].r, projectiles1[i].speed, angleToRadian(projectiles1[i].angle));
	}
	for (size_t i = 0; i < projectiles2.size(); i++)
	{
		moveAccordingToRadian(projectiles2[i].r, projectiles2[i].speed, angleToRadian(projectiles2[i].angle));
	}

}

void orbMoveAll() {
	orbMove(player1,player1.fireOrbs, ORB_BASE_ANGLE*1.5, 0);
	orbMove(player1, player1.iceOrbs, ORB_BASE_ANGLE*1.3 + 0.5 , 0.012);
	orbMove(player1, player1.windOrbs, ORB_BASE_ANGLE*1 + 1, 0.024);

	orbMove(player2,player2.fireOrbs, ORB_BASE_ANGLE*1.5, 0);
	orbMove(player2, player2.iceOrbs, ORB_BASE_ANGLE*1.3 + 0.8, 0.012);
	orbMove(player2, player2.windOrbs, ORB_BASE_ANGLE*1 + 1.2, 0.024);
}


float angleToRadian(float angle) {
	return angle*PI / 180;
}

//draw sth, pose means which frame of animation to draw
void drawSpritePhase(Rect rect, GLuint texture, int pose) {
	//should draw with animation according to phase value
	GLfloat x = rect.x;
	GLfloat y = rect.y;
	GLfloat w = rect.w;
	GLfloat h = rect.h;
	// Turn on texturing so We can apply a texture to our shape
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);

	GLfloat quad[] = { -w / 2,-h / 2,w / 2,-h / 2,w / 2,h / 2,-w / 2,h / 2 };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs1[] = { 0.0, 1.0,
		0.5, 1.0,
		0.5, 0.0,
		0.0, 0.0 };
	GLfloat quadUVs2[] = { 0.5, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.5, 0.0 };

	if (pose == 0)
	{
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs1);
	}
	else {
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs2);
	}

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}


