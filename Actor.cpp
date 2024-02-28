#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>
using namespace std;

// Actor method implementations
Actor::Actor(StudentWorld* sw, int id, int x, int y, int dir)
	: GraphObject(id, x, y, dir), m_world(sw), isAlive(true)
	, shootable(3)
{
	setVisible(true);
}

void Actor::doSomething() {
	return;
}

StudentWorld* Actor::getWorld()
{
	return m_world;
}

bool Actor::isActive() const
{
	return true;
}

bool Actor::getIsAlive() const
{
	return isAlive;
}

void Actor::setIsAlive(bool alive)
{
	isAlive = alive;
}

bool Actor::canStepOn() const
{
	return false;
}

void Actor::damage()
{
	return;
}

int Actor::getShootable() const
{
	return shootable;
}

bool Actor::canHaveMarbleOver() const
{
	return false;
}

bool Actor::canFillPit() const
{
	return false;
}


bool Actor::canShootOver() const
{
	return true;
}

bool Actor::canBeThieved() const
{
	return false;
}

void Actor::setShootable(int shoot)
{
	shootable = shoot;
}

void Actor::setIsActive(bool active)
{
	return;
}

bool Actor::madeByFactory() const
{
	return false;
}

// AttackableActor method implementations
AttackableActor::AttackableActor(StudentWorld* sw, int id, int x, int y, int dir, int health)
	: Actor(sw, id, x, y, dir), m_health(health)
{
	setShootable(1);
}

int AttackableActor::getHealth() const
{
	return m_health;
}

void AttackableActor::setHealth(int health)
{
	m_health = health;
}

bool AttackableActor::canMoveForward(int x, int y)
{
	return true;
}

void AttackableActor::damage()
{
	return;
}

bool AttackableActor::canShootOver() const
{
	return false;
}

// Avatar method implementations
Avatar::Avatar(StudentWorld* sw, int id, int x, int y, int dir, int health, int ammo)
	: AttackableActor(sw, id, x, y, dir, health), m_ammo(ammo)
{

}

void Avatar::doSomething()
{
	// check if alive
	if (!this->getIsAlive())
		return;

	int ch;
	if (getWorld()->getKey(ch)) {
		// user hits a key this tick
		int x, y;
		Actor* actor1, * actor2;
		switch (ch)
		{
		case KEY_PRESS_ESCAPE:
			this->setIsAlive(false);
			break;
		case KEY_PRESS_SPACE:
			if (m_ammo > 0) {
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				m_ammo--;

				int dir = getDirection();
				switch (dir) {
				case GraphObject::left:
					x = getX() - 1;
					y = getY();
					break;
				case GraphObject::right:
					x = getX() + 1;
					y = getY();
					break;
				case GraphObject::down:
					x = getX();
					y = getY() - 1;
					break;
				case GraphObject::up:
					x = getX();
					y = getY() + 1;
					break;
				}
				// create pea
				getWorld()->addItem(new Peas(getWorld(), IID_PEA, x, y, dir));
			}
			break;
		case KEY_PRESS_LEFT:
			setDirection(GraphObject::left);
			x = getX(), y = getY();
			// marble logic
			actor1 = getWorld()->objectAtLocation(x - 1, y);
			if (actor1 != nullptr && actor1->canFillPit()) {
				actor2 = getWorld()->objectAtLocation(x - 2, y);
				if (actor2 == nullptr || actor2->canHaveMarbleOver())
					actor1->moveTo(x - 2, y);
			}
			// otherwise check if can move forward
			if (canMoveForward(getX() - 1, getY()))
				this->moveTo(getX() - 1, getY());
			break;

		case KEY_PRESS_RIGHT:
			setDirection(GraphObject::right);
			x = getX(), y = getY();
			// checking marble logic
			actor1 = getWorld()->objectAtLocation(x + 1, y);
			if (actor1 != nullptr && actor1->canFillPit()) {
				actor2 = getWorld()->objectAtLocation(x + 2, y);
				if (actor2 == nullptr || actor2->canHaveMarbleOver())
					actor1->moveTo(x + 2, y);
			}
			// otherwise check if can move forward
			if (canMoveForward(getX() + 1, getY()))
				this->moveTo(getX() + 1, getY());
			break;
		case KEY_PRESS_DOWN:
			setDirection(GraphObject::down);
			x = getX(), y = getY();
			// checking marble logic
			actor1 = getWorld()->objectAtLocation(x, y - 1);
			if (actor1 != nullptr && actor1->canFillPit()) {
				actor2 = getWorld()->objectAtLocation(x, y - 2);
				if (actor2 == nullptr || actor2->canHaveMarbleOver())
					actor1->moveTo(x, y - 2);
			}
			// check if can move forward
			if (canMoveForward(getX(), getY() - 1))
				this->moveTo(getX(), getY() - 1);
			break;
		case KEY_PRESS_UP:
			setDirection(GraphObject::up);
			x = getX(), y = getY();
			// checking marble logic
			actor1 = getWorld()->objectAtLocation(x, y + 1);
			if (actor1 != nullptr && actor1->canFillPit()) {
				actor2 = getWorld()->objectAtLocation(x, y + 2);
				if (actor2 == nullptr || actor2->canHaveMarbleOver())
					actor1->moveTo(x, y + 2);
			}
			// check if can move forward
			if (canMoveForward(getX(), getY() + 1))
				this->moveTo(getX(), getY() + 1);
			break;
		default:
			break;
		}
	}

}

