#include "entity.h"
#include "globalConstants.h"

cJelly::cJelly(cEngine& engine, EntColour c):
cEntity { engine }
{

    mColour = c;
    
    // OK so based on the colour and the type,
    // load the appropriate entityInfo
    
    std::string     id;

    if ( c == EntColour::random )
    {
        c = static_cast<EntColour>(rand() % 4 + 1);
    }
    
    switch (c) {
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
    mAnimStepCounter = 0;
    
    mSpeed = gkJellySpeed;
    
}
