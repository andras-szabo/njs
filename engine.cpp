#include "engine.h"
#include "introState.h"

cEngine::cEngine()
{
    mWindow.create(sf::VideoMode(gkWindowSize.x, gkWindowSize.y), " not jelly splash " + gkVersion);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setKeyRepeatEnabled(false);
    
    // Load fonts, textures
    mTextureHolder.load(TexID::introScreen, "sisu_splash.png");
    
    mView.setSize(gkViewSize.x, gkViewSize.y);
    mView.setCenter(gkViewSize.x / 2, gkViewSize.y / 2);
    mView.setViewport(sf::FloatRect(0,0,1,1));

    mWindow.setView(mView);
    
}

template <typename T>
void cEngine::launchState()
{

    // Create new state ( initializing through constructor to which "*this,"
    // i.e. ref to the engine, is passed.
    
    std::unique_ptr<cState> p { new T { *this } };
    p->init();
    mStateStack.push_back(std::move(p));
}

void cEngine::request(SAID action, const std::string& s)
{
    if ( mCurrentStackAction == SAID::nothing )
    {
        mCurrentStackAction = action;
        mStackParam = s;
    }
}

void cEngine::executeStackChanges()
{
    // Parameters expected in mCurrentStackAction and mStackParam
    switch ( mCurrentStackAction ) {
        case SAID::pop:
        {
            popState();
            break;
        }
        case SAID::push:
        {
            pushState(mStackParam);
            break;
        }
        case SAID::swap:
        {
            swapState(mStackParam);
            break;
        }
        case SAID::returnTo:
        {
            returnToState(mStackParam);
            break;
        }
        case SAID::clear:
        {
            clearStateStack();
            break;
        }
        default: break;
    }
    
    // Reset stack requests:
    
    mCurrentStackAction = SAID::nothing;
}

void cEngine::pushState(const std::string& s)
{
    if ( !mStateStack.empty() ) ( *mStateStack.rbegin())->deactivate();
    
    // if ( s == "mainMenu" ) launchState<cMenuState>();
    // if ( s == "game" ) launchState<cGameState>();
    // if ( s == "pause" ) launchState<cPauseState>();
    // if ( s == "controlMenu" ) launchState<cControlState>();
    // if ( s == "hiScore" ) { mStrParam = "__HISCORES__"; launchState<cMessageState>(); }
    // if ( s == "gameOver" ) { mStrParam = "__GAMEOVER__"; launchState<cMessageState>(); }
}

void cEngine::popState()
{
    if ( mStateStack.empty() ) return;
 
    // If state stack not empty: clean up after state that's being popped,
    // then simply pop it. mStateStack is a vector of unique_ptrs, so
    // no need to worry about memory leaks.
    
    (*mStateStack.rbegin())->cleanup();
    mStateStack.pop_back();
    if ( !mStateStack.empty() )
        (*mStateStack.rbegin())->reset();
}

void cEngine::swapState(const std::string& s)
{
    // Pops the current state and pushes onto the stack a new one to
    // replace it.
    
    popState();
    if ( !mStateStack.empty() ) (*mStateStack.rbegin())->deactivate();
    pushState(s);
}

void cEngine::returnToState(const std::string& s)
{
    
    // Pop states until finding requested one. If no such state,
    // this will terminate the program.
    
    while (!mStateStack.empty() && (*mStateStack.rbegin())->mName != s )
        popState();
}

void cEngine::clearStateStack()
{
    // Clear state stack: terminate program
    
    while ( !mStateStack.empty() )
        popState();
}

void cEngine::run()
{
    launchState<cIntroState>();
    
    // Main loop
    
    while ( !mStateStack.empty() )
    {
        mWindow.clear();
        
        for ( const auto& i : mStateStack )
            i->render();

        mWindow.display();
        (*mStateStack.rbegin())->run();
        
        executeStackChanges();
    }
}