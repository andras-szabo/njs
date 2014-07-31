#ifndef __not_jelly_splash__entity__
#define __not_jelly_splash__entity__

#include <SFML/Graphics.hpp>
#include "globalConstants.h"
#include "entityInfo.h"
#include "engine.h"

class cEntity {
public:
    cEntity(cEngine& engine, const std::string& id): rEngine { engine } { loadInfo(id); }
    
    void            setPos(float x, float y) { mPos.x = x; mPos.y = y; }
    void            setGoal(float x, float y);
    
    virtual void    switchColour(EntColour) = 0;
    virtual void    render(sf::RenderWindow&) = 0;
    virtual void    update(float dt);
    virtual void    explode();
    virtual void    switchToAnim(const std::string&);

protected:
    void            loadInfo(const std::string& id);
    
public:
    EntType                             mType;
    EntColour                           mColour;
    EntState                            mState { EntState::normal };
    bool                                mNeedToMove { false };
    Direction                           mDir { Direction::undecided };  // super jelly orientation
    bool                                mSuper { false };   // super jelly?
    int                                 mLives { 1 };
    
protected:
    const cEngine&                      rEngine;
    sf::Vector2f                        mPos { 0, 0 };
    sf::Vector2f                        mGoal { 0, 0 };
    sf::Vector2f                        mVel { 0, 0 };      // velocity
    sf::Sprite                          mSprite;
    std::map<std::string, cAnimation>   mAnims;
    int                                 mAnimStepCounter;
    int                                 mCurrentAnimSteps;
    cAnimation*                         pCurrentAnim;
    float                               mSpeed;
    float                               mTimeAccumulated { 0.f };
};

class cJelly : public cEntity {
public:
    cJelly(cEngine& engine, const std::string& id);
    virtual void        switchColour(EntColour) override;
    virtual void        render(sf::RenderWindow&) override;

    void                makeSuper(Direction dir = Direction::undecided);
    
protected:
    sf::Sprite          mSuperSprite;
    int                 mSuperAnimStepCounter { 0 };
    int                 mSuperAnimSteps;
    cAnimation*         pSuperAnim;
    float               mSuperTimeAccumulated { 0.f };
    
};

class cGuard: public cEntity {
    
};

class cDiamond : public cEntity {
    
};

class cBlock : public cEntity {
    
};

#endif /* defined(__not_jelly_splash__entity__) */
