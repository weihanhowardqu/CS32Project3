#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <climits>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}




// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}




StudentWorld::~StudentWorld(){
    cleanUp();
}




int StudentWorld::init()
{
    //create a new ghostracer object and initialize pointer
    track.push_back(new GhostRacer(this, nullptr));
    //crete left/right boundary of yellow borderline and track them
    for(int i = 0; i < VIEW_HEIGHT/SPRITE_HEIGHT; i++){
        track.push_back(new YellowBorder(ROAD_CENTER - ROAD_WIDTH/2.0, i * SPRITE_HEIGHT, 1, this, track[0]));
        track.push_back(new YellowBorder(ROAD_CENTER + ROAD_WIDTH/2.0, i * SPRITE_HEIGHT, 1, this, track[0]));
    }
    //create left/right white dashline and track
    for(int i = 0; i < VIEW_HEIGHT/(4 * SPRITE_HEIGHT); i++){
        track.push_back(new WhiteBorder(ROAD_CENTER - (ROAD_WIDTH/2.0) + ROAD_WIDTH/3, i * (4 * SPRITE_HEIGHT), 1, this, track[0]));
        track.push_back(new WhiteBorder(ROAD_CENTER + (ROAD_WIDTH/2.0) - ROAD_WIDTH/3, i * (4 * SPRITE_HEIGHT), 1, this, track[0]));
    }
    //create 
    return GWSTATUS_CONTINUE_GAME;
}




