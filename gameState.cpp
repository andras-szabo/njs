#include "gameState.h"
#include <fstream>
#include <cmath>

cGameState::cGameState(cEngine& engine, const std::string& sLevel):
cState { engine, "game" },
mState { GameState::waiting }
{
    loadLevel(sLevel);
    fillLevel();
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
        for ( int j = 0; j < mSizeY; ++j )
        {
            if ( mBoard.at(i,j) == 0 )                  // empty
            {
                mBoard.place(i, j, EntType::jelly);     // default: random colour
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
    
    mBoard.reCreate(mSizeX, mSizeY, mTop, mBottom);
    
    std::string     stmp;
    int             xtmp, ytmp;
    inFile >> stmp;
    while ( stmp != "END" )
    {
        inFile >> xtmp >> ytmp;
        if ( stmp == "INACC" )
        {
            mBoard.set(xtmp, ytmp, -1);
        }
        else if ( stmp == "SLIME" )
        {
            mBoard.set(xtmp, ytmp, 16);     // 00010000  -> slime bit
        }
        else if ( stmp == "GUARD" )
        {
            mBoard.place(xtmp, ytmp, EntType::guard);
        }
        else if ( stmp == "BLOCK" )
        {
            mBoard.place(xtmp, ytmp, EntType::block);
        }
        else if ( stmp == "STUCK" )
        {
            short int ztmp, qtmp;
            inFile >> ztmp >> qtmp;
            
            // spawn stuck jelly, of colour ztmp, and with qtmp number of lives
        }
        inFile >> stmp;
    }
    
    inFile.close();
    
    // Level structure:
    // name
    // rows columns top bottom
    // onestar_points twostar_points threestar_points
    // INACC x y            <--- inaccessible terrain
    // SLIME x y            <--- grey slime
    // GUARD x y            <--- guard
    // BLOCK x y            <--- block
    // STUCK x y colour lives <---- stuck jelly
    // END
    
    // Create "visited" structure:
    for ( int i = 0; i < mSizeX; ++i )
        visited.push_back(std::vector<bool>(mSizeY, false));
    
}

void cGameState::scrollBoard(float dt)
{
    sf::Vector2f v { mTargetPos - mBoardPos };

    float length = sqrtf(pow(v.x, 2) + pow(v.y, 2));
    v.x /= length;
    v.y /= length;
    
    mBoardPos += v * gkScrollSpeed * dt;

    if ( length < 0.001 )
    {
        mBoardPos = mTargetPos;
        mNeedToScroll = false;
    }
    
    mBoardState.transform.translate(mBoardPos);
}

void cGameState::setUpGraphics()
{
    // Set up alternating darker and lighter board backgrounds;
    // and mark inaccessible fields; initialize coordinates
    
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = 0; j < mSizeY; ++j )
        {
            size_t p = (i * 4) + (j * 4 * mSizeX);
            pBoardVA[p].position    = sf::Vector2f { i * gkCellPixSizeX, j * gkCellPixSizeY };
            pBoardVA[p+1].position  = sf::Vector2f { (i+1) * gkCellPixSizeX - 1 , j * gkCellPixSizeY };
            pBoardVA[p+2].position  = sf::Vector2f { (i+1) * gkCellPixSizeX - 1, (j+1) * gkCellPixSizeY - 1 };
            pBoardVA[p+3].position  = sf::Vector2f { i * gkCellPixSizeX, (j+1) * gkCellPixSizeY - 1 };
            
            sf::Vector2f texcoords;
            
            if ( mBoard.closed(i,j) )       // inaccesible terrain!
            {
                texcoords = gkClosedBkgTexCoords;
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
}

void cGameState::init()
{
    // Set up the vertexArray of the board
    pBoardVA = new sf::Vertex[ mSizeX * mSizeY * 4 ];
    setUpGraphics();
}

void cGameState::render()
{
    // Render background
    //      - this is simple: render some background texture ( perhaps not even needed ).
    
    // Render board
    //      - low priority: only draw the actually visible parts of the vertexarray
    
    rWindow.draw(&pBoardVA[0], mSizeX * mSizeY * 4, sf::Quads, mBoardState);

    // Now remove hilights from everywhere; much refactoring here,
    // yea, is advised.
    
    for ( auto x = 0; x < mSizeX; ++x )
        for ( auto y = mTop; y <= mBottom; ++y )
            removeHilight(sf::Vector2i(x,y));

    // Render connections

    // Render jellies; to make sure, start with one line above mTop.
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = mTop-1; j <= mBottom; ++j )
        {
            cEntity* p = mBoard.piece(i,j);
            if ( p != nullptr ) p->render(rWindow);
        }

    // Render game info
}

sf::Vector2i cGameState::screenToBoardTranslate(sf::Vector2i v)
{
    sf::Vector2i ret;
    ret.x = (v.x - gkScrLeft) / gkCellPixSizeX;
    ret.y = (v.y - gkScrTop ) / gkCellPixSizeY + mTop;
    return ret;
}

// Are two positions actually adjacent?
bool cGameState::adjacent(sf::Vector2i a, sf::Vector2i b)
{
    return abs(a.x - b.x) <= 1 && abs(a.y - b.y) <= 1;
}

void cGameState::switchToState(GameState s)
{
    mState = s;
    
    switch ( s )
    {
        case GameState::executing:
        {
            itExplode = mToBlowUp.begin();
            mAccumulatedTime = gkExplosionDelay;    // make sure to blow the first up right away
            mToExplode = mToBlowUp.size();
            break;
        }
        case GameState::refilling:
        {
            break;
        }
        case GameState::aftermath:
        {
            break;
        }
        case GameState::waiting:
        {
            break;
        }
    }
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
                if ( mTouchedFields.size() > 2 )
                {
                    switchToState(GameState::executing);
                }
                else
                {
                    mButtonPressed = false;
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
    }
}

void cGameState::prepareHilight(sf::Vector2i v)
{
    mBoard.mark(v, true);
    auto p = v.x + v.y * mSizeX;
    pBoardVA[p].color += gkHilightColor;
    pBoardVA[p+1].color += gkHilightColor;
    pBoardVA[p+2].color += gkHilightColor;
    pBoardVA[p+3].color += gkHilightColor;
}

void cGameState::removeHilight(sf::Vector2i v)
{
    mBoard.mark(v, false);
    auto p = v.x + v.y * mSizeX;
    pBoardVA[p].color = sf::Color::Black;
    pBoardVA[p+1].color = sf::Color::Black;
    pBoardVA[p+2].color = sf::Color::Black;
    pBoardVA[p+3].color = sf::Color::Black;
}

// Oh god refactor this srsly
void cGameState::hilight(sf::Vector2i v)
{
    switch ( mLastSuperDirection ) {
        case Direction::undecided: mLastSuperDirection = mBoard.piece(v.x, v.y)->mDir; break;
        case Direction::horizontal: mLastSuperDirection = Direction::vertical; break;
        case Direction::vertical: mLastSuperDirection = Direction::horizontal; break;
    }
    
    sf::Vector2i tmpv = v;
    
    if ( mLastSuperDirection == Direction::horizontal )
    {
        for ( int x = 0; x < mSizeX; ++x )
        {
            tmpv.x = x;
            if ( x != v.x && mBoard.canBlowUp(tmpv) ) // canBlowUp checks if it's already marked, too
            {
                prepareHilight(tmpv);
                mToBlowUp.push_back(tmpv);
                if ( mBoard.piece(x, v.y)->mType == EntType::superJelly )
                {
                    hilight(tmpv);
                }
            }
        }
    }
    else
    {
        for ( int y = mTop; y < mBottom; ++y )
        {
            tmpv.y = y;
            if ( y != v.y && mBoard.canBlowUp(tmpv) ) // canBlowUp checks if it's already marked, too
            {
                prepareHilight(tmpv);
                mToBlowUp.push_back(tmpv);
                if ( mBoard.piece(v.x, y)->mType == EntType::superJelly )
                    hilight(tmpv);
            }
        }
    }
    
}

// Prepares hilights that will mark those fields on the board
// which are going to be affected by the current move input by the user
void cGameState::predictOutcome()
{
    mToBlowUp.clear();
    for ( const auto& i : mTouchedFields )
    {
        if ( !mBoard.marked(i) )                        // not already marked?
        {
            prepareHilight(i);
            mToBlowUp.push_back(i);
            
            if ( mBoard.piece(i.x, i.y)->mType == EntType::superJelly )
            {
                hilight(i);     // Will recursively hilight everything
            }
        }
    }
}

void cGameState::proceedWithExplosions(sf::Time dt)
{
    // itExplode points to the next item we should blow up
    // if it points to mTouchFields.end(), then there's
    // nothing left to start exploding.
    
    if ( itExplode != mTouchedFields.end() )
    {
        mAccumulatedTime += dt;
        if ( mAccumulatedTime >= gkExplosionDelay )
        {
            mAccumulatedTime = sf::Time::Zero;
            mBoard.piece((*itExplode).x, (*itExplode).y)->explode();
            ++itExplode;
        }
    }
}

// Remove dead jellies, and check if the explosions are over
void cGameState::removeAndCheck()
{
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = mTop; j < mBottom; ++j )
        {
            if ( mBoard.piece(i,j) != nullptr && mBoard.piece(i,j)->mState == EntState::dead )
            {
                mBoard.remove(i,j);
                mToExplode--;
            }
        }
    
    if ( mToExplode == 0 )
    {
        switchToState(GameState::refilling);
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
            //prepareConnections();       // prepares the gfx for connecting jellies
            break;
        case GameState::executing:
            proceedWithExplosions(mTimeSinceLastUpdate);
            removeAndCheck();
            // Then: check if we should start refill, or whatnot
            break;
        case GameState::refilling:
            // Check move logic jne.
            break;
        case GameState::aftermath:
            // check if we're finished and can get back to
            // waiting, or there's a stageover
            break;
    }
    
    /*
     
     get mouse input events, and handle them;
        - possibly: call "start executing()", which will switch to "executing move" state
     
     if we're in "waiting for move" state:
        - prepare higlights
        - prepare "connections" line segments
     
     if we're in "executing move" state:
        -go on emptying the "explosion" queue, if sufficient time has passed;
         add points on the basis of explosions
     
        - check if everyone finished exploding
        - if so, call "start refill()", which will switch to "refill" state, and add the
          necessary number of jellies to the board
     
     if we're in "refill" state:
        - check move logic and see if we have to continue refilling; if not, call
          start aftermath() which will set up the aftermath state - or call "start waiting"
          which will put us back to waiting state, or call "stageover"
     
     if we're in "aftermath" state
        - just continue animating; if everyone finished, call "start waiting", which will put
          us back to start waiting, or call stageover
     
     update(animations);
     
     */
    
}

void cGameState::cleanup()
{
    // Free board vertexarray
    delete[] pBoardVA;
}

void cGameState::reset()
{
    
}