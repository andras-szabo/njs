#include "board.h"

cBoard::cBoard(int x, int y, int bottom, int top):
mSizeX { x }, mSizeY { y }, mBottom { bottom }, mTop { top }
{
    init();
}

void cBoard::init()
{
    // Fill up a board with empty cells (value: 0 )
    for ( int i = 0; i < mSizeX; ++i )
        mCell.push_back(std::vector<short>(mSizeY, 0));
}

inline bool cBoard::valid(int x, int y) const
{
    // Can we refer to a given pair of coordinates as valid?
    // Not if they are out of scope or pointing to inaccessible field.
    return x >= 0 && x < mSizeX && y >= mTop && y <= mBottom && mCell[x][y] != -1;
}

inline bool cBoard::closed(int x, int y) const
{
    return at(x,y) == -1;
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
            mCell[x][y] = 0;
}

void cBoard::reCreate(int x, int y, int top, int bottom)
{
    mCell.clear();      // clear cell vectors; will be recreated in init()

    mSizeX = x;
    mSizeY = y;
    mTop = top;
    mBottom = bottom;
    
    init();
}