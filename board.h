#ifndef __not_jelly_splash__board__
#define __not_jelly_splash__board__

#include <vector>
#include "globalConstants.h"
#include <SFML/Graphics.hpp>

class cBoard {
public:
    cBoard(int x = 7, int y = 9, int bottom = 8, int top = 0);
    
    inline bool         valid(int x, int y) const;
    inline bool         closed(int x, int y) const;
    void                init();
    short               at(int x, int y) const;
    void                set(int x, int y, unsigned short value);
    void                clear();
    void                reCreate(int x, int y, int bottom, int top);
    
public:
    int                 mSizeX, mSizeY, mTop, mBottom;

    
private:
    // How to decode a cell?
    // 0 - 2nd bits:    type ( empty / r / g / b / p / diamong / block / guard )
    //      3rd bit:    slime?
    //      4th bit:    super?
    //     negative:    inaccessible

    std::vector<std::vector<short>>     mCell;
};

#endif /* defined(__not_jelly_splash__board__) */
