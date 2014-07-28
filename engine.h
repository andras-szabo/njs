#ifndef __sstc__engine__
#define __sstc__engine__

#include <SFML/Graphics.hpp>
#include "resourceHolder.h"
#include "globalConstants.h"
#include "state.h"
#include "animation.h"
#include "entityInfo.h"

class cState;

class cEngine {
public:
    cEngine();
    ~cEngine() { };
    
    void        run();
    void        request(SAID, const std::string& = " ");
    void        addHiScore(unsigned int, const std::string&);
    
public:
    sf::RenderWindow                                    mWindow;
    cResourceHolder<sf::Font, FontID>                   mFontHolder;
    cResourceHolder<sf::Texture, std::string>           mTextureHolder;
    cResourceHolder<cAnimation, std::string>            mAnimationHolder;
    cResourceHolder<cEntityInfo, std::string>           mEntityHolder;
    sf::View                                            mView;
    
    // mStrParam is the postbox through which states can pass
    // strings to one another
    std::string                                         mStrParam;
    
private:
    template <typename T>               // Creates an appropriate state and pushes it onto the
    void            launchState();      // state stack.
    
    void            executeStackChanges();
    void            pushState(const std::string&);
    void            popState();
    void            swapState(const std::string&);
    void            clearStateStack();
    void            returnToState(const std::string&);

private:
    std::vector<std::unique_ptr<cState>>    mStateStack;
    SAID            mCurrentStackAction { SAID::nothing };
    std::string     mStackParam { "" };
};

#endif /* defined(__sstc__engine__) */
