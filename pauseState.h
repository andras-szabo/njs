
#ifndef __not_jelly_splash__pauseState__
#define __not_jelly_splash__pauseState__

#include "state.h"
#include "guiMenu.h"

class cPauseState : public cState {
public:
    cPauseState(cEngine&);
    
    virtual void        init();
    virtual void        run();
    virtual void        reset();
    virtual void        cleanup();
    virtual void        render();
    
private:
    cMenu               mPauseMenu { rWindow };
    sf::RectangleShape  mBackground;
    
};

#endif /* defined(__not_jelly_splash__pauseState__) */
