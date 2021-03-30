#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>
#include <stdlib.h>

//Actor class implementations
Actor::Actor(int imageID, double startX, double startY, int startDirection, double size, int depth, int health, bool alive, int v_speed, int h_speed, bool c_actor, StudentWorld* ptr, Actor* gr_ptr)
:GraphObject(imageID, startX, startY, startDirection, size, depth), m_status(alive), v_speed(v_speed), h_speed(h_speed), c_actor(c_actor), m_health(health), m_spray(10), SW_ptr(ptr), gr_ptr(gr_ptr)
{
}

Actor::~Actor(){
}

void Actor::hitByHoly(){
}

void Actor::spunByOil(){
}

bool Actor::gRacer(){
    return false;
}

bool Actor::whiteBorder(){
    return false;
}

bool Actor::holyWaterRefill(){
    return false;
}

bool Actor::healingWater(){
    return false;
}

bool Actor::humanPed(){
    return false;
}

bool Actor::zomPed(){
    return false;
}

bool Actor::zomCab(){
    return false;
}

StudentWorld* Actor::getWorld(){
    return SW_ptr;
}

int Actor::verSpeed() const{
    return v_speed;
}

void Actor::setVerSpeed(int n){
    v_speed = n;
}

void Actor::setHorSpeed(int n){
    h_speed = n;
}

int Actor::horSpeed() const{
    return h_speed;
}

bool Actor::status() const{
    return m_status;
}

void Actor::setStatus(bool status){
    m_status = status;
}

int Actor::getHealth() const{
    return m_health;
}

bool Actor::overlap(const Actor* A, const Actor* B){
    double del_x = abs(A->getX() - B->getX());
    double del_y = abs(A->getY() - B->getY());
    double radiusSum = A->getRadius() + B->getRadius();
    if (del_x < radiusSum * 0.25 && del_y < radiusSum * 0.6){
        return true;
    }
    return false;
}

void Actor::damageHealth(int n){
    if(m_health - n < 0){
        m_health = 0;
    }
    else{
        m_health -= n;
    }
}

void Actor::setHealth(int n){
    m_health = n;
}

void Actor::healHealth(int n){
    m_health += n;
}

void Actor::increaseSpray(int n){
    m_spray += n;
}

void Actor::decreaseSpray(int n){
    m_spray -= n;
}

bool Actor::diedOrNot(){
    if(m_health <= 0){
        setStatus(false);
        getWorld()->playSound(SOUND_PLAYER_DIE);
        return true;
    }
    return false;
}

bool Actor::isCActor(){
    return c_actor;
}

bool Actor::outOfScreen(){
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        setStatus(false);
        return true;
    }
    return false;
}

int Actor::getSpray() const{
    return m_spray;
}

Actor* Actor::getGrPtr() const{
    return gr_ptr;
}

int Actor::whichRoad(int position){
    //left road
    if ((position >= ROAD_CENTER - (ROAD_WIDTH/2.0)) && (position < ROAD_CENTER - (ROAD_WIDTH/2.0) + (ROAD_WIDTH/3))){
        return 1;
    }
    //mid
    else if ((position >= ROAD_CENTER - (ROAD_WIDTH/2.0) + (ROAD_WIDTH/3)) && (position < ROAD_CENTER + (ROAD_WIDTH/2.0) - (ROAD_WIDTH/3))){
        return 2;
    }
    //right
    return 3;
}

void Actor::moveBasedOnGR(){
    //move borderline based on the ghostracer's speed
    moveTo(getX() + horSpeed(), getY() + (verSpeed() - (getGrPtr()->verSpeed())));
}




//Spray Projectile class implementation
SprayProjectile::SprayProjectile(double startX, double startY, int startDirection, StudentWorld* ptr, Actor* gr_ptr)
:Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, startDirection, 1.0, 1, 1, true, 0, 0, false, ptr, gr_ptr), maxTravelDistance(160), sprayTraveled(0)
{
}

void SprayProjectile::doSomething(){
    //if not alive
    if (getHealth() == 0){
        return;
    }
    //if hit actors that could be damaged
    if(getWorld()->iterate(this)){
        setStatus(false);
        return;
    }
    //move forward
    moveForward(SPRITE_HEIGHT);
    sprayTraveled += SPRITE_HEIGHT;
    if(outOfScreen()){
        return;
    }
    //if out of max projectile
    if(sprayTraveled == 160){
        setStatus(false);
    }
}




