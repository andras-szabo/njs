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
    if ( !valid(v.x, v.y) || mMarked[v.x][v.y] || mCell[v.x][v.y] == 0 ) return false;
    
    // If it's only grey slime - yes, that can blow up too
    if ( mCell[v.x][v.y] == gkSlimeBit ) return true;
    
    // What can't blow up? Diamonds, that's what.
    return mPieces[v.x][v.y]->mType != EntType::diamond;
}

bool cBoard::closed(int x, int y) const
{
    return x >= 0 && x < mSizeX && y >= 0 && y < mSizeY && mCell[x][y] == -1;
}

bool cBoard::valid(int x, int y) const
{
    // Can we refer to a given pair of coordinates as valid?
    // Not if they are out of scope or pointing to inaccessible field.
    return x >= 0 && x < mSizeX && y >= 0 && y < mSizeY && mCell[x][y] != -1;
}

short cBoard::at(int x, int y) const
{
    return !valid(x,y) ? -1 : mCell[x][y];
}

// Empty: if 0, or 64 (64 marking an empty grey slime field)
bool cBoard::empty(int x, int y) const
{
    return valid(x,y) && ( mCell[x][y] == 0 || mCell[x][y] == gkSlimeBit );
}

void cBoard::moveEveryone(sf::Vector2f v)
{
    for ( auto& i : mPieces )
        for ( auto& p : i )
            if ( p != nullptr )
                p->move(v);
}

bool cBoard::clickable(int x, int y) const
{
    if ( !valid(x, y) || empty(x,y) ) return false;
    
    // Non-clickable things should have a colour of 0.
    return mPieces[x][y]->mColour != EntColour::random;
    
}

// Makes sure that mCell[] represents the actual
// state of things, i.e. the same info will be
// found here as one would get when looking at
// the unique_pointers of mPieces.
void cBoard::keepTheBooks(int x, int y)
{
    if ( valid(x, y) )
    {
        short int tmp { 0 };
        
        if ( mPieces[x][y] != nullptr )
        {
            auto t = mPieces[x][y]->mType;
            switch ( t ) {
                case EntType::jelly:
                {
                    tmp = static_cast<short>(mPieces[x][y]->mColour);
                    if ( mPieces[x][y]->mSuper )
                    {
                        tmp |= gkSuperBit;      // set the superjelly bit
                    }
                    if ( mPieces[x][y]->mLives > 1 )
                    {
                        tmp |= gkStuckBit;      // set the "stuck" bit
                    }
                    break;
                }
                case EntType::diamond:      tmp = 6;    break;
                case EntType::block:        tmp = 7;    break;
                case EntType::guard:        tmp = 8;    break;
            }
        }
        
        // Consider the "slime bit"
        short slimeBit = mCell[x][y] & gkSlimeBit;
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
  
    for ( int i = -1; i < 2; ++i )
        for ( int j = -1; j < 2; ++j )
            if ( ( i != 0 || j != 0 ) && clickable(x+i, y+j) )
                adj.push_back(mPieces[x+i][y+j].get());

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
    if ( !clickable(v.x, v.y) ) return -1;
    return static_cast<short>(mPieces[v.x][v.y]->mColour);
}

bool cBoard::normal(int x, int y) const
{
    // Non-empty, non-super, non-stuck jelly, that's the norm.
    return valid(x,y) && !empty(x, y) && mPieces[x][y]->mType == EntType::jelly && !mPieces[x][y]->mSuper
    && mPieces[x][y]->mLives == 1;
}

bool cBoard::guard(int x, int y) const
{
    return valid(x,y) && !empty(x,y) && mPieces[x][y]->mType == EntType::guard;
}

bool cBoard::slime(int x, int y) const
{
    // Check whether the slime bit is 1.
    // Note: this doesn't check for validity, because there
    // might be times when we need to check as-yet invisible
    // squares for being slimy too.
    return valid(x, y) && (mCell[x][y] & gkSlimeBit);
}

// Is there at least one free space in given column?
// (If there is, it'll move down, making space e.g.
// for spawning diamonds)
bool cBoard::freeColumn(int x) const
{
    if ( x < 0 || x >= mSizeX ) return false;
    for ( int i = mTop; i <= mBottom; ++i )
    {
        if ( empty(x,i) ) return true;
    }
    return false;
}

bool cBoard::diamond(int x, int y) const
{
    return valid(x, y) && !empty(x, y) && mPieces[x][y]->mType == EntType::diamond;
}

bool cBoard::fallible(int x, int y) const
{
    return valid(x, y) && !empty(x, y) && mPieces[x][y]->mFallible;
}

short cBoard::neighbourCount(int x, int y) const
{
    if (!valid(x, y) || empty(x, y)) return 0;
    
    short count = 0;
    
    for ( int i = -1; i < 2; ++i )
        for ( int j = -1; j < 2; ++j )
            if ( ( i != 0 || j !=0 ) && clickable(x+i, y+j) )
                ++count;
    
    return count;
}

// From (p,q), fall to (x,y), which is an adjacent
// cell below.
void cBoard::executeFall(int p, int q, int x, int y)
{
    mPieces[x][y] = std::move(mPieces[p][q]);
    
    // Make sure that bookkeeping is done, and
    // e.g. slime stays where it should
    
    keepTheBooks(p, q);
    keepTheBooks(x, y);
    
    // Now all we need to do is tell the piece that
    // it has to move.
    
    mPieces[x][y]->setGoal(gkScrLeft + x * gkCellPixSizeX, gkScrTop + (y - mTop) * gkCellPixSizeY);
}

void cBoard::set(int x, int y, short value)
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
    if ( !valid(x, y) || empty(x,y) ) return nullptr;
    return mPieces[x][y].get();
}

