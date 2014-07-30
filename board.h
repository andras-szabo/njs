#ifndef __not_jelly_splash__board__
#define __not_jelly_splash__board__

#include <vector>
#include "globalConstants.h"
#include <SFML/Graphics.hpp>
#include "entity.h"
#include "resourceHolder.h"
#include "engine.h"
#include "entityInfo.h"

class cBoard {
public:
    cBoard(cEngine& engine, int x = 7, int y = 9, int bottom = 8, int top = 0);
    
    inline bool         valid(int x, int y) const;
    inline bool         closed(int x, int y) const { return at(x,y) == -1; }
    void                init();
    short               at(int x, int y) const;
    void                set(int x, int y, unsigned short value);
    void                clear();
    void                reCreate(int x, int y, int bottom, int top);
    
    cEntity*            piece(int x, int y) const;
    void                place(int x, int y, EntType t, EntColour c = EntColour::random);
    bool                clickable(int x, int y) const;
    short               neighbourCount(int x, int y) const;
    void                makeTriplet(int x, int y);
    short               colourAt(sf::Vector2i) const;
    bool                marked(sf::Vector2i) const;
    void                mark(sf::Vector2i, bool);
    void                resetMarked();
    bool                canBlowUp(sf::Vector2i);
    void                remove(int x, int y);

private:
    template<class T>
    std::unique_ptr<T>    spawn(EntColour colour)
    {
        std::unique_ptr<T> ptr { new T(rEngine, colour) };
        return std::move(ptr);
    }
    void                keepTheBooks(int, int);
    
public:
    int                 mSizeX, mSizeY, mTop, mBottom;

private:
    // mCell stores everything about the piece in one, i.e. its type and colour.
    // -1: not accessible, 0: empty.
    // If not 0:
    // 1 - 5: colour: red, green, blue, yellow, purple.
    // 6: diamond, 7: block, 8: guard
    // So this covers the first 1 bits.
    // The rest are sign bits:
    // bit16: superjelly?
    // bit32: stuckjelly?
    // bit64: slime?
    // To get at these: XOR with 255-16, 255-32, 255-64, jne.

    std::vector<std::vector<short>>                     mCell;
    std::vector<std::vector<std::unique_ptr<cEntity>>>  mPieces;
    std::vector<std::vector<bool>>                      mMarked;
    cEngine&                                            rEngine;
    
};

#endif /* defined(__not_jelly_splash__board__) */