int Avatar::getAmmo() const
{
	return m_ammo;
}

void Avatar::addAmmo(int add)
{
	m_ammo += add;
}

bool Avatar::canMoveForward(int x, int y)
{
	Actor* atLocation = getWorld()->objectAtLocation(x, y);
	if (atLocation == nullptr)
		return true;
	return atLocation->canStepOn();
}

void Avatar::damage()
{
	setHealth(getHealth() - 2);
	if (getHealth() <= 0) {
		setIsAlive(false);
		getWorld()->playSound(SOUND_PLAYER_DIE);
	}
	else
		getWorld()->playSound(SOUND_PLAYER_IMPACT);
}

// Bots method implementations
Bots::Bots(StudentWorld* sw, int id, int x, int y, int dir, int health)
	: AttackableActor(sw, id, x, y, dir, health), tickCount(0)
{

}

int Bots::movementSpeed()
{
	int ticks = (28 - (getWorld()->getLevel())) / 4;
	if (ticks < 3)
		ticks = 3;
	return ticks;
}

int Bots::getTickCount() const
{
	return tickCount;
}

void Bots::increaseTickCount()
{
	tickCount++;
}

bool Bots::canMoveForward(int x, int y)
{
	Avatar* player = getWorld()->getPlayer();
	if (x == player->getX() && y == player->getY())
		return false;
	Actor* atLocation = getWorld()->objectAtLocation(x, y);
	if (atLocation == nullptr)
		return true;
	return atLocation->canStepOn();
}

bool Bots::clearLineOfSight(int x, int y, int playerX, int playerY)
// helper function used by bots who can shoot to determine if the line of sight is clear
{
	// Check if there are obstacles between bot and player
	if (x == playerX) {
		// Bot and player are on the same column
		int startY = min(y, playerY) + 1;
		int endY = max(y, playerY) - 1;
		for (int i = startY; i <= endY; i++) {
			Actor* object = getWorld()->objectAtLocation(x, i);
			if (object != nullptr && !object->canShootOver())
				return false;
		}
	}
	else if (y == playerY) {
		// Bot and player are on the same row
		int startX = min(x, playerX) + 1;
		int endX = max(x, playerX) - 1;
		for (int i = startX; i <= endX; i++) {
			Actor* object = getWorld()->objectAtLocation(i, y);
			if (object != nullptr && !object->canShootOver())
				return false;
		}
	}
	return true;
}

// RageBots method implementations
RageBots::RageBots(StudentWorld* sw, int id, int x, int y, int dir, int health)
	: Bots(sw, id, x, y, dir, health)
{

}

