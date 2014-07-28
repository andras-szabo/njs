#ifndef __sstc__introState__
#define __sstc__introState__

#include "engine.h"

class cIntroState : public cState {
public:
    cIntroState(cEngine&);
    
    virtual void        init();
    virtual void        run();
    virtual void        cleanup();
    virtual void        reset();
    virtual void        render();
    
private:
    sf::Clock           mClock;
    sf::Time            mFadeInTimer;
    sf::Time            mProceedTimer;
    sf::Time            mTimePassed;
    sf::Sprite          mSprite;
};

#endif /* defined(__sstc__introState__) */
