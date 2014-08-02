#include "gameState.h"
#include <fstream>
#include <cmath>

// Helper function
template <class C, class V>
bool contains(C container, V value)
{
    return std::find(container.begin(),
                     container.end(),
                     value) != container.end();
}


// Upon construction, the level to be loaded
// is stored by rEngine.mStrParam
cGameState::cGameState(cEngine& engine):
cState { engine, "game" },
mState { GameState::waiting }
{
    loadLevel(rEngine.mStrParam);
    fillLevel();
    collectVisibleGuards();
    
    tMovesLeft.setFont(rEngine.mFontHolder.get(FontID::defaultFont));
    tMovesLeft.setCharacterSize(38);
    tMovesLeft.setColor(sf::Color::Green);
    
    tScore.setFont(rEngine.mFontHolder.get(FontID::defaultFont));
    tScore.setCharacterSize(38);
    tScore.setColor(sf::Color::Green);
    

    tKills.setFont(rEngine.mFontHolder.get(FontID::defaultFont));
    tKills.setCharacterSize(38);
    tKills.setColor(sf::Color::Green);
    
    updateScoreEtc();
}

void cGameState::updateScoreEtc()
{
    std::string stmp { "Moves\nleft: " };
    if ( mMovesLeft == -1 ) { stmp += " inf. "; }
    else { stmp += std::to_string(mMovesLeft); }
    tMovesLeft.setString(stmp);
    tMovesLeft.setPosition(gkScrLeft * 0.1, gkViewSize.y * 0.2);
    auto tWidth = tMovesLeft.getLocalBounds().width;
    auto scale = (gkScrLeft * 0.8) / tWidth;
    tMovesLeft.setScale(scale, scale);

    tScore.setString("Score   \n\t" + std::to_string(mScore));
    tScore.setPosition(gkScrLeft * 0.1, gkViewSize.y * 0.4);
    tWidth = tScore.getLocalBounds().width;
    scale = (gkScrLeft * 0.8) / tWidth;
    tScore.setScale(scale, scale);
    
    tKills.setString("Jellies\nsplashed:\n\t" + std::to_string(mJelliesKilled));
    tKills.setPosition(gkScrLeft * 0.1, gkViewSize.y * 0.6);
    tWidth = tKills.getLocalBounds().width;
    scale = (gkScrLeft * 0.8) / tWidth;
    tKills.setScale(scale, scale);
}

void cGameState::collectVisibleGuards()
{
    mVisibleGuards.clear();
    for ( const auto& i : mGuards )
    {
        if ( i.y >= mTop && i.y <= mBottom )
            mVisibleGuards.push_back(i);
    }
        
}

// Fills level up with random jellies. This is not to be confused
// with "refill", where the board gets "refilled" in the sense
// of jellies being poured on top of ones already on the board.
//
// This also checks if the starting layout is solvable, i.e. there's
// at least one triplet to be touched.
void cGameState::fillLevel()
{
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = mTop; j < mSizeY; ++j )
        {
            if ( j < mNoFillTop || j > mNoFillBottom )
            {
                if ( mBoard.empty(i,j) )                  // empty
                {
                    mBoard.place(i, j, EntType::jelly);     // default: random colour
                }
            }
        }
    
    makeSureItsPlayable();
}

// Depth first search
void cGameState::dfs(int i, int j, int& depth, int maxdepth)
{
    if ( visited[i][j] ) return;
    ++depth;
    
    visited[i][j] = true;
    if ( depth == maxdepth ) return;

    for ( int x = -1; x < 2; ++x )
        for ( int y = -1; y < 2; ++y )
            if ( (x != 0 || y != 0) && mBoard.clickable(i+x, j+y) )
                dfs(i+x, j+y, depth, maxdepth);
}

// Makes sure that there is at least one clickable triplet
// in the area between mTop and mBottom on the board.
void cGameState::makeSureItsPlayable()
{
    int depth { 0 };
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = 0; j < mSizeY; ++j )
            visited[i][j] = false;
    
    for ( int i = 0; i < mSizeX; ++i )
    {
        for ( int j = mTop; j < mBottom; ++j )
        {
            dfs(i, j, depth, 3);
            if ( depth >= 3 ) break;
        }
        if (depth >= 3 ) break;
    }
    
    // If there's no triplet found:
    // pick random node
    // if it doesn't have at least 2 neighbours, pick a new one
    // make the first 2 neighbours the same colour as itself
    
    if ( depth < 3 )
    {
        int tmpx, tmpy;
        do
        {
            tmpx = rand() % mSizeX;
            tmpy = rand() % (mBottom - mTop) + mTop;
        }
        while ( mBoard.neighbourCount(tmpx, tmpy) < 2 );
        
        mBoard.makeTriplet(tmpx, tmpy);
    }
}

// Set victory condition
void cGameState::setVcond(int* pa, int* pb)
{
    pVictoryCondition = pa;
    pVictoryGoal = pb;
}