void RageBots::doSomething()
{
	// check if alive
	if (!getIsAlive())
		return;

	// check if can move this tick
	increaseTickCount();
	if (getTickCount() % movementSpeed() != 0)
		return;

	int x = getX();
	int y = getY();
	int playerX = getWorld()->getPlayer()->getX();
	int playerY = getWorld()->getPlayer()->getY();
	int dir = getDirection();

	// for each direction, determine if bot can shoot
	// otherwise determine if bot can move forward
	// otherwise turn around
	switch (dir) {
	case GraphObject::up:
		if (x == playerX && playerY > y && clearLineOfSight(x, y, playerX, playerY)) {
			getWorld()->addItem(new Peas(getWorld(), IID_PEA, x, y + 1, dir));
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			break;
		}
		if (canMoveForward(x, y + 1))
			moveTo(x, y + 1);
		else
			setDirection(GraphObject::down);
		break;
	case GraphObject::down:
		if (x == playerX && playerY < y && clearLineOfSight(x, y, playerX, playerY)) {
			getWorld()->addItem(new Peas(getWorld(), IID_PEA, x, y - 1, dir));
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			break;
		}
		if (canMoveForward(x, y - 1))
			moveTo(x, y - 1);
		else
			setDirection(GraphObject::up);
		break;
	case GraphObject::left:
		if (x > playerX && playerY == y && clearLineOfSight(x, y, playerX, playerY)) {
			getWorld()->addItem(new Peas(getWorld(), IID_PEA, x - 1, y, dir));
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			break;
		}
		if (canMoveForward(x - 1, y))
			moveTo(x - 1, y);
		else
			setDirection(GraphObject::right);
		break;
	case GraphObject::right:
		if (x < playerX && playerY == y && clearLineOfSight(x, y, playerX, playerY)) {
			getWorld()->addItem(new Peas(getWorld(), IID_PEA, x + 1, y, dir));
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			break;
		}
		if (canMoveForward(x + 1, y))
			moveTo(x + 1, y);
		else
			setDirection(GraphObject::left);
		break;
	}
}

void RageBots::damage()
{
	setHealth(getHealth() - 2);
	if (getHealth() <= 0) {
		setIsAlive(false);
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(100);
	}
	else
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
}

// ThiefBots method implementations
ThiefBots::ThiefBots(StudentWorld* sw, int id, int x, int y, int dir, int health)
	: Bots(sw, id, x, y, dir, health), loot(nullptr), distanceLeft(0)
{
	newDistance();
}

void ThiefBots::doSomething()
{
	if (!getIsAlive())
		return;

	// increase count, check if turn to move
	increaseTickCount();
	if (getTickCount() % movementSpeed() != 0)
		return;

	// check if bot can steal a goodie
	Actor* object = getWorld()->lootAtLocation(getX(), getY());
	if (getLoot() == nullptr && object != nullptr && randInt(1, 10) == 1) {
		setLoot(object);
		getLoot()->setVisible(false);
		getLoot()->setIsActive(false);
		getWorld()->playSound(SOUND_ROBOT_MUNCH);
		return;
	}

	// check if bot can move in the same diretion
	if (getDistanceLeft() > 0 && move())
		return;

	// select a new distance
	newDistance();
	// set new direction
	int dir = randInt(0, 3) * 90;
	setDirection(dir);
	int count = 0;
	while (!move() && count < 4) {
		setDirection(getDirection() + 90);
		count++;
	}
}

void ThiefBots::damage()
{
	setHealth(getHealth() - 2);
	if (getHealth() <= 0) {
		if (getLoot() != nullptr) {
			getLoot()->moveTo(getX(), getY());
			getLoot()->setVisible(true);
			getLoot()->setIsActive(true);
		}
		setIsAlive(false);
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(10);
	}
	else
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
}

void ThiefBots::newDistance()
{
	distanceLeft = randInt(1, 6);
}

void ThiefBots::decreaseDistanceLeft()
{
	distanceLeft--;
}

int ThiefBots::getDistanceLeft() const
{
	return distanceLeft;
}

