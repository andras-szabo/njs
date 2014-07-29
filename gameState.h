#ifndef __not_jelly_splash__gameState__
#define __not_jelly_splash__gameState__

#include "engine.h"
#include "board.h"
#include "entity.h"
#include <SFML/Graphics.hpp>

class cGameState : public cState {
public:
    cGameState(cEngine&, const std::string&);       // ctor's 2nd argument: level to be loaded
    
    virtual void        init();
    virtual void        run();
    virtual void        cleanup();
    virtual void        reset();
    virtual void        render();

private:
    void                setUpGraphics();
    void                loadLevel(const std::string&);
    void                fillLevel();
    void                makeSureItsPlayable();
    void                dfs(int, int, int&, int);
    void                scrollBoard(float dt);  // dt in seconds

public:
    std::string         mLevelName;
    int                 mStarScores[3];
    GameState           mState;
    
private:
    
    cBoard              mBoard { rEngine };
    sf::Clock           mClock;
    int                 mSizeX, mSizeY; // size of the board
    int                 mTop, mBottom;
    
    sf::Vertex*         pBoardVA;       // vertex array for the board
    sf::RenderStates    mBoardState;
    sf::Vector2f        mBoardPos { 0, 0 };
    sf::Vector2f        mTargetPos { 0, 0 };
    bool                mNeedToScroll { false };
    
    std::vector<std::vector<bool>>  visited;
};
#endif /* defined(__not_jelly_splash__gameState__) */