//
// Level structure:
// name
// rows columns top bottom
// onestar_points twostar_points threestar_points
// MOVES x  <-- you start out with this many moves; if none: unlimited
// VICTORY SCORE / DIAMOND xx / SLIME / GUARD / STUCK <--- what kind of level is this
//                                            <--- with DIAMOND, xx is the number of
//                                            <--- diamonds expected to be fallen
//
// NEW_DIAMOND_AFTER n x  <---- spawn n new diamonds after x (total!) splashed jellies
//
// DIAMOND x y          <---- place diamond on level
// NOFILL y1 y2          <--- do NOT fill the are between rows [y1, y2]
// INACC x y            <--- inaccessible terrain
// SLIME x y            <--- grey slime
// GUARD x y            <--- guard
// BLOCK x y            <--- block
// STUCK x y colour lives <---- stuck jelly
// END
//
void cGameState::loadLevel(const std::string& sLevel)
{
    std::ifstream   inFile(resourcePath() + sLevel + ".lvl");
    if ( !inFile.is_open() )
    {
        throw std::runtime_error("Couldn't load level.");
    }
   
    inFile >> mLevelName;
    inFile >> mSizeX >> mSizeY >> mTop >> mBottom;
    inFile >> mStarScores[0] >> mStarScores[1] >> mStarScores[2];
    
    mBoardPos.x = 0;
    mBoardPos.y = -mTop * gkCellPixSizeY;
    
    mBoard.reCreate(mSizeX, mSizeY, mTop, mBottom);
    
    // Read victory condition
    std::string     stmp;
    
    inFile >> stmp;
    
    if ( stmp != "MOVES" )
    {
        // No move limit
        mMovesLeft = -1;
    }
    else
    {
        inFile >> mMovesLeft;
        inFile >> stmp;
    }
    
    if ( stmp != "VICTORY" )
    {
        // No victory condition; open-ended play: the
        // victory condition ( *pVictoryCondition >= *pVictoryGoal )
        // will never be met.
        setVcond(&mNullValue, &mSizeX);
    }
    else
    {
        inFile >> stmp;
        if ( stmp == "SCORE" )
        {
            setVcond(&mScore, &mStarScores[0]);
        }
        else if ( stmp == "SLIME" )
        {
            setVcond(&mNullValue, &mSlimeCount);
        }
        else if ( stmp == "GUARD" )
        {
            setVcond(&mNullValue, &mGuardCount);
        }
        else if ( stmp == "DIAMOND" || stmp == "DIAMONDS" )
        {
            inFile >> mDiamondGoal;
            setVcond(&mFallenDiamonds, &mDiamondGoal);
        }
        else if ( stmp == "STUCK" )
        {
            setVcond(&mNullValue, &mStuckCount);
        }
        inFile >> stmp;
    }

    int             xtmp, ytmp;
    while ( stmp != "END" )
    {
        inFile >> xtmp >> ytmp;
        if ( stmp == "INACC" )
        {
            mBoard.set(xtmp, ytmp, -1);
        }
        else if ( stmp == "SLIME" )
        {
            mBoard.set(xtmp, ytmp, gkSlimeBit);
            ++mSlimeCount;
            mSlimes.push_back(ytmp);
        }
        else if ( stmp == "GUARD" )
        {
            mBoard.place(xtmp, ytmp, EntType::guard);
            ++mGuardCount;
            mGuards.push_back(sf::Vector2i(xtmp, ytmp));
        }
        else if ( stmp == "BLOCK" )
        {
            mBoard.place(xtmp, ytmp, EntType::block);
        }
        else if ( stmp == "STUCK" )
        {
            short int ztmp, qtmp;
            inFile >> ztmp >> qtmp;
            
            mBoard.place(xtmp, ytmp, EntType::jelly, static_cast<EntColour>(ztmp));
            cJelly* ptr = dynamic_cast<cJelly*>(mBoard.piece(xtmp, ytmp));
            if ( ptr != nullptr ) ptr->setLives(qtmp);
            
            // spawn stuck jelly, of colour ztmp, and with qtmp number of lives
        }
        else if ( stmp == "NOFILL" )
        {
            mNoFillTop = xtmp;
            mNoFillBottom = ytmp;
        }
        else if ( stmp == "DIAMOND" )
        {
            mBoard.place(xtmp, ytmp, EntType::diamond);
        }
        else if ( stmp == "NEW_DIAMONDS_AFTER" )
        {
            for ( int i = 0; i < xtmp; ++i )
            {
                mDiamondsToSpawn.push(ytmp);
            }
        }
        inFile >> stmp;
    }
    
    inFile.close();
        
    // Create "visited" structure:
    for ( int i = 0; i < mSizeX; ++i )
        visited.push_back(std::vector<bool>(mSizeY, false));
    
    // Sort slimes, in case we have any, so that lowest is on top;
    // this is necessary for knowing how much we can/should scroll
    if ( !mSlimes.empty() )
    {
        std::sort(mSlimes.begin(), mSlimes.end());
    }
    
}

void cGameState::scrollBoard(float dt)
{
    if ( !mNeedToScroll )
    {
        switchToState(GameState::refilling);
        return;
    }
    
    sf::Vector2f v { mTargetPos - mBoardPos };

    float length = sqrtf(pow(v.x, 2) + pow(v.y, 2));
    v.x /= length;
    v.y /= length;
    
    mBoardPos += v * gkScrollSpeed * dt;

    if ( length < 2 )
    {
        mBoardPos = mTargetPos;
        mNeedToScroll = false;
    }
    
    sf::Transform t;
    t.translate(mBoardPos);
    mBoardState.transform = t;
    mBoard.moveEveryone( v * gkScrollSpeed * dt);
}