bool ThiefBots::move()
// helper function to help facilitate movement of ThiefBots (and child class MeanThiefBots)
{
	int dir = getDirection();
	int x = getX(), y = getY();
	// for each direction move forward if able and return true
	// otherwise return false
	switch (dir) {
	case GraphObject::up:
		if (canMoveForward(x, y + 1)) {
			moveTo(x, y + 1);
			decreaseDistanceLeft();
			return true;
		}
		break;
	case GraphObject::down:
		if (canMoveForward(x, y - 1)) {
			moveTo(x, y - 1);
			decreaseDistanceLeft();
			return true;
		}
		break;
	case GraphObject::left:
		if (canMoveForward(x - 1, y)) {
			moveTo(x - 1, y);
			decreaseDistanceLeft();
			return true;
		}
		break;
	case GraphObject::right:
		if (canMoveForward(x + 1, y)) {
			moveTo(x + 1, y);
			decreaseDistanceLeft();
			return true;
		}
		break;
	}
	return false;
}

Actor* ThiefBots::getLoot()
{
	return loot;
}

void ThiefBots::setLoot(Actor* object)
{
	loot = object;
}

bool ThiefBots::madeByFactory() const
{
	return true;
}

// MeanThiefBots method implementations
MeanThiefBots::MeanThiefBots(StudentWorld* sw, int id, int x, int y, int dir, int health)
	: ThiefBots(sw, id, x, y, dir, health)
{

}

void MeanThiefBots::doSomething()
{
	if (!getIsAlive())
		return;

	// increase count, check if turn to move
	increaseTickCount();
	if (getTickCount() % movementSpeed() != 0)
		return;

	// check if bot can fire a pea
	int x = getX();
	int y = getY();
	int playerX = getWorld()->getPlayer()->getX();
	int playerY = getWorld()->getPlayer()->getY();
	int dir = getDirection();

	switch (dir) {
	case GraphObject::up:
		if (x == playerX && playerY > y && clearLineOfSight(x, y, playerX, playerY)) {
			getWorld()->addItem(new Peas(getWorld(), IID_PEA, x, y + 1, dir));
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			return;
		}
		break;
	case GraphObject::down:
		if (x == playerX && playerY < y && clearLineOfSight(x, y, playerX, playerY)) {
			getWorld()->addItem(new Peas(getWorld(), IID_PEA, x, y - 1, dir));
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			return;
		}
		break;
	case GraphObject::left:
		if (x > playerX && playerY == y && clearLineOfSight(x, y, playerX, playerY)) {
			getWorld()->addItem(new Peas(getWorld(), IID_PEA, x - 1, y, dir));
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			return;
		}
		break;
	case GraphObject::right:
		if (x < playerX && playerY == y && clearLineOfSight(x, y, playerX, playerY)) {
			getWorld()->addItem(new Peas(getWorld(), IID_PEA, x + 1, y, dir));
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			return;
		}
		break;
	}

	// check if bot can steal a goodie
	Actor* object = getWorld()->lootAtLocation(getX(), getY());
	if (getLoot() == nullptr && object != nullptr && randInt(1, 10) == 1) {
		setLoot(object);
		getLoot()->setVisible(false);
		getLoot()->setIsActive(false);
		getWorld()->playSound(SOUND_ROBOT_MUNCH);
		return;
	}

	// check if bot can move in the same diretion
	if (getDistanceLeft() > 0 && move())
		return;

	// select a new distance
	newDistance();
	// set new direction
	dir = randInt(0, 3) * 90;
	setDirection(dir);
	int count = 0;
	while (!move() && count < 4) {
		setDirection(getDirection() + 90);
		count++;
	}
}

void MeanThiefBots::damage()
{
	setHealth(getHealth() - 2);
	if (getHealth() <= 0) {
		if (getLoot() != nullptr) {
			getLoot()->moveTo(getX(), getY());
			getLoot()->setVisible(true);
			getLoot()->setIsActive(true);
		}
		setIsAlive(false);
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(20);
	}
	else
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
}

