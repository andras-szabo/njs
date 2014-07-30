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
    void                processEvents();
    sf::Vector2i        screenToBoardTranslate(sf::Vector2i);
    bool                adjacent(sf::Vector2i, sf::Vector2i);
    void                predictOutcome();       // prepares hilights marking affected fields
    void                hilight(sf::Vector2i);
    void                prepareHilight(sf::Vector2i);
    void                removeHilight(sf::Vector2i);
    void                proceedWithExplosions(sf::Time);
    void                switchToState(GameState);
    void                removeAndCheck();
    
public:
    std::string         mLevelName;
    int                 mStarScores[3];
    GameState           mState;
    
private:
    
    cBoard              mBoard { rEngine };
    sf::Clock           mClock;
    int                 mSizeX, mSizeY; // size of the board
    int                 mTop, mBottom;
    sf::Time            mTimeSinceLastUpdate { sf::Time::Zero };
    sf::Time            mAccumulatedTime { sf::Time::Zero };
    
    sf::Vertex*         pBoardVA;       // vertex array for the board
    sf::RenderStates    mBoardState;
    sf::Vector2f        mBoardPos { 0, 0 };
    sf::Vector2f        mTargetPos { 0, 0 };
    bool                mNeedToScroll { false };
    
    bool                            mButtonPressed { false };
    Direction                       mLastSuperDirection { Direction::undecided };
    
    std::vector<sf::Vector2i>               mTouchedFields;
    std::vector<sf::Vector2i>               mToBlowUp;
    std::vector<sf::Vector2i>::iterator     itExplode;
    std::vector<std::vector<bool>>          visited;
    int                                     mToExplode;
};
#endif /* defined(__not_jelly_splash__gameState__) */