// scroll(): sets new mTop and mBottom levels;
// actual (visual) scrolling is done by
// scrollBoard(dt), see above.
// amount: amount of rows
void cGameState::scroll(int amount)
{
    mNeedToScroll = true;
    mTargetPos = mBoardPos;
    mTargetPos.y -= (amount * gkCellPixSizeY);
    
    // Scrolling down means moving the board
    // upwards, and vice versa, hence the -=.
    
    mTop += amount;
    mBottom += amount;
    
    mBoard.mTop += amount;
    mBoard.mBottom += amount;
    
    // Re-calculate visible guards
    
    collectVisibleGuards();
    
    // And make sure that guards that
    // have just only become visible can't multiply
    
    mGuardKilled = true;
}

void cGameState::setUpGraphics()
{
    // Set up alternating darker and lighter board backgrounds;
    // and mark inaccessible fields; initialize coordinates
    
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = 0; j < mSizeY; ++j )
        {
            size_t p = (i * 4) + (j * 4 * mSizeX);
            pBoardVA[p].position    = sf::Vector2f { gkScrLeft + i * gkCellPixSizeX, j * gkCellPixSizeY };
            pBoardVA[p+1].position  = sf::Vector2f { gkScrLeft + (i+1) * gkCellPixSizeX, j * gkCellPixSizeY };
            pBoardVA[p+2].position  = sf::Vector2f { gkScrLeft + (i+1) * gkCellPixSizeX, (j+1) * gkCellPixSizeY};
            pBoardVA[p+3].position  = sf::Vector2f { gkScrLeft + i * gkCellPixSizeX, (j+1) * gkCellPixSizeY };
            
            sf::Vector2f texcoords;
            
            if ( !mBoard.valid(i,j) )       // inaccesible terrain!
            {
                texcoords = gkClosedBkgTexCoords;
            }
            else if ( mBoard.slime(i,j) )
            {
                texcoords = gkSlimeTexCoords;
            }
            else
            {
                texcoords = ( i + j ) % 2 ? gkLightBkgTexCoords : gkDarkBkgTexCoords;
            }
            
            pBoardVA[p].texCoords   = texcoords;
            pBoardVA[p+1].texCoords = texcoords + sf::Vector2f { gkCellPixSizeX, 0 };
            pBoardVA[p+2].texCoords = texcoords + sf::Vector2f { gkCellPixSizeX, gkCellPixSizeY };
            pBoardVA[p+3].texCoords = texcoords + sf::Vector2f { 0, gkCellPixSizeY };
        }
    
    // Set up renderstate for board rendering
    
    mBoardState.texture = &rEngine.mTextureHolder.get("assets");
    mBoardState.blendMode = sf::BlendMode::BlendAlpha;
    mBoardState.transform.translate(mBoardPos);

}

void cGameState::init()
{
    // Set up the vertexArray of the board
    pBoardVA = new sf::Vertex[ mSizeX * mSizeY * 4 ];
    setUpGraphics();
}

void cGameState::render()
{

    // Render board
    rWindow.draw(&pBoardVA[0], mSizeX * mSizeY * 4, sf::Quads, mBoardState);

    // Now remove hilights from everywhere
    for ( auto x = 0; x < mSizeX; ++x )
        for ( auto y = mTop; y <= mBottom; ++y )
            removeHilight(sf::Vector2i(x,y));

    // Render jellies;
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = 0; j <= mBottom; ++j )
        {
            cEntity* p = mBoard.piece(i,j);
            if ( p != nullptr ) p->render(rWindow);
        }

    // Render doodads (the flying little pixiedust that marks
    // super jellies to be born
    for ( auto& i : mDoodads )
        i->render(rWindow);
    
    // Render other stuff
    rWindow.draw(tMovesLeft);
    rWindow.draw(tScore);
    rWindow.draw(tKills);
    
}

sf::Vector2i cGameState::screenToBoardTranslate(sf::Vector2i v)
{
    // First: translate between screen and view coords
    
    v.x *= ( gkViewSize.x / gkWindowSize.x );
    v.y *= ( gkViewSize.y / gkWindowSize.y );
    
    // Then translate between screen and board coords
    // However, let's add a bit of leeway - only the centre
    // of tiles should be clickable. How do we know? It depends
    // on how far we are from the edge of tiles =>
    // let's use %. If we're too far from the centre, we return
    // negative coordinates.
    
    v.x -= gkScrLeft;
    v.y -= gkScrTop;
    
    
    sf::Vector2i ret;
    ret.x = v.x / gkCellPixSizeX;
    ret.y = v.y / gkCellPixSizeY + mTop;
    
    auto m = static_cast<int>(v.x) % static_cast<int>(gkCellPixSizeX);
    if ( m < gkSideLeeway || m > gkCellPixSizeX - gkSideLeeway ) ret.x *= -1;

    m = static_cast<int>(v.y) % static_cast<int>(gkCellPixSizeY);
    if ( m < gkSideLeeway || m > gkCellPixSizeY - gkSideLeeway ) ret.y *= -1;
    
    return ret;
}

