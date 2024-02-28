#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"

#include<iostream>
#include<sstream>
#include<iomanip>
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}


StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath), player(nullptr), m_bonus(1000), m_crystalsLeft(0), levelIsFinished(false)
{

}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    // Load the current maze details 
    Level level(assetPath());
    ostringstream lev;
    lev.fill('0');
    lev << "level" << setw(2) << getLevel() << ".txt";
    string l = lev.str();
    Level::LoadResult result = level.loadLevel(l);

    // If there is no level data file with the next number
    if (getLevel() == 100 || result == Level::load_fail_bad_format)
        return GWSTATUS_PLAYER_WON;

    // If the next file is not a maze file
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_LEVEL_ERROR;

    // Allocate and insert Actors
    for (int y = 0; y < VIEW_HEIGHT; y++) {
        for (int x = 0; x < VIEW_WIDTH; x++) {
            Level::MazeEntry item = level.getContentsOf(x, y);
            switch (item) {
            case Level::exit:
                actors.push_back(new Exits(this, IID_EXIT, x, y, GraphObject::none));
                break;
            case Level::player:
                player = new Avatar(this, IID_PLAYER, x, y, GraphObject::right, 20, 20);
                break;
            case Level::horiz_ragebot:
                actors.push_front(new RageBots(this, IID_RAGEBOT, x, y, GraphObject::right, 10));
                break;
            case Level::vert_ragebot:
                actors.push_front(new RageBots(this, IID_RAGEBOT, x, y, GraphObject::down, 10));
                break;
            case Level::thiefbot_factory:
                actors.push_back(new Factories(this, IID_ROBOT_FACTORY, x, y, GraphObject::none, true));
                break;
            case Level::mean_thiefbot_factory:
                actors.push_back(new Factories(this, IID_ROBOT_FACTORY, x, y, GraphObject::none, false));
                break;
            case Level::wall:
                actors.push_back(new Walls(this, IID_WALL, x, y, GraphObject::none));
                break;
            case Level::marble:
                actors.push_front(new Marbles(this, IID_MARBLE, x, y, GraphObject::none, 10));
                break;
            case Level::pit:
                actors.push_back(new Pits(this, IID_PIT, x, y, GraphObject::none));
                break;
            case Level::crystal:
                m_crystalsLeft++;
                actors.push_back(new Crystals(this, IID_CRYSTAL, x, y, GraphObject::none));
                break;
            case Level::restore_health:
                actors.push_back(new RestoreHealthGoodies(this, IID_RESTORE_HEALTH, x, y, GraphObject::none));
                break;
            case Level::extra_life:
                actors.push_back(new ExtraLifeGoodies(this, IID_EXTRA_LIFE, x, y, GraphObject::none));
                break;
            case Level::ammo:
                actors.push_back(new AmmoGoodies(this, IID_AMMO, x, y, GraphObject::none));
                break;
            default:
                break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Update the Game Status Line
    // update the score/lives/level text at screen top
    setDisplayText();

    // Give player a chance to do something
    player->doSomething();
    if (!player->getIsAlive()) {
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    // Give each actor a chance to do something
    for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
        // If actor is active
        if ((*i)->isActive()) {
            // Make a move
            (*i)->doSomething();

            if (!player->getIsAlive()) {
                decLives();
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
            if (levelIsFinished)
                return GWSTATUS_FINISHED_LEVEL;
        }
    }

    // Remove newly-dead actors after each tick
    for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); ) {
        if (!(*i)->getIsAlive()) {
            delete* i;
            i = actors.erase(i);
        }
        else
            i++;
    }

    // Reduce current bonus for the level by one
    if (m_bonus > 0)
        m_bonus--;

    // return the proper result
    if (!player->getIsAlive()) {
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    if (levelIsFinished)
        return GWSTATUS_FINISHED_LEVEL;
    // if player did not finish level or die, continue game
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (player != nullptr) {
        delete player;
        player = nullptr;
    }
    for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); ) {
        delete* i;
        i = actors.erase(i);
    }
    setFinishedLevel(false);
    m_bonus = 1000;
    m_crystalsLeft = 0;
}

// returns pointer to the player
Avatar* StudentWorld::getPlayer()
{
    return player;
}

// returns pointer to the Actor at location x, y
// nullptr if no Actor at x, y
// logic in how the Actors are added to the list (push_front vs push_back) ensure
// that when multiple objects are on the same x, y the right object is returned
Actor* StudentWorld::objectAtLocation(int x, int y)
{
    if (player->getX() == x && player->getY() == y)
        return player;
    for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
        if ((*i)->getX() == x && (*i)->getY() == y)
            return (*i);
    }
    return nullptr;
}

// returns pointer to Goodie at location x, y
// nullptr if no Goodie at x, y
Actor* StudentWorld::lootAtLocation(int x, int y)
{
    for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
        if ((*i)->canBeThieved() && (*i)->getX() == x && (*i)->getY() == y)
            return (*i);
    }
    return nullptr;
}

// handles setting the Display Text seen in the game
void StudentWorld::setDisplayText()
{
    int score = getScore();
    int level = getLevel();
    unsigned int bonus = m_bonus;
    int livesLeft = getLives();

    // get health into percent format
    int health = 100 * player->getHealth() / 20;

    // Format display
    ostringstream display;
    display.fill('0');
    display << "Score: " << setw(7) << score << "  ";
    display << "Level: " << setw(2) << level << "  ";
    display.fill(' ');
    display << "Lives: " << setw(2) << livesLeft << "  ";
    display << "Health: " << setw(3) << health << "%  ";
    display << "Ammo: " << setw(3) << player->getAmmo() << "  ";
    display << "Bonus: " << setw(4) << m_bonus;

    string d = display.str();

    // Set Display
    setGameStatText(d);
}

void StudentWorld::addItem(Actor* item)
{
    actors.push_back(item);
}

void StudentWorld::addItemToFront(Actor* item)
{
    actors.push_front(item);
}

int StudentWorld::getBonus() const
{
    return m_bonus;
}

int StudentWorld::getCrystalsLeft() const
{
    return m_crystalsLeft;
}

void StudentWorld::collectCrystal()
{
    m_crystalsLeft--;
}

void StudentWorld::setFinishedLevel(bool finish)
{
    levelIsFinished = finish;
}