void cBoard::remove(int x, int y)
{
    mPieces[x][y].reset(nullptr);
    keepTheBooks(x, y);
}

int cBoard::getHighestDiamond() const
{
    int highest { mBottom };
    for ( int j = mTop; j < mBottom; ++j )  // here, strict < is enough
    {
        for ( int i = 0; i < mSizeX; ++i )
        {
            if ( diamond(i, j) )
            {
                highest = j;
                break;
            }
        }
        if ( highest != mBottom)
            break;
    }
    return highest;
}

int cBoard::getHighestStuck() const
{
    int highest { mBottom };
    for ( int j = mTop; j < mBottom; ++j )
    {
        for ( int i = 0; i < mSizeX; ++i )
        {
            if ( valid(i,j) && !empty(i, j) && mPieces[i][j]->mLives > 1 )
            {
                highest = j;
                break;
            }
        }
        if ( highest != mBottom)
            break;
    }
    return highest;
}

void cBoard::place(int x, int y, EntType t, EntColour c, bool super)
{
    if ( !valid(x, y) ) return;
    
    // If something already here: kill it, and replace it
    // with new entity.
    
    if ( !empty(x, y) )
    {
        remove(x,y);
    }
    
    switch ( t ) {
        case EntType::jelly: {
            if ( c == EntColour::random ) { c = static_cast<EntColour>(rand() % 5 + 1 ); }
            std::string id;
            switch ( c ) {
                case EntColour::red:    id = "redJelly"; break;
                case EntColour::blue:   id = "blueJelly"; break;
                case EntColour::green:  id = "greenJelly"; break;
                case EntColour::yellow: id = "yellowJelly"; break;
                case EntColour::purple: id = "purpleJelly"; break;
                default: break;
            }
            std::unique_ptr<cJelly> ptr = spawn<cJelly> ( id );
            
            ptr->setPos(gkScrLeft + x * gkCellPixSizeX,
                        gkScrTop + (y - mTop) * gkCellPixSizeY);
            
            if ( super ) ptr->makeSuper();
            
            mPieces[x][y] = std::move(ptr);
            keepTheBooks(x, y);
            break;
        }
        case EntType::guard: {
            std::unique_ptr<cGuard> ptr = spawn<cGuard> ("guard");
            ptr->setPos(gkScrLeft + x * gkCellPixSizeX,
                        gkScrTop + (y - mTop) * gkCellPixSizeY);
            mPieces[x][y] = std::move(ptr);
            keepTheBooks(x, y);
            break;
        }
        case EntType::diamond: {
            std::unique_ptr<cDiamond> ptr = spawn<cDiamond> ("diamond");
            ptr->setPos(gkScrLeft + x * gkCellPixSizeX,
                        gkScrTop + (y - mTop) * gkCellPixSizeY);
            mPieces[x][y] = std::move(ptr);
            keepTheBooks(x, y);
            break;
        }
        default:
            break;
    }
}