// Are two positions actually adjacent?
bool cGameState::adjacent(sf::Vector2i a, sf::Vector2i b)
{
    return abs(a.x - b.x) <= 1 && abs(a.y - b.y) <= 1;
}

// Pick a random guard, make it move.
void cGameState::makeGuardMove()
{
    if ( mGuardKilled || mGuardCount <= 0 || mVisibleGuards.size() == 0 ) return;
    
    auto mover = rand() % mVisibleGuards.size();
    
    int x = mVisibleGuards[mover].x;
    int y = mVisibleGuards[mover].y;
    bool found = false;

    // Pick an adjacent one to the left, right, or top
    // (but not bottom!), which is a normal, non-super,
    // non-stuck jelly
    
    for ( int i = -1; i < 2; ++i )
        for ( int j = -1; j < 2; ++j )
            if ( !found && ((i && !j) || (i == 0 && j == -1 )) )
            {
                if ( mBoard.normal(x+i, y+j) )
                {
                    found = true;
                    mBoard.remove(x+i, y+j);
                    mBoard.place(x+i, y+j, EntType::guard);
                    mBoard.piece(x+i, y+j)->setPos(gkScrLeft + x * gkCellPixSizeX,
                                                   gkScrTop + (y - mTop) * gkCellPixSizeY);
                    mBoard.piece(x+i, y+j)->setGoal(gkScrLeft + (x+i) * gkCellPixSizeX,
                                                    gkScrTop + (y + j - mTop) * gkCellPixSizeY);
                    mGuards.push_back(sf::Vector2i(x+i, y+j));
                    mVisibleGuards.push_back(sf::Vector2i(x+i, y+j));
                    ++mGuardCount;
                    break;
                }
            }
    
}

void cGameState::makeSupers()
{
    if ( mDoodads.size() < 1 ) return;
    for ( auto it = mDoodads.begin(); it != mDoodads.end(); )
    {
        int x, y;
        int tries = 0;
        do
        {
            x = rand() % mSizeX;
            y = rand() % (mBottom - mTop) + mTop;
        } while ( ++tries < 10 && (!mBoard.clickable(x, y) &&
                                  !contains(mPickedForPromotion, dynamic_cast<cJelly*>(mBoard.piece(x, y)))) );
        if ( tries < 10 )    // managed to find one
        {
            cJelly* ptr = dynamic_cast<cJelly*>(mBoard.piece(x,y));
            mPickedForPromotion.push_back(ptr);
            (*it)->setGoal(gkScrLeft + x * gkCellPixSizeX, gkScrTop + (y - mTop) * gkCellPixSizeY);
            (*it)->mLives = 0;
            ++it;
        }
        else                // couldn't find one, this is sadly wasted :(
        {
            it = mDoodads.erase(it);
        }
    }
}

void cGameState::checkVictoryConditions()
{
    if ( *pVictoryCondition >= *pVictoryGoal )
    {
        std::cout << "Yay!, victory!\n";
    }
}

void cGameState::switchToState(GameState s)
{
    mState = s;
    
    switch ( s )
    {
        case GameState::executing:
        {            
            if ( mMovesLeft > 0 ) --mMovesLeft;
            mGuardKilled = false;
            itExplode = mToBlowUp.begin();
            mAccumulatedTime = gkExplosionDelay;    // make sure to blow the first up right away
            mToExplode = mToBlowUp.size();
            break;
        }
        case GameState::refilling:
        {
            // To get the falling started, set mFell
            // to true. (Jellies will try to fall if: they all are in "normal"
            // state, and at least one of them managed to fall in the
            // previous iteration - as marked by mFell.)
            
            refillTop();
            mFell = true;
            break;
        }
        case GameState::aftermath:
        {
            makeSupers();
            makeGuardMove();
            break;
        }
        case GameState::scrolling:
        {
            // Check how much we can / have to scroll
            if ( mSlimes.empty() ) switchToState(GameState::waiting);   // no scroll if no slime left
            else
            {
                auto highest = *mSlimes.begin();
                if ( highest >= mBottom )   // possibly need to scroll
                {
                    auto highestDiamond = mBoard.getHighestDiamond();  // will return mBottom if no diamonds
                    auto scrollAmount = std::min(highest - mBottom + 1, highestDiamond - mTop);
                    scrollAmount = std::min(scrollAmount, (mSizeY-1) - mBottom);
                    if ( scrollAmount > 0)
                    {
                        scroll(scrollAmount);
                    }
                }
                else
                {
                    switchToState(GameState::waiting);
                }
                    
            }
            break;
        }
        case GameState::waiting:
        {
            mScore += mPredictedScore;
            updateScoreEtc();
            checkVictoryConditions();
            if ( mMovesLeft == 0 )
            {
                // Game over!
            }
            mToBlowUp.clear();
            mTouchedFields.clear();
            break;
        }
    }
}

