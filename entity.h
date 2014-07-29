#ifndef __not_jelly_splash__entity__
#define __not_jelly_splash__entity__

#include <SFML/Graphics.hpp>
#include "globalConstants.h"
#include "entityInfo.h"
#include "engine.h"

class cEntity {
public:
    cEntity(cEngine& engine): rEngine { engine } { }
    
    void        setPos(float x, float y) { mPos.x = x; mPos.y = y; }
    void        setGoal(float x, float y) { mGoal.x = x; mGoal.y = y; }
    
protected:
    const cEngine&                      rEngine;
    sf::Vector2f                        mPos { 0, 0 };
    sf::Vector2f                        mGoal { 0, 0 };
    sf::Sprite                          mSprite;
    std::map<std::string, cAnimation>   mAnims;
    int                                 mAnimStepCounter;
    float                               mSpeed;
};

class cJelly : public cEntity {
public:
    cJelly(cEngine& engine, EntColour c);
    
protected:
    EntType             mType;
    EntColour           mColour;
};

class cSuperJelly : public cJelly {
    
};

class cGuard: public cJelly {
    
};

class cDiamond : public cEntity {
    
};

class cBlock : public cEntity {
    
};

#endif /* defined(__not_jelly_splash__entity__) */