//GhostRacer class implementations
GhostRacer::GhostRacer(StudentWorld* ptr, Actor* gr_ptr)
:Actor(IID_GHOST_RACER, 128, 32, 90, 4.0, 0, 100, true, 0, 0, true, ptr, gr_ptr)
{
}

void GhostRacer::doSomething(){
    //if not alive
    if(getHealth() == 0){
        return;
    }
    //crash the left boundary
    if (getX() <= ROAD_CENTER - ROAD_WIDTH/2.0){
        if (getDirection() > 90){
            damageHealth(10);
            if(diedOrNot()){
                return;
            }
        }
        setDirection(82);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    
    //crash the right boundary
    if (getX() >= ROAD_CENTER + ROAD_WIDTH/2.0){
        if (getDirection() < 90){
            damageHealth(10);
            if(diedOrNot()){
                return;
            }
        }
        setDirection(98);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    
    //get movement input
    int key;
    if (getWorld()->getKey(key)){
        switch(key){
            //space bar pressed
            case KEY_PRESS_SPACE:
                if(getSpray() > 0){
                    //create a new spray projectile, push it to the container in studentworld
                    //SPRITE_HEIGHT in front of current ghostracer direction
                    getWorld()->pushNewActor(new SprayProjectile(getX() + (SPRITE_HEIGHT * cos((getDirection()/180.0) * M_PI)), getY()+(SPRITE_HEIGHT * sin((getDirection()/180.0) * M_PI)), getDirection(), getWorld(), this));
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    decreaseSpray(1);
                }
                break;
            //left key pressed
            case KEY_PRESS_LEFT:
                if (getDirection() < 114){
                    setDirection(getDirection() + 8);
                }
                break;
            //right key pressed
            case KEY_PRESS_RIGHT:
                if (getDirection() > 66){
                    setDirection(getDirection() - 8);
                }
                break;
            //speed up
            case KEY_PRESS_UP:
                if (verSpeed() < 5){
                    //increase and move based on alg
                    setVerSpeed(verSpeed() + 1);
                }
                break;
            //slow down
            case KEY_PRESS_DOWN:
                if (verSpeed() > -1){
                    setVerSpeed(verSpeed() - 1);
                }
                break;
        }
    }
    //move when no key input
    moveAlg();
}

bool GhostRacer::gRacer(){
    return true;
}

void GhostRacer::spunByOil(){
    //spin clockwise or counter
    int clockOrCounter = randInt(0, 1);
    //spin [5, 20] degrees
    int randDegree = randInt(5, 20);
    //if spin clockwise and not go lower 60
    if (clockOrCounter == 1 && (getDirection() - randDegree >= 60)){
        setDirection(getDirection() - randDegree);
    }
    //if spin clockwsie and go lower 60
    else if (clockOrCounter == 1 && (getDirection() - randDegree < 60)){
        setDirection(60);
    }
    //if spin counter and not go over 120
    else if (clockOrCounter == 0 && (getDirection() + randDegree <= 120)){
        setDirection(getDirection() + randDegree);
    }
    //if spin counter and go over 120
    else if (clockOrCounter == 0 && (getDirection() + randDegree > 120)){
        setDirection(120);
    }
}

void GhostRacer::moveAlg(){
    //moving algorithm based on instructions
    double max_shift_per_tick = 4.0;
    int direction = getDirection();
    double x_shift = cos((direction/180.0) * M_PI) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + x_shift, cur_y);
}




//PassiveActor class implementation
PassiveActor::PassiveActor(int imageID, double startX, double startY, int startDirection, double size, int depth, int health, StudentWorld* ptr, Actor* gr_ptr)
:Actor(imageID, startX, startY, startDirection, size, depth, health, true, -4, 0, false, ptr, gr_ptr)
{
    
}




//BorderLine Class Implementation
BorderLine::BorderLine(int imageID, double startX, double startY, int health, StudentWorld* ptr, Actor* gr)
:PassiveActor(imageID, startX, startY, 0, 2.0, 2, health, ptr, gr)
{
}

void BorderLine::doSomething(){
    moveBasedOnGR();
    if(outOfScreen()){
        return;
    }
}




//white border class implementation
WhiteBorder::WhiteBorder(double startX, double startY, int health, StudentWorld* ptr, Actor* gr)
:BorderLine(IID_WHITE_BORDER_LINE, startX, startY, health, ptr, gr)
{
}

bool WhiteBorder::whiteBorder(){
    return true;
}

void WhiteBorder::doSomething(){
    BorderLine::doSomething();
}




//yellow border class implementation
YellowBorder::YellowBorder(double startX, double startY, int health, StudentWorld* ptr, Actor* gr)
:BorderLine(IID_YELLOW_BORDER_LINE, startX, startY, health, ptr, gr)
{
}

void YellowBorder::doSomething(){
    BorderLine::doSomething();
}




//lostsoul class implementation
LostSoul::LostSoul(double startX, double startY, StudentWorld* ptr, Actor* gr)
:PassiveActor(IID_SOUL_GOODIE, startX, startY, 0, 4.0, 2, 1, ptr, gr)
{
}

void LostSoul::doSomething(){
    moveBasedOnGR();
    if(outOfScreen()){
        return;
    }
    //collect soul
    if(overlap(getGrPtr(), this)){
        getWorld()->incLostSoul();
        setStatus(false);
        getWorld()->playSound(SOUND_GOT_SOUL);
        getWorld()->increaseScore(100);
    }
    setDirection(getDirection() - 10);
}




//Oil Slick class implementation
OilSlick::OilSlick(double startX, double startY, StudentWorld* ptr, Actor* gr)
:PassiveActor(IID_OIL_SLICK, startX, startY, 0, randInt(2, 5), 2, 1, ptr, gr)
{
}

void OilSlick::doSomething(){
    moveBasedOnGR();
    if(outOfScreen()){
        return;
    }
    //if overlap, spin
    if(overlap(getGrPtr(), this)){
        getWorld()->playSound(SOUND_OIL_SLICK);
        //if still alive
        if(getGrPtr()->status()){
            getGrPtr()->spunByOil();
        }
    }
}




//HealingGoodie class implementation
HealingGoodie::HealingGoodie(double startX, double startY, StudentWorld* ptr, Actor* gr)
:PassiveActor(IID_HEAL_GOODIE, startX, startY, 0, 1.0, 2, 1, ptr, gr)
{
}

void HealingGoodie::doSomething(){
    moveBasedOnGR();
    if(outOfScreen()){
        return;
    }
    //if ovelap, do as instructed
    if(overlap(getGrPtr(), this)){
        //can't go over 100 health
        if(getGrPtr()->getHealth() + 10 > 100){
            getGrPtr()->setHealth(100);
        }
        else{
            getGrPtr()->healHealth(10);
        }
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(250);
        setStatus(false);
    }
}

bool HealingGoodie::healingWater(){
    return true;
}




//HolyWaterRefill class implementation
HolyWaterRefill::HolyWaterRefill(double startX, double startY, StudentWorld* ptr, Actor* gr)
:PassiveActor(IID_HOLY_WATER_GOODIE, startX, startY, 90, 2.0, 2, 1, ptr, gr)
{
}

void HolyWaterRefill::doSomething(){
    moveBasedOnGR();
    if(outOfScreen()){
        return;
    }
    //if ovelap, do as instructed
    if(overlap(getGrPtr(), this)){
        getGrPtr()->increaseSpray(10);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
        setStatus(false);
    }
}

bool HolyWaterRefill::holyWaterRefill(){
    return true;
}




//Active Actor class implementation
ActiveActor::ActiveActor(int imageID, double startX, double startY, int startDirection, double size, int health, int v_speed, StudentWorld* ptr, Actor* gr_ptr)
:Actor(imageID, startX, startY, startDirection, size, 0, health, true, v_speed, 0, true, ptr, gr_ptr), plan(0)
{
}

int ActiveActor::getPlan() const{
    return plan;
}

void ActiveActor::decPlan(int n){
    plan -= n;
}

void ActiveActor::setPlan(int n){
    plan = n;
}

bool ActiveActor::inPlan(){
    if (getPlan() > 1){
        decPlan(1);
        return true;
    }
    decPlan(1);
    return false;
}

void ActiveActor::planUpdate(){
    int n = randInt(-3, 3);
    //can't be zero
    while(n == 0){
        n = randInt(-3, 3);
    }
    //new hor speed and plan distance
    setHorSpeed(n);
    setPlan(randInt(4, 32));
    //update direction
    if (horSpeed() > 0){
        setDirection(0);
    }
    else if(horSpeed() < 0){
        setDirection(180);
    }
}




//Human Ped class implementation
HumanPed::HumanPed(double startX, double startY, StudentWorld* ptr, Actor* gr_ptr)
:ActiveActor(IID_HUMAN_PED, startX, startY, 0, 2.0, 2, -4, ptr, gr_ptr)
{
}

void HumanPed::doSomething(){
    //if not alive
    if (getHealth() == 0){
        return;
    }
    //if hit a human ped
    if (overlap(getGrPtr(), this)){
        getWorld()->hitPed();
        return;
    }
    //move based on ghost racer
    moveBasedOnGR();
    if(outOfScreen()){
        return;
    }
    //if still in plan
    if(inPlan()){
        return;
    }
    //else update the moving plan
    planUpdate();
}

void HumanPed::hitByHoly(){
    if (getDirection() == 0){
        setDirection(180);
    }
    else{
        setDirection(0);
    }
    setHorSpeed(horSpeed() * (-1));
    getWorld()->playSound(SOUND_PED_HURT);
}

bool HumanPed::humanPed(){
    return true;
}




//Zombie Ped class implementation
ZomPed::ZomPed(double startX, double startY, StudentWorld* ptr, Actor* gr_ptr)
:ActiveActor(IID_ZOMBIE_PED, startX, startY, 0, 3.0, 2, -4, ptr, gr_ptr), ticks(0)
{
}

void ZomPed::doSomething(){
    //if not alive
    if (getHealth() == 0){
        return;
    }
    
    //if overlap with ghostracer
    if (overlap(getGrPtr(), this)){
        getGrPtr()->damageHealth(5);
        //if gr dies
        getGrPtr()->diedOrNot();
        getWorld()->playSound(SOUND_PED_DIE);
        damageHealth(2);
        setStatus(false);
        return;
    }
    
    //if close to ghostracer
    if ((getX() <= (getGrPtr()->getX() + 30) && getX() >= (getGrPtr()->getX() - 30)) && getY() > getGrPtr()->getY()){
        setDirection(270);
        //left of ghostracer
        if(getX() < getGrPtr()->getX()){
            setHorSpeed(1);
        }
        //right of ghostracer
        else if(getX() > getGrPtr()->getX()){
            setHorSpeed(-1);
        }
        //same X
        else{
            setHorSpeed(0);
        }
        //decrease ticks
        ticks -= 1;
        if (ticks <= 0){
            getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            ticks = 20;
        }
    }
    
    //move accordingly
    moveBasedOnGR();
    if(outOfScreen()){
        return;
    }
    if (inPlan()){
        return;
    }
    planUpdate();
}

bool ZomPed::zomPed(){
    return true;
}




//Zombie Cab class implementation
ZomCab::ZomCab(double startX, double startY, int v_speed, StudentWorld* ptr, Actor* gr_ptr)
:ActiveActor(IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 3, v_speed, ptr, gr_ptr), damageGr(false), front(false), behind(false), closeF(false), closeB(false)
{
}

void ZomCab::doSomething(){
    //if not alive
    if (getHealth() == 0){
        return;
    }
    
    //if overlap with ghost racer
    if(overlap(getGrPtr(), this)){
        //if not already damaged
        if(!damageGr){
            //damage if cab alive
            if (status()){
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            getGrPtr()->damageHealth(20);
            }
            //if ghost racer dead
            if(getGrPtr()->diedOrNot()){
                return;
            }
            if(getX() <= getGrPtr()->getX()){
                setHorSpeed(-5);
                setDirection(120 + (randInt(0, 19)));
            }
            else{
                setHorSpeed(5);
                setDirection(60 - (randInt(0, 19)));
            }
            damageGr = true;
        }
    }
    moveBasedOnGR();
    if(outOfScreen()){
        return;
    }
    //compare speed with gr and have collision actor in front/back
    //left most road
    if (whichRoad(getX()) == 1){
        getWorld()->lookCActors(getY(), 1, front, behind, closeF, closeB);
        closeToCActors();
            
    }
    //middle road
    else if (whichRoad(getX()) == 2){
        getWorld()->lookCActors(getY(), 2, front, behind, closeF, closeB);
        closeToCActors();
    }
    //right road
    else if (whichRoad(getX()) == 3){
        getWorld()->lookCActors(getY(), 3, front, behind, closeF, closeB);
        closeToCActors();
    }
    if (inPlan()){
        return;
    }
    setPlan(randInt(4, 32));
    setVerSpeed(verSpeed() + randInt(-2, 2));
}

bool ZomCab::zomCab(){
    return true;
}

void ZomCab::closeToCActors(){
    //there's collision actor in front
    if ((verSpeed() > getGrPtr()->verSpeed()) && (front) && closeF){
        //if closer than 96 pixels
        setVerSpeed(verSpeed() - 0.5);
    }
    //there's collision actor behind
    if((verSpeed() <= getGrPtr()->verSpeed()) && (behind) && closeB){
        //if closer than 96 pixels
        setVerSpeed(verSpeed() + 0.5);
    }
    //reset
    front = false;
    behind = false;
    closeF = false;
    closeB = false;
}