void cGameState::spawnDoodad()
{
    std::unique_ptr<cEntity>    ptr { new cEntity(rEngine, "doodad") };
    sf::Vector2i vec = *mTouchedFields.rbegin();
    ptr->setPos(gkScrLeft + vec.x * gkCellPixSizeX, gkScrTop + (vec.y - mTop) * gkCellPixSizeY);
    mDoodads.push_back(std::move(ptr));
}

// Process events, considering current state
void cGameState::processEvents()
{
    sf::Event event;
    while ( rWindow.pollEvent(event) )
    {
        if ( event.type == sf::Event::Closed )
        {
            clearRequest();
            return;
        }
        
        if ( mState == GameState::waiting )
        {
            
            if ( event.type == sf::Event::MouseButtonPressed )
            {
                sf::Vector2i    vec = screenToBoardTranslate(sf::Mouse::getPosition(rWindow));
                if ( mBoard.clickable(vec.x, vec.y) )
                {
                    mButtonPressed = true;
                    mTouchedFields.clear();
                    mTouchedFields.push_back(vec);
                    mBoard.piece(vec.x, vec.y)->switchToAnim("awake");       // put the dude into "wake up" state
                }
            }
            
            if ( event.type == sf::Event::MouseButtonReleased )
            {
                mButtonPressed = false;
                if ( mTouchedFields.size() > 2 )
                {
                    switchToState(GameState::executing);
                }
                else
                {
                    for ( const auto& i : mTouchedFields )
                    {
                        mBoard.piece(i.x, i.y)->switchToAnim("idle");
                    }
                    mTouchedFields.clear();
                }
            }
            // pause, blah
        }
    }

    // If in active state, and the mouse is pressed,
    // check if we have to add another node as
    // being highlighted, or perhaps change higlighting
    // because of the player backtracking
    
    if ( mState == GameState::waiting && mButtonPressed )
    {
        sf::Vector2i currentPos = screenToBoardTranslate(sf::Mouse::getPosition(rWindow));
        auto it = std::find(mTouchedFields.begin(), mTouchedFields.end(), currentPos);
        if ( it == mTouchedFields.end() )
        {
            // Have to add it if a.) valid, b.) adjacent and c.) fitting in colour
            // Validity test is implicit: mTouchFields only contains valid nodes.
            if ( mBoard.colourAt(currentPos) == mBoard.colourAt(*mTouchedFields.begin()) &&
                 adjacent(currentPos, *mTouchedFields.rbegin()) )
                {
                    mBoard.piece(currentPos.x, currentPos.y)->switchToAnim("awake");
                    mTouchedFields.push_back(currentPos);
                }
            
        }
        else    // Touched field already touched before. If it's the last one, do nothing;
                // otherwise remove those from mTouched who were touched AFTER current one
        {
            ++it;
            while ( it != mTouchedFields.end() )
            {
                mBoard.piece((*it).x, (*it).y)->switchToAnim("idle");
                it = mTouchedFields.erase(it);
            }
        }
    
        // Check doodads: at any given time, the number of doodads should be mTouchedFields / 7
        int n = mDoodads.size() - mTouchedFields.size() / 7;
        if ( n < 0 )
        {
            spawnDoodad();
        }
        else
        {
            for ( int i = 0; i < n; ++i )
                mDoodads.pop_back();
        }
    
    }
}

// Prepare the highlight on the board
void cGameState::prepareHilight(sf::Vector2i v)
{
    mBoard.mark(v, true);
    auto p = v.x * 4 + v.y * 4 * mSizeX;
    pBoardVA[p].color = gkHilightColor;
    pBoardVA[p+1].color = gkHilightColor;
    pBoardVA[p+2].color = gkHilightColor;
    pBoardVA[p+3].color = gkHilightColor;
}

void cGameState::removeHilight(sf::Vector2i v)
{
    mBoard.mark(v, false);
    auto p = v.x * 4 + v.y * mSizeX * 4;
    pBoardVA[p].color = sf::Color(255, 255, 255, 255);
    pBoardVA[p+1].color = sf::Color(255, 255, 255, 255);
    pBoardVA[p+2].color = sf::Color(255, 255, 255, 255);
    pBoardVA[p+3].color = sf::Color(255, 255, 255, 255);
}

