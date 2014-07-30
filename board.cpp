#include "board.h"
#include <fstream>

cBoard::cBoard(cEngine& engine, int x, int y, int bottom, int top):
rEngine { engine },
mSizeX { x },
mSizeY { y },
mBottom { bottom },
mTop { top }
{
    init();
}

void cBoard::init()
{
    // Fill up a board with empty cells (value: 0 )
    for ( int i = 0; i < mSizeX; ++i )
    {
        mCell.push_back(std::vector<short>(mSizeY, 0));
        mMarked.push_back(std::vector<bool>(mSizeY, false));
        
        // Bit cumbersome with unique ptrs, but so be it
        
        std::vector<std::unique_ptr<cEntity>> col;
        for (int j = 0; j < mSizeY; ++j )
        {
            col.push_back(std::move(nullptr));
        }
        
        mPieces.push_back(std::move(col));
    }
}

bool cBoard::marked(sf::Vector2i v) const
{
    return valid(v.x, v.y) ? mMarked[v.x][v.y] : false;
}

void cBoard::mark(sf::Vector2i v, bool flip)
{
    if ( valid(v.x, v.y) ) mMarked[v.x][v.y] = flip;
}


void cBoard::resetMarked()
{
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = 0; j < mSizeY; ++j )
            mMarked[i][j] = false;
}

bool cBoard::canBlowUp(sf::Vector2i v)
{
    // If invalid, or already marked: nope
    if ( !valid(v.x, v.y) || mMarked[v.x][v.y] ) return false;
    
    // What can't blow up? Diamonds, that's what.
    return mPieces[v.x][v.y]->mType != EntType::diamond;
}

bool cBoard::valid(int x, int y) const
{
    // Can we refer to a given pair of coordinates as valid?
    // Not if they are out of scope or pointing to inaccessible field.
    return x >= 0 && x < mSizeX && y >= mTop && y <= mBottom && mCell[x][y] != -1;
}

short cBoard::at(int x, int y) const
{
    return valid(x,y) ? -1 : mCell[x][y];
}

bool cBoard::clickable(int x, int y) const
{
    if (!valid(x, y)) return false;
    
    // Non-clickable things should have a colour of 0.
    return mPieces[x][y]->mColour != EntColour::random;
    
    //auto p = mCell[x][y] & 15;      // we're only interested in the lower
    //                                // 4 bits (0th - 3rd)
    //return p > 0 && p < 6;          // 0: empty, 6: diamond; in between jellies.
    
}

// Makes sure that mCell[] represents the actual
// state of things, i.e. the same info will be
// found here as one would get when looking at
// the unique_pointers of mPieces.
void cBoard::keepTheBooks(int x, int y)
{
    if ( valid(x, y) )
    {
        short int tmp;
        
        auto t = mPieces[x][y]->mType;
        switch ( t ) {
            case EntType::jelly:    tmp = static_cast<short>(mPieces[x][y]->mColour); break;
            case EntType::diamond:      tmp = 6;    break;
            case EntType::block:        tmp = 7;    break;
            case EntType::guard:        tmp = 8;    break;
            case EntType::superJelly:
            {
                tmp = static_cast<short>(mPieces[x][y]->mColour);
                tmp |= 32;      // set the "superjelly!" bit
                break;
            }
            case EntType::stuckJelly:
            {
                tmp = static_cast<short>(mPieces[x][y]->mColour);
                tmp |= 64;      // set the "stuck jelly!" bit
                break;
            }
        }
        
        // Consider the "slime bit"
        short slimeBit = mCell[x][y] ^ (255-128);
        
        tmp |= slimeBit;
        
        mCell[x][y] = tmp;
    }
}

// Makes sure that given object becomes part of a
// clickable triplet, by changing its 2
// selected neighbours (1st and last) to its colour.
void cBoard::makeTriplet(int x, int y)
{
    if ( !valid(x, y) ) return;
    std::vector<cEntity*>   adj;    // as in, adjacent
   
    if ( clickable(x-1, y-1) )  adj.push_back(mPieces[x-1][y-1].get());
    if ( clickable(x, y-1) )    adj.push_back(mPieces[x][y-1].get());
    if ( clickable(x+1, y-1) )  adj.push_back(mPieces[x+1][y-1].get());
    if ( clickable(x-1, y) )    adj.push_back(mPieces[x-1][y].get());
    if ( clickable(x+1, y) )    adj.push_back(mPieces[x+1][y].get());
    if ( clickable(x-1, y+1) )  adj.push_back(mPieces[x-1][y+1].get());
    if ( clickable(x, y+1) )    adj.push_back(mPieces[x][y+1].get());
    if ( clickable(x+1, y+1) )  adj.push_back(mPieces[x+1][y+1].get());
    
    (*adj.begin())->switchColour(mPieces[x][y]->mColour);
    (*adj.rbegin())->switchColour(mPieces[x][y]->mColour);
    
    // Do the bookkeeping
    for ( int i = -1; i < 2; ++i )
        for ( int j = -1; j < 2; ++j )
        {
            keepTheBooks(x+i, y+j);
        }
    
}

// Non-clickable things should have a colour of 0.
short cBoard::colourAt(sf::Vector2i v) const
{
    if ( !valid(v.x, v.y) ) return -1;
    return static_cast<short>(mPieces[v.x][v.y]->mColour);
}

short cBoard::neighbourCount(int x, int y) const
{
    if (!valid(x, y)) return 0;
    
    short count = 0;
    
    for ( int i = -1; i < 2; ++i )
        for ( int j = -1; j < 2; ++j )
            if ( ( i != 0 || j !=0 ) && clickable(x+i, y+j) )
                ++count;
    
    return count;
}

void cBoard::set(int x, int y, unsigned short value)
{
    if ( valid(x,y) )
    {
        mCell[x][y] = value;
    }
}

void cBoard::clear()
{
    for ( int i = 0; i < mSizeX; ++i )
        for ( int j = 0; j < mSizeY; ++j )
        {
            mCell[i][j] = 0;
            mPieces[i][j].reset();
        }
}

void cBoard::reCreate(int x, int y, int top, int bottom)
{
    mCell.clear();      // clear cell vectors; will be recreated in init()
    mPieces.clear();    // clear piece vector (unique_ptr; will manage memory itself)

    mSizeX = x;
    mSizeY = y;
    mTop = top;
    mBottom = bottom;
    
    init();
}

cEntity* cBoard::piece(int x, int y) const
{
    return valid(x, y) ? mPieces[x][y].get() : nullptr;
}

void cBoard::remove(int x, int y)
{
    mCell[x][y] = 0;
    mPieces[x][y].reset(nullptr);
}

void cBoard::place(int x, int y, EntType t, EntColour c)
{
    if ( !valid(x, y) ) return;
    
    switch ( t ) {
        case EntType::jelly: {
            std::unique_ptr<cJelly> ptr = spawn<cJelly> ( c );
            ptr->setPos(gkScrLeft + x * gkCellPixSizeX,
                        gkScrTop + (y - mTop) * gkCellPixSizeY);
            mPieces[x][y] = std::move(ptr);
            keepTheBooks(x, y);
            break;
        }
        /*case EntType::superJelly: {
            std::unique_ptr<cSuperJelly> ptr = spawn<cSuperJelly>(c);
            mCell[x][y] |= static_cast<short>(c);
            mCell[x][y] |= 32;
            mPieces[x][y] = std::move(ptr);
            break;
        }*/
        default:
            break;
    }
}

