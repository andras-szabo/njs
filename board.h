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

private:
    template<class T>
    std::unique_ptr<T>    spawn(EntColour colour)
    {
        std::unique_ptr<T> ptr { new T(rEngine, colour) };
        return std::move(ptr);
    }
    
public:
    int                 mSizeX, mSizeY, mTop, mBottom;

private:
    // How to decode a cell?
    // 0 - 3rd bits:    type ( empty / r / g / b / y / p / diamond / block / guard )
    //      4rd bit:    slime?
    //      5th bit:    super?
    //     negative:    inaccessible
    // so: 0: empty, 1: red, 2: green, 3: blue, 4: yellow, 5: purple, 6: 

    std::vector<std::vector<short>>                     mCell;
    std::vector<std::vector<std::unique_ptr<cEntity>>>  mPieces;
    cEngine&                                            rEngine;
    
};

#endif /* defined(__not_jelly_splash__board__) */
