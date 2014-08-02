#include "messageState.h"

#include "messageState.h"

cMessageState::cMessageState(cEngine& eng):
cState { eng, "message" }
{
    mBackground.setSize(sf::Vector2f { 800, 600 });
    mBackground.setPosition(sf::Vector2f { 200, 100 });
    mBackground.setOutlineColor(sf::Color::Green);
    mBackground.setOutlineThickness(5.0);
    mBackground.setFillColor(sf::Color(10, 20, 10, 230));
    
    tMessageOne.setFont(rFontHolder.get(FontID::defaultFont));
    tMessageOne.setCharacterSize(28);
    
    tMessageTwo = tMessageOne;
}

void cMessageState::render()
{
    rWindow.draw(mBackground);
    rWindow.draw(tTitle);
    rWindow.draw(tMessageOne);
    rWindow.draw(tMessageTwo);
}

void cMessageState::init()
{
    mIsActive = true;
    
    // Check if the message to be displayed is a special
    // kind, e.g. __VICTORY__ or __DEFEAT__
    
    if ( rEngine.mStrParam == "__DEFEAT__" || rEngine.mStrParam == "__VICTORY__" )
    {
        tTitle = tMessageOne;
        tTitle.setColor(sf::Color::Green);
        bool victory = rEngine.mStrParam != "__DEFEAT__";
        std::string stmp = victory ? "Yay!" : "Oh noes";
        tTitle.setString(stmp);
        
        stmp = victory ? "You won! :)\n" : "You lost. :(\n";
        
        tMessageOne.setString(stmp);
        
        sf::Vector2f pos;
        pos.x = rView.getCenter().x - tMessageOne.getLocalBounds().width / 2.0;
        pos.y = rView.getCenter().y - tMessageOne.getLocalBounds().height / 2.0;
        
        tMessageOne.setPosition(pos);
        tMessageOne.setColor(sf::Color::White);
        
        tMessageTwo.setColor(sf::Color::White);
        tMessageTwo.setPosition(pos.x + 140, pos.y + 200);
        
    }
    else
    {
        tTitle = tMessageOne;
        tTitle.setColor(sf::Color::Green);
        tTitle.setString(rEngine.mStrParam);
        
        tMessageOne.setString(rEngine.mStrOtherParam);
        tMessageOne.setColor(sf::Color::White);
    }
    
    sf::Vector2f pos;
    pos.x = (rView.getSize().x - tTitle.getLocalBounds().width) / 2.0;
    pos.y = 150;
    tTitle.setPosition(pos);

    pos.x = rView.getCenter().x - tMessageOne.getLocalBounds().width / 2.0;
    pos.y = rView.getCenter().y - tMessageOne.getLocalBounds().height / 2.0;
    tMessageOne.setPosition(pos);
    

}

void cMessageState::run()
{
    sf::Event event;
    while ( rWindow.pollEvent(event) )
    {
        if ( event.type == sf::Event::Closed )
        {
            clearRequest();
        }
        
        if ( event.type == sf::Event::KeyPressed )
        {
            if ( rEngine.mStrParam == "__VICTORY__" || rEngine.mStrParam == "__DEFEAT__" )
            {
                returnRequest("menu");
            }
            else
            {
                popRequest();
            }
        }
    }
}

void cMessageState::reset()
{
    
}

void cMessageState::cleanup()
{
    
}