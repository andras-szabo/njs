#ifndef __not_jelly_splash__messageState__
#define __not_jelly_splash__messageState__

#include "state.h"
#include <SFML/Graphics.hpp>

class cMessageState : public cState {
public:
    cMessageState(cEngine&);
    
    virtual void        init();
    virtual void        run();
    virtual void        reset();
    virtual void        cleanup();
    virtual void        render();
    
private:
    sf::RectangleShape      mBackground;
    sf::Text                tMessageOne;
    sf::Text                tMessageTwo;
    sf::Text                tTitle;
    bool                    mNewHiScore { false };
    
};

#endif /* defined(__not_jelly_splash__messageState__) */