// Factories method implementations
Factories::Factories(StudentWorld* sw, int id, int x, int y, int dir, bool reg)
	: Actor(sw, id, x, y, dir), regularFactory(reg)
{

}

void Factories::doSomething()
{
	int count = 0;
	int x = getX(), y = getY();
	// get the boundaries of the square to check
	int x_min = max(x - 3, 1), x_max = min(x + 3, VIEW_WIDTH - 2);
	int y_min = max(y - 3, 1), y_max = min(y + 3, VIEW_HEIGHT - 2);
	// count theif bots in the square
	for (int i = x_min; i <= x_max; i++) {
		for (int j = y_min; j <= y_max; j++) {
			Actor* object = getWorld()->objectAtLocation(i, j);
			if (object != nullptr && object->madeByFactory())
				count++;
		}
	}
	// handle adding thief bots if applicable
	if (count < 3) {
		Actor* object = getWorld()->objectAtLocation(x, y);
		if (object != nullptr && !object->madeByFactory() && randInt(1, 50) == 1) {
			if (regularFactory)
				getWorld()->addItemToFront(new ThiefBots(getWorld(), IID_THIEFBOT, x, y, GraphObject::right, 5));
			else
				getWorld()->addItemToFront(new MeanThiefBots(getWorld(), IID_MEAN_THIEFBOT, x, y, GraphObject::right, 8));
			getWorld()->playSound(SOUND_ROBOT_BORN);
		}
	}
}

bool Factories::canShootOver() const
{
	return false;
}

// Peas method implementations
Peas::Peas(StudentWorld* sw, int id, int x, int y, int dir)
	: Actor(sw, id, x, y, dir), first(true)
{
	setShootable(2);
}

void Peas::doSomething()
{
	if (!this->getIsAlive())
		return;
	// ensure that the pea is first animated at its starting location
	if (first) {
		first = false;
		return;
	}

	int x = getX(), y = getY();

	// Get object at the location of the pea
	Actor* object = getWorld()->objectAtLocation(x, y);
	// Handle the case where the object is shootable
	if (object != nullptr && object->getShootable() == 1) {
		object->damage();
		setIsAlive(false);
		return;
	}
	// Handle the case where the object is an obstacle
	else if (object != nullptr && object->getShootable() == 3) {
		setIsAlive(false);
		return;
	}
	// Handle the cases where the pea can go over/through the object or there is no object
	else {
		int dir = getDirection();
		int x, y;
		switch (dir) {
		case GraphObject::left:
			x = getX() - 1;
			y = getY();
			break;
		case GraphObject::right:
			x = getX() + 1;
			y = getY();
			break;
		case GraphObject::down:
			x = getX();
			y = getY() - 1;
			break;
		case GraphObject::up:
			x = getX();
			y = getY() + 1;
			break;
		}
		moveTo(x, y);

		// Get object at the new location of the pea
		object = getWorld()->objectAtLocation(x, y);
		// Handle the case where the object is shootable
		if (object != nullptr && object->getShootable() == 1) {
			object->damage();
			setIsAlive(false);
			return;
		}
		// Handle the case where the object is an obstacle
		else if (object != nullptr && object->getShootable() == 3) {
			setIsAlive(false);
			return;
		}
		return;
	}
}

bool Peas::canStepOn() const {
	return true;
}

// Exits method implementations
Exits::Exits(StudentWorld* sw, int id, int x, int y, int dir)
	: Actor(sw, id, x, y, dir), revealed(false)
{
	setVisible(false);
	setShootable(2);
}

void Exits::doSomething()
{
	// Reveal the exit if all the crystals are collected
	if (!revealed && getWorld()->getCrystalsLeft() <= 0) {
		revealed = true;
		setVisible(true);
		getWorld()->playSound(SOUND_REVEAL_EXIT);
	}
	// Finish the level if the exit is revealed and the player reaches the exit
	Actor* player = getWorld()->getPlayer();
	if (revealed && player->getX() == getX() && player->getY() == getY()) {
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
		getWorld()->increaseScore(2000);
		getWorld()->increaseScore(getWorld()->getBonus());
		getWorld()->setFinishedLevel(true);
	}
}