void cGameState::hilight(sf::Vector2i v, bool lastOne)
{
    if ( !lastOne )
    {
        if ( mFirstSuperDirection == Direction::undecided )     // This is the first
        {                                                       // super we touch! Jolly good, early exit.
            mFirstSuperDirection = mBoard.piece(v.x, v.y)->mDir;
            mLastSuperDirection = mFirstSuperDirection;
            return;
        }
        else    // This is ont the first super that's touched. The direction of the current one
                // thus depends on the direction of the previous one (so the direction of touched
                // supers alternates between horizontal and vertical.
        {
        switch ( mLastSuperDirection ) {
            case Direction::horizontal:
                        {
                            mLastSuperDirection = Direction::vertical;
                            cJelly* ptr = dynamic_cast<cJelly*>(mBoard.piece(v.x, v.y));
                            ptr->force(Direction::vertical);
                            break;
                        }
            case Direction::vertical:
                        {
                            mLastSuperDirection = Direction::horizontal;
                            cJelly* ptr = dynamic_cast<cJelly*>(mBoard.piece(v.x, v.y));
                            ptr->force(Direction::horizontal);
                            break;
                        }
            default: break;
        }
    }
    }
    else
    {
        // If we're here: we're now at the end of the hilighting-trip, so to say;
        // i.e. now we have to highlight the effects of the FIRST super jelly
        // touched by the user. (The effects of the first-touched super manifest
        // themselves at the LAST touched coordinate.)
        mLastSuperDirection = mFirstSuperDirection;
    }
    
    sf::Vector2i tmpv = v;
    int stepx { 0 }, stepy { 0 };
    
    if ( mLastSuperDirection == Direction::horizontal )
    {
        stepx = 1;
    }
    else
    {
        stepy = 1;
    }
    
    auto bounds = [&](int x, int y) { return x >= 0 && x < mSizeX && y >= mTop && y <= mBottom; };
    
    // Starting at the coordinate of the touched item, move away in
    // the appropriate direction (stepx or stepy), until this is possible
    // to do in either one of the two possible ways. (E.g. with vertical
    // highlight: up or down.)
    
    while ( bounds(v.x + stepx, v.y + stepy) || bounds(v.x-stepx, v.y-stepy) )
    {
        for ( int i = 1; i > -2;  i -= 2)       // i: first 1, then -1
        {
            tmpv.x = v.x + (stepx * i);   tmpv.y = v.y + (stepy * i);
            if ( mBoard.canBlowUp(tmpv) )
            {
                mPredictedScore += gkJellyScore;
                prepareHilight(tmpv);
                mToBlowUp.push_back(tmpv);
                if ( mBoard.piece(tmpv.x, tmpv.y)->mSuper )
                {
                    mPredictedScore += gkSuperScore;
                    hilight(tmpv);
                }
            }
        }
        if ( stepx >= 1 ) ++stepx; else ++stepy;
    }
    
}

// Prepares hilights that will mark those fields on the board
// which are going to be affected by the current move input by the user
void cGameState::predictOutcome()
{
    mToBlowUp.clear();
    mFirstSuperDirection = Direction::undecided;
    mLastSuperDirection = Direction::undecided;
    mPredictedScore = 0;
    for ( const auto& i : mTouchedFields )
    {
        if ( !mBoard.marked(i) )                        // not already marked?
        {
            mPredictedScore += gkJellyScore;
            prepareHilight(i);              // mark it, hilight its background
            mToBlowUp.push_back(i);         // add node to the list of positions to be blown up
            
            if ( mBoard.piece(i.x, i.y)->mSuper )
            {
                mPredictedScore += gkSuperScore;
                hilight(i);     // Will recursively hilight everything
            }
        }
    }
    
    // If mFirstSuperDirection is not undecided, that means that at least
    // one super was touched, so as the last thing to do, we need to
    // highlight the consequence of it being touched.
    
    if ( mFirstSuperDirection != Direction::undecided )
    {
        hilight(*mTouchedFields.rbegin(), true);
    }
    
    for ( const auto& i : mToBlowUp )
        mBoard.piece(i.x, i.y)->predictDamage(mToBlowUp.size());
    
    mPredictedScore *= 1 + (mToBlowUp.size() / 3);
}

void cGameState::removeGuard(sf::Vector2i vec)
{
    --mGuardCount;
    mGuardKilled = true;
    mGuards.erase(std::find(mGuards.begin(), mGuards.end(), vec));
    mVisibleGuards.erase(std::find(mVisibleGuards.begin(), mVisibleGuards.end(), vec));
}

void cGameState::proceedWithExplosions(sf::Time dt)
{
    // itExplode points to the next item we should blow up
    // if it points to mTouchFields.end(), then there's
    // nothing left to start exploding.
    
    if ( itExplode != mToBlowUp.end() )
    {
        mAccumulatedTime += dt;
        if ( mAccumulatedTime >= gkExplosionDelay )
        {
            mAccumulatedTime = sf::Time::Zero;
            auto x = (*itExplode).x;
            auto y = (*itExplode).y;
            if ( mBoard.guard(x, y) )
            {
                removeGuard(sf::Vector2i(x, y));
            }
            mBoard.piece(x, y)->explode(mToBlowUp.size());
            ++mJelliesKilled;
            
            // Now check if there's a guard nearby, and if so,
            // make that explode as well.
 
            if ( contains(mTouchedFields, sf::Vector2i(x, y)) )
            {
                for ( int i = -1; i < 2; ++i )
                    for ( int j = -1; j < 2; ++j )
                    {
                        if ( (i & !j) || (j & !i))      // exactly one of them is 0;
                                                        // so explosions won't work
                                                        // diagonally; also: only blow up
                                                        // those who haven't been already blown up
                        {
                            if ( mBoard.guard(x+i, y+j)
                                && !contains(mToBlowUp, sf::Vector2i(x+i, y+j))
                                && contains(mVisibleGuards, sf::Vector2i(x+i, y+j)) )
                            {
                                mBoard.piece(x+i, y+j)->explode();
                                removeGuard(sf::Vector2i(x+i, y+j));
                                ++mToExplode;
                            }
                        }
                    }
            }
            ++itExplode;
        }
    }
}

