#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <list>

class Actor;
class Avatar;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	Avatar* getPlayer();
	Actor* objectAtLocation(int x, int y);
	Actor* lootAtLocation(int x, int y);
	void addItem(Actor* item);
	void addItemToFront(Actor* item);
	int getBonus() const;
	int getCrystalsLeft() const;
	void collectCrystal();
	void setFinishedLevel(bool finish);

private:
	Avatar* player;
	std::list<Actor*> actors;
	int m_bonus;
	int m_crystalsLeft;
	bool levelIsFinished;

	// Additional helper functions
	void setDisplayText();
};

#endif // STUDENTWORLD_H_