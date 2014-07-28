#ifndef __sstc__state__
#define __sstc__state__

#include <SFML/Graphics.hpp>
#include "engine.h"
#include "globalConstants.h"

class cEngine;

class cState {
public:
    cState(cEngine&, const std::string&);
    
    virtual void        init() = 0;
    virtual void        run() = 0;
    virtual void        cleanup() = 0;
    virtual void        reset() = 0;
    virtual void        render() = 0;
    
    inline void         deactivate() { mIsActive = false; }
    
protected:
    void    pushRequest(std::string);       // Request new state to be pushed on top of current one
    void    popRequest();                   // Request to pop current state
    void    swapRequest(std::string);       // Request to pop current state and push sg on it instead
    void    returnRequest(std::string);     // Request to keep popping states until reaching the one given in prm
    void    clearRequest();                 // Request to clear state stack => to terminate the program
    
protected:
    cEngine&                                rEngine;            // Ref to engine
    sf::RenderWindow&                       rWindow;            // Reference to renderwindow
    sf::View&                               rView;              // reference to renderview
    cResourceHolder<sf::Font, FontID>&      rFontHolder;        // ... to resource holders
    cResourceHolder<sf::Texture, TexID>&    rTextureHolder;
    bool                                    mIsActive;
    
public:
    std::string                             mName;
};
#endif /* defined(__sstc__state__) */
