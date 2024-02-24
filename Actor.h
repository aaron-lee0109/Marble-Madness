#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* sw, int id, int x, int y, int dir);
	virtual void doSomething();
	StudentWorld* getWorld();
	virtual bool isActive() const;
	virtual bool getIsAlive() const;
	void setIsAlive(bool alive);
	virtual bool canStepOn() const;
	virtual void damage();
	virtual int getShootable() const;
	virtual bool canHaveMarbleOver() const;
	virtual bool canFillPit() const;
	virtual bool canShootOver() const;
	virtual bool canBeThieved() const;
	virtual void setIsActive(bool active);
	virtual bool madeByFactory() const;
protected:
	void setShootable(int shoot);
private:
	StudentWorld* m_world;
	bool isAlive;
	int shootable; // 1 = shootable, 2 = pass through, 3 = dies
};

class AttackableActor : public Actor
{
public:
	AttackableActor(StudentWorld* sw, int id, int x, int y, int dir, int health);
	int getHealth() const;
	void setHealth(int health);
	virtual bool canMoveForward(int x, int y);
	virtual void damage();
	virtual bool canShootOver() const;
private:
	int m_health;
	int m_totalHealth;
};

class StudentWorld;

class Avatar : public AttackableActor
{
public:
	Avatar(StudentWorld* sw, int id, int x, int y, int dir, int health, int ammo);
	virtual void doSomething();
	int getAmmo() const;
	void addAmmo(int add);
	virtual bool canMoveForward(int x, int y);
	virtual void damage();

private:
	int m_ammo;

};

class Bots : public AttackableActor
{
public:
	Bots(StudentWorld* sw, int id, int x, int y, int dir, int health);
	int movementSpeed();
	int getTickCount() const;
	void increaseTickCount();
	virtual bool canMoveForward(int x, int y);
	bool clearLineOfSight(int x1, int y1, int x2, int y2);

private:
	int tickCount;

};

class RageBots : public Bots
{
public:
	RageBots(StudentWorld* sw, int id, int x, int y, int dir, int health);
	void doSomething();
	virtual void damage();

private:

};

class ThiefBots : public Bots
{
public:
	ThiefBots(StudentWorld* sw, int id, int x, int y, int dir, int health);
	void doSomething();
	virtual void damage();
	void newDistance();
	void decreaseDistanceLeft();
	int getDistanceLeft() const;
	bool move();
	virtual bool madeByFactory() const;
protected:
	Actor* getLoot();
	void setLoot(Actor* object);
private:
	Actor* loot;
	int distanceLeft;

};

class MeanThiefBots : public ThiefBots
{
public:
	MeanThiefBots(StudentWorld* sw, int id, int x, int y, int dir, int health);
	void doSomething();
	virtual void damage();
private:

};

class Factories : public Actor
{
public:
	Factories(StudentWorld* sw, int id, int x, int y, int dir, bool reg);
	void doSomething();
	virtual bool canShootOver() const;
private:
	// true = thiefbot factory, false = mean thiefbot factory
	bool regularFactory;
};

class Peas : public Actor
{
public:
	Peas(StudentWorld* sw, int id, int x, int y, int dir);
	virtual bool canStepOn() const;
	virtual void doSomething();

private:

};

class Exits : public Actor
{
public:
	Exits(StudentWorld* sw, int id, int x, int y, int dir);
	virtual void doSomething();
	virtual bool canStepOn() const;
private:
	bool revealed;

};

class Walls : public Actor
{
public:
	Walls(StudentWorld* sw, int id, int x, int y, int dir);
	virtual bool isActive() const;
	virtual bool canShootOver() const;
private:

};

class Marbles : public AttackableActor
{
public:
	Marbles(StudentWorld* sw, int id, int x, int y, int dir, int health);
	virtual bool isActive() const;
	virtual void damage();
	virtual bool canFillPit() const;
private:

};

class Pits : public Actor
{
public:
	Pits(StudentWorld* sw, int id, int x, int y, int dir);
	virtual void doSomething();
	virtual bool canHaveMarbleOver() const;
private:

};


class Goodies : public Actor
{
public:
	Goodies(StudentWorld* sw, int id, int x, int y, int dir);
	virtual bool canStepOn() const;
	bool playerIsOn();
	virtual bool canBeThieved() const;
	virtual bool isActive() const;
	virtual void setIsActive(bool active);
private:
	bool active;
};

class Crystals : public Goodies
{
public:
	Crystals(StudentWorld* sw, int id, int x, int y, int dir);
	virtual void doSomething();
	virtual bool canBeThieved() const;
	virtual bool isActive() const;
	virtual void setIsActive(bool active);
private:

};

class ExtraLifeGoodies : public Goodies
{
public:
	ExtraLifeGoodies(StudentWorld* sw, int id, int x, int y, int dir);
	virtual void doSomething();
private:

};

class RestoreHealthGoodies : public Goodies
{
public:
	RestoreHealthGoodies(StudentWorld* sw, int id, int x, int y, int dir);
	virtual void doSomething();
private:

};

class AmmoGoodies : public Goodies
{
public:
	AmmoGoodies(StudentWorld* sw, int id, int x, int y, int dir);
	virtual void doSomething();
private:

};

#endif // ACTOR_H_