int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.

    //set string text
    ostringstream oss;
    oss << "Score: " << getScore() << "  " << "Lvl: " << getLevel() << "  " << "Souls2Save: " << (2 * getLevel()) + 5 - soulSaved << "  " << "Lives: " << getLives() << "  " << "Health: " << track[0]->getHealth() << "  " << "Sprays: " << track[0]->getSpray() << "  " << "Bonus: " << bonus;
    
    setGameStatText(oss.str());
    
    //if pass current level
    if (soulSaved == (2 * getLevel()) + 5){
        soulSaved = 0;
        playSound(SOUND_FINISHED_LEVEL);
        //increase score with bonus
        increaseScore(bonus);
        bonus = 5000;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    //decrease one per tick
    if(bonus >= 1){
        bonus -= 1;
    }
    
    //if gRacer not alive
    if(!track[0]->status()){
        decLives();
        //reset
        soulSaved = 0;
        bonus = 5000;
        return GWSTATUS_PLAYER_DIED;
    }
    
    //if gRacer hit a humanPed
    if(hit){
        decLives();
        track[0]->setStatus(false);
        //reset
        hit = false;
        soulSaved = 0;
        bonus = 5000;
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    
    track[0]->doSomething();
    
    if (track[0]->status() && track[0]->gRacer()){
        //if alive, do something
        for(int i = 1; i < track.size(); i++){
            if(track[i]->status()){
                track[i]->doSomething();
            }
        }
    }
    
    vector<Actor*>::iterator it;
    //if not alive, remove the actor
    for(it = track.end() - 1; it > track.begin(); it--){
        if(!(*it)->status()){
            delete *it;
            track.erase(it);
        }
    }
    
    //add new actors
    //generate lostsoul randomly
    int chance = 100;
    if (randInt(0, chance - 1) == 0){
        track.push_back(new LostSoul(randInt(ROAD_CENTER - ROAD_WIDTH/2.0, ROAD_CENTER + ROAD_WIDTH/2.0), VIEW_HEIGHT, this, track[0]));
    }
    
    //generate oil slick randomly
    chance = max(150 - (getLevel() * 10), 40);
    if (randInt(0, chance - 1) == 0){
        track.push_back(new OilSlick(randInt(ROAD_CENTER - ROAD_WIDTH/2.0, ROAD_CENTER + ROAD_WIDTH/2.0), VIEW_HEIGHT, this, track[0]));
    }
    
    //generate holy water refill randomly
    chance = 100 + (100 * getLevel());
    if (randInt(0, chance - 1) == 0){
        track.push_back(new HolyWaterRefill(randInt(ROAD_CENTER - ROAD_WIDTH/2.0, ROAD_CENTER + ROAD_WIDTH/2.0), VIEW_HEIGHT, this, track[0]));
    }
    
    //generate human ped randomly
    chance = max(200 - (getLevel() * 10), 30);
    if (randInt(0, chance - 1) == 0){
        track.push_back(new HumanPed(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this, track[0]));
    }
    
    //generate zombie ped randomly
    chance = max(100 - (getLevel() * 10), 20);
    if (randInt(0, chance - 1) == 0){
        track.push_back(new ZomPed(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this, track[0]));
    }
    
    //generate zombie cab randomly
    if(randInt(0, chance - 1) == 0){
        //random lane to start with
        int curLane = randInt(1, 3);
        int count = 0;
        vector<int> vec = {1, 2, 3};
        while(count < 3){
            int min = INT_MAX;
            int max = INT_MIN;
            //if no such actors or has y big enough
            //left road
            if ((!isValid(curLane, min, max) || min > (VIEW_HEIGHT/3)) && curLane == 1){
                track.push_back(new ZomCab(ROAD_CENTER - (ROAD_WIDTH/3), SPRITE_HEIGHT/2, track[0]->verSpeed() + (randInt(2, 4)), this, track[0]));
                break;
            }
            //if no such actors or has y small enough
            //left road
            else if ((!isValid(curLane, min, max) || max < VIEW_HEIGHT * 2/3) && curLane == 1){
                track.push_back(new ZomCab(ROAD_CENTER - (ROAD_WIDTH/3), VIEW_HEIGHT - (SPRITE_HEIGHT / 2), track[0]->verSpeed() - (randInt(2, 4)), this, track[0]));
                break;
            }
            
            //middle road two cases
            else if ((!isValid(curLane, min, max) || min > VIEW_HEIGHT / 3) && curLane == 2){
                track.push_back(new ZomCab(ROAD_CENTER, SPRITE_HEIGHT / 2, track[0]->verSpeed() + (randInt(2, 4)), this, track[0]));
                break;
            }
            else if ((!isValid(curLane, min, max) || max < VIEW_HEIGHT * 2/3) && curLane == 2){
                track.push_back(new ZomCab(ROAD_CENTER, VIEW_HEIGHT - (SPRITE_HEIGHT / 2), track[0]->verSpeed() - (randInt(2, 4)), this, track[0]));
                break;
            }
            
            //right road two cases
            else if ((!isValid(curLane, min, max) || min > VIEW_HEIGHT / 3) && curLane == 3){
                track.push_back(new ZomCab(ROAD_CENTER + (ROAD_WIDTH/3), SPRITE_HEIGHT / 2, track[0]->verSpeed() + (randInt(2, 4)), this, track[0]));
                break;
            }
            else if ((!isValid(curLane, min, max) || max < VIEW_HEIGHT * 2/3) && curLane == 3){
                track.push_back(new ZomCab(ROAD_CENTER + (ROAD_WIDTH/3), VIEW_HEIGHT - (SPRITE_HEIGHT / 2), track[0]->verSpeed() - (randInt(2, 4)), this, track[0]));
                break;
            }
            
            //update the curlane to other lanes
            vector<int>::iterator it;
            for(it = vec.begin(); it != vec.end(); it++){
                if(*it == curLane){
                    vec.erase(it);
                    break;
                }
            }
            //if there're still lanes
            if(vec.size() > 0){
                curLane = vec[0];
            }
            count++;
        }
    }
    
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    //track to see the last white borderline
    for(unsigned long i = track.size() - 1; int(i) >= 0; i--){
        if(track[i]->whiteBorder()){
            del_y = new_border_y - track[i]->getY();
            break;
        }
    }
    //if enough space for yellow borderline
    if(del_y >= SPRITE_HEIGHT){
        track.push_back(new YellowBorder(ROAD_CENTER - ROAD_WIDTH/2.0, new_border_y, 1, this, track[0]));
        track[track.size()-1]->doSomething();
        track.push_back(new YellowBorder(ROAD_CENTER + ROAD_WIDTH/2.0, new_border_y, 1, this, track[0]));
        track[track.size()-1]->doSomething();
    }
    //if enough space for white borderline
    if(del_y >= 4 * SPRITE_HEIGHT){
        track.push_back(new WhiteBorder(ROAD_CENTER - (ROAD_WIDTH/2.0) + ROAD_WIDTH/3, new_border_y, 1, this, track[0]));
        track[track.size()-1]->doSomething();
        track.push_back(new WhiteBorder(ROAD_CENTER + (ROAD_WIDTH/2.0) - ROAD_WIDTH/3, new_border_y, 1, this, track[0]));
        track[track.size()-1]->doSomething();
    }
    return GWSTATUS_CONTINUE_GAME;
}




void StudentWorld::cleanUp()
{
    //if nothing happens yet
    if (track.size() == 0){
        return;
    }
    //free any remaining dynamic memory in the vector
    vector<Actor*>::iterator it;
    for(it = track.end() - 1; it >= track.begin(); it--){
        delete *it;
        track.erase(it);
    }
}


void StudentWorld::incLostSoul(){
    soulSaved++;
}

void StudentWorld::pushNewActor(Actor* actor){
    track.push_back(actor);
    track[track.size() - 1]->doSomething();
}

bool StudentWorld::iterate(Actor* ptr){
    for (int i = 1; i < track.size(); i++){
        //actors that could be dead
        if (holyAffectedActors(track[i]) && track[i]->overlap(track[i], ptr)){
            //damage 1 hit point
            track[i]->damageHealth(1);
            
            //if zombie died
            if ((track[i]->getHealth() == 0) && (track[i]->zomPed())){
                track[i]->setStatus(false);
                playSound(SOUND_PED_DIE);
                
                //if not overlap with gr, generate healing goodie
                if (!track[i]->overlap(track[i], track[0])){
                    if(randInt(0, 4) == 0){
                        track.push_back(new HealingGoodie(track[i]->getX(), track[i]->getY(), this, track[0]));
                    }
                }
                //increase points
                increaseScore(150);
            }
            
            //if zombie not die
            else if ((track[i]->getHealth() > 0) && (track[i]->zomPed())){
                playSound(SOUND_PED_HURT);
            }
            
            //if zombie cab die
            else if ((track[i]->getHealth() == 0) && (track[i]->zomCab())){
                track[i]->setStatus(false);
                playSound(SOUND_VEHICLE_DIE);
                if(randInt(0, 4) == 0){
                    track.push_back(new OilSlick(track[i]->getX(), track[i]->getY(), this, track[0]));
                }
                //increase points
                increaseScore(200);
                return true;
            }
            
            //if zombie cab not die
            else if ((track[i]->getHealth() > 0) && (track[i]->zomCab())){
                playSound(SOUND_VEHICLE_HURT);
            }
            
            //cases of goodies
            else if (track[i]->getHealth() == 0){
                track[i]->setStatus(false);
            }
            return true;
        }
        
        //actors that can't be damaged
        else if(track[i]->humanPed() && track[i]->overlap(track[i], ptr)){
            track[i]->hitByHoly();
            return true;
        }
    }
    return false;
}

void StudentWorld::lookCActors(int position, int road, bool& front, bool& behind, bool& closeF, bool&closeB){
    //except for ghostracer
    for (int i = 1; i < track.size(); i++){
        //if it's collision actor on the same road
        if (track[i]->isCActor() && track[i]->whichRoad(track[i]->getX()) == road){
            //determine its position and distance from passed in position
            if (track[i]->getY() > position && (track[i]->getY() - position) < 96){
                front = true;
                closeF = true;
            }
            else if (track[i]->getY() < position && (position - track[i]->getY()) < 96){
                behind = true;
                closeB  = true;
            }
        }
    }
}

void StudentWorld::hitPed(){
    hit = true;
}

bool StudentWorld::isValid(int lane, int& min, int& max){
    if(lane == 1){
        for (int i = 0; i < track.size(); i++){
            //left road
            if(track[i]->whichRoad(track[i]->getX()) == 1 && track[i]->isCActor()){
                if (track[i]->getY() < min){
                    min = track[i]->getY();
                }
                if (track[i]->getY() > max){
                    max = track[i]->getY();
                }
                return true;
            }
        }
    }
    
    //middle road
    else if (lane == 2){
        for (int i = 0; i < track.size(); i++){
            if (track[i]->isCActor() && track[i]->whichRoad(track[i]->getX()) == 2){
                if (track[i]->getY() < min){
                    min = track[i]->getY();
                }
                if (track[i]->getY() > max){
                    max = track[i]->getY();
                }
                return true;
            }
        }
    }
    
    //right road
    else{
        for (int i = 0; i < track.size(); i++){
            if (track[i]->isCActor() && track[i]->whichRoad(track[i]->getX()) == 3){
                if (track[i]->getY() < min){
                    min = track[i]->getY();
                }
                if (track[i]->getY() > max){
                    max = track[i]->getY();
                }
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::holyAffectedActors(Actor* actor){
    if (actor->holyWaterRefill() || actor->zomPed() || actor->healingWater() || actor->zomCab()){
        return true;
    }
    return false;
}