// Remove dead jellies, and check if the explosions are over
void cGameState::removeAndCheck()
{
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = mTop; j <= mBottom; ++j )
        {
            if ( mBoard.piece(i,j) != nullptr && mBoard.piece(i,j)->mState == EntState::explOver )
            {
                --mToExplode;
                if ( mBoard.piece(i,j)->mLives <= 0 )
                {
                    // If the dead jelly was not a guard, then we have to remove
                    // the slime, (that may have been under it), and reset
                    // appropriate coords of the vertexarray
                    if ( mBoard.piece(i,j)->mType == EntType::jelly && mBoard.slime(i,j) )
                    {
                        mSlimes.erase(std::find(mSlimes.begin(), mSlimes.end(), j));
                        --mSlimeCount;
                        mBoard.set(i, j, 0);
                        
                        auto p = i * 4 + j * 4 * mSizeX;
                        auto texcoords = ( i + j ) % 2 ? gkLightBkgTexCoords : gkDarkBkgTexCoords;
                    
                        pBoardVA[p].texCoords   = texcoords;
                        pBoardVA[p+1].texCoords = texcoords + sf::Vector2f { gkCellPixSizeX, 0 };
                        pBoardVA[p+2].texCoords = texcoords + sf::Vector2f { gkCellPixSizeX, gkCellPixSizeY };
                        pBoardVA[p+3].texCoords = texcoords + sf::Vector2f { 0, gkCellPixSizeY };
                    }
                    
                    mBoard.remove(i,j);
                }
                else { mBoard.piece(i,j)->mState = EntState::normal; }
            }
        }
    
    if ( mToExplode <= 0 )
    {
        switchToState(GameState::refilling);
    }
}

// OK so the following happens here.
// Each iteration, fill up the line just above mTop, wherever
// this is possible (i.e. inaccessible terrain stays inaccessible
// even when we've scrolled on). Then, make entities fall, 1 row
// a time. When entities finished falling, refill above-the-top
// line; and continue falling until this is not possible anymore.
void cGameState::refillTop()
{
    // Fill up above-the-top line. mBoard.place() won't do anything
    // if that's an invalid field; and it'll overwrite whatever is
    // on that position, in case there's something there - with the
    // exception of previously spawned diamonds.
    for ( int x = 0; x < mSizeX; ++x )
    {
        if ( !mBoard.diamond(x, mTop-1) )
        {
            mBoard.place(x, mTop-1, EntType::jelly);
        }
    }
    
    // However, if there are diamonds to spawn, let's spawn 'em here.
    if ( !mDiamondsToSpawn.empty() && mJelliesKilled >= mDiamondsToSpawn.front())
    {
        mDiamondsToSpawn.pop();
        // Look for a place where we are sure to have a free way
        int x = 0;
        while ( x < mSizeX && (mBoard.diamond(x, mTop-1) || mBoard.closed(x, mTop-1) || !mBoard.freeColumn(x)))
        {
            ++x;
        }
        if ( x == mSizeX )  // won't be able to spawn it reliably from top => let's just put it on board
        {                   // at the first place it'll fit
            bool found = false;
            for ( int x = 0; x < mSizeX; ++x )
            {
                for ( int y = mTop; y <= mBottom; ++y )
                {
                    if ( !mBoard.closed(x,y) && !mBoard.diamond(x,y) )
                    {
                        mBoard.place(x, y, EntType::diamond);
                        found = true;
                        break;
                    }
                }
                if ( found == true )
                    break;
            }
        }
        else    // Yup, we can drop it in from top
        {
            mBoard.place(x, mTop-1, EntType::diamond);
        }
    }
    
}

// The recursive fall function. If vertOnly is true, then
// vertical fall is prioritized. To see if a given node
// can fall, check things below it, and fall there if possible;
// if not, recursively check the ones below (i.e. whether it'd
// be possible to fall if the thing below us fell as well).
// Basic idea: falling from p,q to x,y.
bool cGameState::fallTo(int p, int q, int x, int y, bool vertOnly)
{
    if ( p < 0 || p >= mSizeX || q < 0 || q > mBottom ) return false;
    if ( x < 0 || x >= mSizeX || y < 0 || y > mBottom ) return false;

    if ( mBoard.fallible(p,q) && mBoard.piece(p,q)->mState != EntState::moving )
    {
        if ( mBoard.empty(x,y) )
        {
            mBoard.executeFall(p, q, x, y);
            return true;
        }
        if ( !vertOnly && x > 0 && mBoard.empty(x-1, y) )
        {
            mBoard.executeFall(p, q, x-1, y);
            return true;
        }
        if ( !vertOnly && x < mSizeX-1 && mBoard.empty(x+1, y) )
        {
            mBoard.executeFall(p, q, x+1, y);
            return true;
        }
        
        if ( mBoard.fallible(x,y) && fallTo(x,y,x,y+1,vertOnly) )
        {
            mBoard.executeFall(p,q,x,y);
            return true;
        }
        
        if ( !vertOnly && x > 0 && mBoard.fallible(x-1, y) && fallTo(x-1,y,x-1,y+1,vertOnly) )
        {
            mBoard.executeFall(p,q,x-1,y);
            return true;
        }
        
        if ( !vertOnly && x < mSizeX-1 && mBoard.fallible(x+1, y) && fallTo(x+1,y,x+1,y+1,vertOnly) )
        {
            mBoard.executeFall(p,q,x+1,y);
            return true;
        }
    }
    
    return false;
}

