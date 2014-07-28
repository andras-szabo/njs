#include "gameState.h"
#include <cmath>

cGameState::cGameState(cEngine& engine):
cState { engine, "game" }
{

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
    mBoardState.texture = &rEngine.mTextureHolder.get(TexID::assets);
}

void cGameState::init()
{
    // Load level, reCreate and fill up board:
    // mBoard.reCreate(x, y, top, bottom);
    // iterate through file:
    //      - mBoard.set(relevant x, relevant y, type)
    //      - fill empty ones randomly
    
    // Set up the vertexArray of the board
    pBoardVA = new sf::Vertex[ mSizeX * mSizeY * 4 ];
    setUpGraphics();
    
}

void cGameState::render()
{
    // Render background
    //      - this is simple: render some background texture ( perhaps not even needed ).
    
    // Render board
    //      - the board is rendered as a vertexarray, using the texture for background tiles.
    //      - the vertexarray is created once and then not touched again; for the background,
    //        the only relevant qualities are: is this a darker green patch, or a lighter green
    //        one, or is this inaccessible.
    //      - when scrolling, the vertexarray's y coords are modified, but that's it.
    //
    // rWindow.draw(&pBoardVA[0], mSizeX * mSizeY * 4, sf::Quads, mBoardState);

    
    // Render jellies
    // Render game info
}

// run() is called once every frame.
void cGameState::run()
{
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