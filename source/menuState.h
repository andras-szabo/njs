#ifndef __not_jelly_splash__menuState__
#define __not_jelly_splash__menuState__

#include "engine.h"
#include "guiMenu.h"

class cMenuState : public cState {
public:
    cMenuState(cEngine&);
    
    virtual void    init();
    virtual void    run();
    virtual void    cleanup();
    virtual void    reset();
    virtual void    render();
    
private:
    cMenu           mMainMenu { rWindow };
    sf::Text        tTitle;
    sf::Text        tCredits;
};

#endif /* defined(__not_jelly_splash__menuState__) */
