#ifndef __not_jelly_splash__entity__
#define __not_jelly_splash__entity__

#include <SFML/Graphics.hpp>
#include "globalConstants.h"
#include "entityInfo.h"
#include "engine.h"

class cEntity {
public:
    cEntity(cEngine& engine, const std::string& id): rEngine { engine }
    {
        loadInfo(id);
        tLives.setFont(rEngine.mFontHolder.get(FontID::defaultFont));
        tLives.setCharacterSize(18);
        tLives.setColor(sf::Color::Cyan);
    }
    
    void            setPos(float x, float y) { mPos.x = x; mPos.y = y; }
    void            setGoal(float x, float y);
    void            predictDamage(int);
    
    virtual void    switchColour(EntColour);
    virtual void    render(sf::RenderWindow&);
    virtual void    update(float dt);
    virtual void    explode(int dmg = 1);
    virtual void    switchToAnim(const std::string&);

protected:
    void            loadInfo(const std::string& id);
    
public:
    EntType                             mType;
    EntColour                           mColour;
    bool                                mFallible { false };
    EntState                            mState { EntState::normal };
    bool                                mNeedToMove { false };
    Direction                           mDir { Direction::undecided };  // super jelly orientation
    bool                                mSuper { false };   // super jelly?
    int                                 mLives { 1 };
    int                                 mPredictedDmg { 0 };
    
    sf::Text                            tLives;
    
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
    virtual void        render(sf::RenderWindow&) override;
    virtual void        update(float dt) override;
    
    void                makeSuper(Direction dir = Direction::undecided);
    void                force(Direction dir);
    void                setLives(int x);
    
    
protected:
    sf::Sprite          mSuperSprite;
    int                 mSuperAnimStepCounter { 0 };
    int                 mSuperAnimSteps;
    cAnimation*         pSuperAnim;
    float               mSuperTimeAccumulated { 0.f };
    bool                mSwitched { false };
    
};

class cGuard: public cEntity {
public:
    cGuard(cEngine& engine, const std::string& id);
};

class cDiamond : public cEntity {
public:
    cDiamond(cEngine& engine, const std::string& id);
    
};

class cBlock : public cEntity {
    
};

#endif /* defined(__not_jelly_splash__entity__) */
