#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
using namespace std;

//base class
class Actor: public GraphObject{
public:
    Actor(int imageID, double startX, double startY, int startDirection, double size, int depth, int health, bool alive, int v_speed, int h_speed, bool c_actor, StudentWorld* ptr, Actor* gr_ptr);
    virtual ~Actor();
    virtual void doSomething() = 0;
    virtual void hitByHoly();
    virtual void spunByOil();
    //check if it's ghostracer
    virtual bool gRacer();
    //check if it's white borderline
    virtual bool whiteBorder();
    //see if it's holywater refill
    virtual bool holyWaterRefill();
    //see if it's healing water
    virtual bool healingWater();
    //see if it's human ped
    virtual bool humanPed();
    //see if it's zombie ped
    virtual bool zomPed();
    //see if it's zombie cab
    virtual bool zomCab();
    //return vertical speed
    int verSpeed() const;
    //return horizontal speed
    int horSpeed() const;
    //return alive or not
    bool status() const;
    //return health
    int getHealth() const;
    //increase health
    void healHealth(int n);
    //damage health
    void damageHealth(int n);
    //set health
    void setHealth(int n);
    //determine overlap or not
    bool overlap(const Actor* A, const Actor* B);
    //increase spray
    void increaseSpray(int n);
    //decrease spray
    void decreaseSpray(int n);
    //set status
    void setStatus(bool status);
    //ghost racer died or not
    bool diedOrNot();
    //return whether it's a collision worthy actor
    bool isCActor();
    //get spray
    int getSpray() const;
    //determine road
    int whichRoad(int position);
protected:
    //return a SW pointer
    StudentWorld* getWorld();
    //set ver speed
    void setVerSpeed(int n);
    //set hor speed
    void setHorSpeed(int n);
    //out of screen
    bool outOfScreen();
    //get ghostracer pointer
    Actor* getGrPtr() const;
    //move based on ghostracer speed
    void moveBasedOnGR();
private:
    //common attributes
    bool m_status;
    int v_speed;
    int h_speed;
    bool c_actor;
    int m_health;
    //specific attributes of GR
    int m_spray;
    //StudentWorld Pointer
    StudentWorld* SW_ptr;
    //ghostracer pointer
    Actor* gr_ptr;
};




//spray projectile class
class SprayProjectile: public Actor{
public:
    SprayProjectile(double startX, double startY, int startDirection, StudentWorld* ptr, Actor* gr_ptr);
    virtual void doSomething();
private:
    int maxTravelDistance;
    int sprayTraveled;
};




//ghostracer class
class GhostRacer: public Actor{
public:
    GhostRacer(StudentWorld* ptr, Actor* gr_ptr);
    virtual void doSomething();
    virtual bool gRacer();
    virtual void spunByOil();
private:
    //helper function for doSomething
    void moveAlg();
};




//passiveactor class (borderline, lostsoul, etc.)
class PassiveActor: public Actor{
public:
    PassiveActor(int imageID, double startX, double startY, int startDirection, double size, int depth, int health, StudentWorld* ptr, Actor* gr_ptr);
    virtual void doSomething() = 0;
private:
};




//borderline class
class BorderLine: public PassiveActor{
public:
    BorderLine(int imageID, double startX, double startY, int health, StudentWorld* ptr, Actor* gr);
    virtual void doSomething();
private:
};
 



//white borderline class
class WhiteBorder: public BorderLine{
public:
    WhiteBorder(double startX, double startY, int health, StudentWorld* ptr, Actor* gr);
    virtual bool whiteBorder();
    virtual void doSomething();
};




//white borderline class
class YellowBorder: public BorderLine{
public:
    YellowBorder(double startX, double startY, int health, StudentWorld* ptr, Actor* gr);
    virtual void doSomething();
};




//lost soul class
class LostSoul: public PassiveActor{
public:
    LostSoul(double startX, double startY, StudentWorld* ptr, Actor* gr);
    virtual void doSomething();
private:
};



//oil slick class
class OilSlick: public PassiveActor{
public:
    OilSlick(double startX, double startY, StudentWorld* ptr, Actor* gr);
    virtual void doSomething();
private:
};




//healing goodie class
class HealingGoodie: public PassiveActor{
public:
    HealingGoodie(double startX, double startY, StudentWorld* ptr, Actor* gr);
    virtual void doSomething();
    virtual bool healingWater();
private:
};




//holy water refill class
class HolyWaterRefill: public PassiveActor{
public:
    HolyWaterRefill(double startX, double startY, StudentWorld* ptr, Actor* gr);
    virtual void doSomething();
    virtual bool holyWaterRefill();
private:
};




//active actor class
class ActiveActor: public Actor{
public:
    ActiveActor(int imageID, double startX, double startY, int startDirection, double size, int health, int v_speed, StudentWorld* ptr, Actor* gr_ptr);
    virtual void doSomething() = 0;
protected:
    //return plan
    int getPlan() const;
    //decrease plan
    void decPlan(int n);
    //set plan to a number
    void setPlan(int n);
    //still in plan
    bool inPlan();
    //update plan
    void planUpdate();
private:
    int plan;
};




//human pedestrian class
class HumanPed: public ActiveActor{
public:
    HumanPed(double startX, double startY, StudentWorld* ptr, Actor* gr_ptr);
    virtual void doSomething();
    //hit by holy water projectile
    virtual void hitByHoly();
    virtual bool humanPed();
private:
};




//zombie pedestrian class
class ZomPed: public ActiveActor{
public:
    ZomPed(double startX, double startY, StudentWorld* ptr, Actor* gr_ptr);
    virtual void doSomething();
    virtual bool zomPed();
private:
    int ticks;
};




//zombie cab class
class ZomCab: public ActiveActor{
public:
    ZomCab(double startX, double startY, int v_speed, StudentWorld* ptr, Actor* gr_ptr);
    virtual void doSomething();
    virtual bool zomCab();
    void closeToCActors();
private:
    bool damageGr;
    bool front;
    bool behind;
    bool closeF;
    bool closeB;
};

#endif // ACTOR_H_