bool Exits::canStepOn() const
{
	return true;
}

// Walls method implementations
Walls::Walls(StudentWorld* sw, int id, int x, int y, int dir)
	: Actor(sw, id, x, y, dir)
{

}

bool Walls::canShootOver() const
{
	return false;
}

// Marbles method implementations
Marbles::Marbles(StudentWorld* sw, int id, int x, int y, int dir, int health)
	: AttackableActor(sw, id, x, y, dir, health)
{

}

void Marbles::damage() {
	setHealth(getHealth() - 2);
	if (getHealth() <= 0)
		setIsAlive(false);
}

bool Marbles::canFillPit() const
{
	return true;
}

// Pits method implementations
Pits::Pits(StudentWorld* sw, int id, int x, int y, int dir)
	: Actor(sw, id, x, y, dir)
{
	setShootable(2);
}

void Pits::doSomething()
{
	if (!this->getIsAlive())
		return;
	Actor* object = getWorld()->objectAtLocation(getX(), getY());
	// Check if marble is on pit, remove pit and marble if so
	if (object != nullptr && object->canFillPit()) {
		setIsAlive(false);
		object->setIsAlive(false);
	}
}

bool Pits::canHaveMarbleOver() const
{
	return true;
}

// Goodies method implementations
Goodies::Goodies(StudentWorld* sw, int id, int x, int y, int dir)
	: Actor(sw, id, x, y, dir), active(true)
{
	setShootable(2);
}

bool Goodies::canStepOn() const
{
	return true;
}

bool Goodies::playerIsOn()
{
	Avatar* player = getWorld()->getPlayer();
	return player->getX() == getX() && player->getY() == getY();
}

bool Goodies::canBeThieved() const
{
	return true;
}

bool Goodies::isActive() const
{
	return active;
}

void Goodies::setIsActive(bool active)
{
	this->active = active;
}

void Goodies::handleCollect(int addScore)
{
	getWorld()->increaseScore(addScore);
	setIsAlive(false);
	getWorld()->playSound(SOUND_GOT_GOODIE);
}

// Crystals method implementations
Crystals::Crystals(StudentWorld* sw, int id, int x, int y, int dir)
	: Goodies(sw, id, x, y, dir)
{

}

void Crystals::doSomething()
{
	if (!getIsAlive())
		return;
	// Handle when the player collects a crystal
	if (playerIsOn()) {
		handleCollect(50);
		getWorld()->collectCrystal();
	}
}

bool Crystals::canBeThieved() const
{
	return false;
}

bool Crystals::isActive() const
{
	return true;
}

void Crystals::setIsActive(bool active)
{
	return;
}

// ExtraLifeGoodies method implementations
ExtraLifeGoodies::ExtraLifeGoodies(StudentWorld* sw, int id, int x, int y, int dir)
	: Goodies(sw, id, x, y, dir)
{

}

void ExtraLifeGoodies::doSomething()
{
	if (!getIsAlive())
		return;
	// Handle when the player collects an extra life goodie
	if (playerIsOn()) {
		handleCollect(1000);
		getWorld()->incLives();
	}
}


// RestoreHealthGoodies method implementations
RestoreHealthGoodies::RestoreHealthGoodies(StudentWorld* sw, int id, int x, int y, int dir)
	: Goodies(sw, id, x, y, dir)
{

}

void RestoreHealthGoodies::doSomething()
{
	if (!getIsAlive())
		return;
	// Handle when the player collects a restore health goodie
	if (playerIsOn()) {
		handleCollect(500);
		getWorld()->getPlayer()->setHealth(20);
	}
}

// AmmoGoodies method implementations
AmmoGoodies::AmmoGoodies(StudentWorld* sw, int id, int x, int y, int dir)
	: Goodies(sw, id, x, y, dir)
{

}

void AmmoGoodies::doSomething()
{
	if (!getIsAlive())
		return;
	// Handle when player collects an ammo goodie
	if (playerIsOn()) {
		handleCollect(100);
		getWorld()->getPlayer()->addAmmo(20);
	}
}