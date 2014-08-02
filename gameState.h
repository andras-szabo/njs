#ifndef __not_jelly_splash__gameState__
#define __not_jelly_splash__gameState__

#include "engine.h"
#include "board.h"
#include "entity.h"
#include <SFML/Graphics.hpp>
#include <queue>

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
    void                loadLevel(const std::string&);
    void                fillLevel();
    void                makeSureItsPlayable();
    void                dfs(int, int, int&, int);
    void                scrollBoard(float dt);  // dt in seconds
    void                processEvents();
    sf::Vector2i        screenToBoardTranslate(sf::Vector2i);
    bool                adjacent(sf::Vector2i, sf::Vector2i);
    void                predictOutcome();       // prepares hilights marking affected fields
    void                hilight(sf::Vector2i, bool lastOne = false);
    void                prepareHilight(sf::Vector2i);
    void                removeHilight(sf::Vector2i);
    void                proceedWithExplosions(sf::Time);
    void                switchToState(GameState);
    void                removeAndCheck();
    void                refillTop();
    void                proceedWithFalling();
    void                proceedWithMovement();
    bool                fallTo(int, int, int, int, bool vertOnly = false);
    void                makeGuardMove();
    void                spawnDoodad();
    void                makeSupers();
    void                removeGuard(sf::Vector2i);
    void                scroll(int amount);
    void                collectVisibleGuards();
    void                setVcond(int*, int*);
    void                checkVictoryConditions();
    void                updateScoreEtc();
    void                prepareIntroMessage();
    
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
   

    bool                mButtonPressed { false };
    Direction           mLastSuperDirection { Direction::undecided };
    Direction           mFirstSuperDirection { Direction::undecided };

    bool                mFell { false };
    bool                mGuardKilled { false };
  
    int                 mFallenDiamonds { 0 };
    int                 mGuardCount { 0 };
    int                 mSlimeCount { 0 };
    
    int                 mNoFillTop { -1 }, mNoFillBottom { -1 };
  
    std::vector<cJelly*>                    mPickedForPromotion;
    std::vector<std::unique_ptr<cEntity>>   mDoodads;
    
    std::vector<int>                        mSlimes;    // Keeping track of the slime which is
                                                        // furthest on top, for scrolling purposes
    std::vector<cEntity*>                   mDiamonds;  // Keeping track on diamonds and their position
    std::queue<int>                         mDiamondsToSpawn;
    
    std::vector<sf::Vector2i>               mGuards;
    std::vector<sf::Vector2i>               mVisibleGuards;

    std::vector<sf::Vector2i>               mTouchedFields;
    std::vector<sf::Vector2i>               mToBlowUp;
    std::vector<sf::Vector2i>::iterator     itExplode;
    std::vector<std::vector<bool>>          visited;
    int                                     mToExplode;
  
    std::string                             mLevelType;
    int*                                    pVictoryCondition { nullptr };
    int*                                    pVictoryGoal { nullptr };
    int                                     mDiamondGoal { 0 };
    int                                     mNullValue { 0 };
   
    int                                     mStuckCount { 0 };
    int                                     mScore { 0 };
    int                                     mJelliesKilled { 0 };
    int                                     mMovesLeft { 0 };
    int                                     mPredictedScore { 0 };
    
    sf::Text                                tMovesLeft;
    sf::Text                                tScore;
    sf::Text                                tKills;
};
#endif /* defined(__not_jelly_splash__gameState__) */
