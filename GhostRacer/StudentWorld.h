#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <vector>
#include <string>
class Actor;
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    //increase lostsoul
    void incLostSoul();
    //push new Actor into track
    void pushNewActor(Actor* actor);
    //iterate through to see overlap
    bool iterate(Actor* ptr);
    //iterate to see collision actors according to the passed in position
    void lookCActors(int position, int road, bool& front, bool& behind, bool& closeF, bool& closeB);
    //hit human ped
    void hitPed();
    //determine whether generate zombie cab
    bool isValid(int lane, int& min, int& max);
private:
    //track container
    vector<Actor*> track;
    int del_y;
    int chanceOfSoul = 100;
    int soulSaved = 0;
    //see gr hit a human ped
    bool hit = false;
    int bonus = 5000;
    //helper function
    bool holyAffectedActors(Actor* actor);
};

#endif // STUDENTWORLD_H_
