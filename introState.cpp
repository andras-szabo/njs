#include "introState.h"

cIntroState::cIntroState(cEngine& engine):
cState { engine, "intro" },
mFadeInTimer { sf::seconds(gkIntroFadeInSecs) },
mTimePassed { sf::Time::Zero },
mProceedTimer { sf::seconds(gkIntroProceedSecs) }
{
    
}

void cIntroState::init()
{
    mSprite.setTexture(rTextureHolder.get("sisu_splash"));
    mSprite.setColor(sf::Color(255, 255, 255, 0));
}

void cIntroState::render()
{
    rWindow.draw(mSprite);
}

void cIntroState::run()
{

    // If key pressed, forward to main menu
    
    sf::Event event;
    while ( rWindow.pollEvent(event) )
    {
        if ( event.type == sf::Event::KeyPressed )
        {
            swapRequest("menu");
            clearRequest();                 // tmp: just quit
            return;
        }
    }
    
    // If enough time passed, proceed to main menu
    
    mTimePassed += mClock.restart();
    if ( mTimePassed >= mProceedTimer )
    {
        swapRequest("menu");
        return;
    }
    
    // If no need to proceed yet, do the fade-in until complete

    if ( mTimePassed < mFadeInTimer )
        mSprite.setColor(sf::Color(255, 255, 255, mTimePassed.asSeconds() / mFadeInTimer.asSeconds() * 255));
}

void cIntroState::cleanup()
{
    
}

void cIntroState::reset()
{
    
}