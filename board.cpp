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
        
        // Bit cumbersome with unique ptrs, but so be it
        
        std::vector<std::unique_ptr<cEntity>> col;
        for (int j = 0; j < mSizeY; ++j )
        {
            col.push_back(std::move(nullptr));
        }
        
        mPieces.push_back(std::move(col));
    }
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

void cBoard::place(int x, int y, EntType t, EntColour c)
{
    if ( !valid(x, y) ) return;
    
    switch ( t ) {
        case EntType::jelly: {
            std::unique_ptr<cJelly> ptr = spawn<cJelly> ( c );
            mCell[x][y] |= static_cast<short>(c);
            mPieces[x][y] = std::move(ptr);
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