void cGameState::proceedWithMovement()
{
    bool over { true };
    for ( int x = 0; x < mSizeX; ++x )
    {
        for ( int y = mTop; y <= mBottom; ++y )
            if ( mBoard.piece(x,y) != nullptr && mBoard.piece(x,y)->mState == EntState::moving )
            {
                over = false;
                break;
            }
        if ( over == false )
            break;
    }
    
    for ( const auto& i : mDoodads )
    {
        if ( i->mState == EntState::moving )
            over = false;
    }
    
    if ( !over ) return;
    
    if ( mState == GameState::aftermath )
    {
        switchToState(GameState::scrolling);
    }
    else
    {
        switchToState(GameState::waiting);
    }
}

void cGameState::proceedWithFalling()
{
    if ( !mFell )
    {
        // Falling is over - it's impossible to fall any more => check
        // if diamonds have reached the bottom, in which case
        // remove them &c
        for ( int x = 0; x < mSizeX; ++x )
        {
            if ( mBoard.diamond(x, mBottom) )
            {
                mFell = true;
                ++mFallenDiamonds;
                mBoard.remove(x, mBottom);
            }
        }
        
        // So: clear above-the-top line, and go on to aftermath - if no diamonds fell.
        if ( !mFell )
        {
            for ( int x = 0; x < mSizeX; ++x )
            {
                if ( mBoard.clickable(x, mTop-1) )
                {
                    mBoard.remove(x, mTop-1);
                }
            }
            switchToState(GameState::aftermath);
        }
    }
    
    // Won't proceed until after all pieces have stopped moving
    // (i.e. wait until there's no piece in EntState::moving)
    bool proceed { true };
    for ( int i = 0; i < mSizeX; ++i )
    {
        for ( int j = mTop; j <= mBottom; ++j )
        {
            if ( mBoard.piece(i,j) != nullptr && mBoard.piece(i,j)->mState == EntState::moving )
            {
                proceed = false;
                break;
            }
        }
        if ( !proceed ) break;
    }
    
    if ( !proceed ) return;         // we'll wait until each jelly finishes falling
    
    refillTop();
    
    // Now iterate through each column from bottom to top, and try to make things
    // fall. fallTo() takes care of the actual falling; and returns true if
    // it was possible to fall.
    // outerLoop makes sure that we try twice, and that things fall in order:
    // vertical falls have priority, so things fall straight down until they
    // can ( outerLoop == 0 => fallTo()'s "vertOnly" parameter is true); then,
    // if that resulted in no falling, try sideways too (outerLoop == 1 => vertOnly == false).
    
    mFell = false;
    
    for ( int outerLoop = 0; outerLoop < 2; ++outerLoop )
    {
        for ( int i = 0; i < mSizeX; ++i )
        {
            for ( int j = mBottom - 1; j >= mTop-1; --j )
            {
                bool well = fallTo(i, j, i, j+1, outerLoop == 0);
                mFell = well || mFell;
            }
        }
        
        if ( mFell == true ) break;
    }
}

// run() is called once every frame.
void cGameState::run()
{
    mTimeSinceLastUpdate = mClock.restart();

    processEvents();
    
    switch ( mState ) {
        case GameState::waiting:
            predictOutcome();           // prepares and highlights outcome of move
            break;
        case GameState::executing:
            proceedWithExplosions(mTimeSinceLastUpdate);
            removeAndCheck();
            break;
        case GameState::refilling:
            proceedWithFalling();
            break;
        case GameState::aftermath:
            proceedWithMovement();
            break;
        case GameState::scrolling:
            scrollBoard(mTimeSinceLastUpdate.asSeconds());
            break;
    }
   
    // Update everyone who's visible; check their lives to
    // see if stuck jellies are still stuck
    mStuckCount = 0;
    for ( int x = 0; x < mSizeX; ++ x )
        for ( int y = mTop; y <= mBottom; ++y )
        {
            auto p = mBoard.piece(x, y);
            if ( p != nullptr )
            {
                p->update(mTimeSinceLastUpdate.asSeconds());
                if ( p->mLives > 1 ) ++mStuckCount;
            }
        }
    
    // ...including doodads
    auto target = mPickedForPromotion.begin();
    for ( auto it = mDoodads.begin(); it != mDoodads.end(); )
    {
        (*it)->update(mTimeSinceLastUpdate.asSeconds());
        if ( (*it)->mLives < 1 && (*it)->mState != EntState::moving )
        {
            // If we're here, the doodad pointed at by *it has reached
            // its destination, so we can create that super and del the doodad
            (*target)->makeSuper();
            target = mPickedForPromotion.erase(target);
            it = mDoodads.erase(it);
        }
        else
        {
            ++it;
            ++target;
        }
    }
    
}

void cGameState::cleanup()
{
    // Free board vertexarray
    delete[] pBoardVA;
}

void cGameState::reset()
{
    
}