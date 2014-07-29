#include "entity.h"
#include "globalConstants.h"
#include <cmath>


void cEntity::setGoal(float x, float y)
{
    mGoal.x = x; mGoal.y = y;
    sf::Vector2f    d = mGoal - mPos;
    float           dist = sqrt(pow(d.x, 2) + pow(d.y, 2));

    dist *= mSpeed;
    
    d.x /= dist;
    d.y /= dist;
    
    mVel.x = d.x;
    mVel.y = d.y;
    
    mNeedToMove = true;
}

void cJelly::loadInfo()
{
    // OK so based on the colour and the type,
    // load the appropriate entityInfo
    
    std::string     id;
    
    if ( mColour == EntColour::random )
    {
        mColour = static_cast<EntColour>(rand() % 4 + 1);
    }
    
    switch (mColour) {
        case EntColour::blue:   id = "blueJelly";   break;
        case EntColour::green:  id = "greenJelly";  break;
        case EntColour::purple: id = "purpleJelly"; break;
        case EntColour::red:    id = "redJelly";    break;
        case EntColour::yellow: id = "yellowJelly"; break;
        default: break;
    }
    
    const cEntityInfo& info = rEngine.mEntityHolder.get(id);
    
    // Load our own animations, so we don't have to go through the engine
    // every time we need a new anim phase
    
    // For instance, info.mAnimations[0] is likely to be named "idle",
    // and contain all the parameters for the idle animation
    
    for ( const auto& i : info.mAnimations )
    {
        cAnimation anim = rEngine.mAnimationHolder.get(i);
        mAnims[anim.mName] = anim;
    }
    
    // Locate texture; set "still" as basic anim, set anim
    // phase counter to 0
    
    mSprite.setTexture(rEngine.mTextureHolder.get("jellies"));
    mSprite.setTextureRect(mAnims["still"].mFirstPhaseRect);
    pCurrentAnim = &mAnims["still"];
    mAnimStepCounter = 0;
    mCurrentAnimSteps = 1;      // still is still.
}

void cJelly::update(float dt)
{
    mTimeAccumulated += dt;
    
    // Update position - if needed
    
    if ( mNeedToMove )
    {
        sf::Vector2f d = mGoal - mPos;
        float dist = (pow(d.x, 2) + pow(d.y, 2));   // first, dist squared, but
        if ( dist < 0.1 )                           // you get the idea
        {
            mPos = mGoal;
            mNeedToMove = false;
            mVel.x = 0; mVel.y = 0;
        }
        else
        {
            mPos += mVel * dt;
        }
    }
    
    // Update texture & animate via moving the texture rectangle
    // to wherever it needs to go
    
    if ( mCurrentAnimSteps > 1 )
    {
        if ( mTimeAccumulated >= pCurrentAnim->mStepTimeInMs )
        {
            mTimeAccumulated = 0.f;
            mAnimStepCounter += 1;
            mAnimStepCounter %= mCurrentAnimSteps;
            
            sf::IntRect itmp = pCurrentAnim->mFirstPhaseRect;
            itmp.left += (itmp.width * mAnimStepCounter);
            mSprite.setTextureRect(itmp);
        }
    }
    
}

void cJelly::switchColour(EntColour c)
{
    mColour = c;
    loadInfo();
    
}

cJelly::cJelly(cEngine& engine, EntColour c):
cEntity { engine }
{
    mColour = c;
    mSpeed = gkJellySpeed;
    loadInfo();
    mSpeed = gkJellySpeed;
}

void cJelly::render(sf::RenderWindow& window)
{
    mSprite.setPosition(mPos);
    window.draw(mSprite);
}
