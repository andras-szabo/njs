#include "state.h"

// Initializes references to engine and resources
cState::cState(cEngine& engine, const std::string& n):
rEngine { engine },
rWindow { engine.mWindow },
rView { engine.mView },
rFontHolder { engine.mFontHolder },
rTextureHolder { engine.mTextureHolder },
mIsActive { false },
mName { n }
{
    
}

void cState::pushRequest(std::string s)
{
    mIsActive = false;
    rEngine.request(SAID::push, s);
}

void cState::popRequest()
{
    rEngine.request(SAID::pop);
}

void cState::swapRequest(std::string s)
{
    rEngine.request(SAID::swap, s);
}

void cState::returnRequest(std::string s)
{
    rEngine.request(SAID::returnTo, s);
}

void cState::clearRequest()
{
    rEngine.request(SAID::clear);
}