#include "menuState.h"

#include "menuState.h"

cMenuState::cMenuState(cEngine& eng):
cState { eng, "menu" }
{
    std::vector<std::string> stmp;
    stmp.push_back("SCORE");
    stmp.push_back("SLIME");
    stmp.push_back("DIAMONDS");
    stmp.push_back("STUCK");
    stmp.push_back("OPEN");
    stmp.push_back("HORRIBLE");
    stmp.push_back("Quit");
    
    
    mMainMenu.setup( stmp, rFontHolder.get(FontID::defaultFont), 36, sf::Vector2f(-rView.getSize().x, 390));
    
    tTitle.setString("DEFINITELY\n\tNOT\n\tJELLY SPLASH");
    tTitle.setFont(rFontHolder.get(FontID::defaultFont));
    tTitle.setCharacterSize(50);
    tTitle.setColor(sf::Color::Green);
    tTitle.setPosition(260, 100);
    
    tCredits = tTitle;
    tCredits.setCharacterSize(12);
    
    std::string s = "2014. Andras Szabo - SFML: sfml-dev.org - Font design by kenney";
    tCredits.setString(s);
    tCredits.setColor(sf::Color(10,50,10));
    tCredits.setPosition(rView.getCenter().x - tCredits.getLocalBounds().width / 2.0, 760);
}

void cMenuState::init()
{
    mIsActive = true;
}

void cMenuState::reset()
{
    mIsActive = true;
    mMainMenu.reset();
}

void cMenuState::render()
{
    rWindow.draw(tCredits);
    if ( !mIsActive ) return;
    rWindow.draw(tTitle);
    mMainMenu.render();
}

void cMenuState::run()
{
    if ( mMainMenu.update() )
    {
        auto s = mMainMenu.getSelectedItem();
        
        if ( s == "Quit" )
        {
            clearRequest();
        } else
        {
            rEngine.mStrParam = s;
            pushRequest("game");
        }

    }
}

void cMenuState::cleanup()
{
    
}