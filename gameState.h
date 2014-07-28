#ifndef __not_jelly_splash__gameState__
#define __not_jelly_splash__gameState__

#include "engine.h"
#include "board.h"
#include "entity.h"
#include <SFML/Graphics.hpp>

class cGameState : public cState {
public:
    cGameState(cEngine&);
    
    virtual void        init();
    virtual void        run();
    virtual void        cleanup();
    virtual void        reset();
    virtual void        render();

private:
    void                setUpGraphics();
    void                scrollBoard(float dt);  // dt in seconds

private:
    
    cBoard              mBoard { rEngine };
    sf::Clock           mClock;
    int                 mSizeX, mSizeY; // size of the board
    
    sf::Vertex*         pBoardVA;       // vertex array for the board
    sf::RenderStates    mBoardState;
    sf::Vector2f        mBoardPos { 0, 0 };
    sf::Vector2f        mTargetPos { 0, 0 };
    bool                mNeedToScroll { false };
};
#endif /* defined(__not_jelly_splash__gameState__) */
